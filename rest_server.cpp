#include "rest_server.h"

RestServer::RestServer() {
	request = Message();
	for (int i = 0; i < GET_SERVICES_COUNT; i++) service_get_state [i] = 0;
	for (int i = 0; i < SET_SERVICES_COUNT; i++) service_set_state [i] = 0;
	prepare_for_next_client();	

	div_chars[0] = '/';
	div_chars[1] = ' ';
	end_sequence[0] = '\r';
	end_sequence[1] = '\n';
	services[0] = GET_SERVICES_COUNT;
	services[1] = SET_SERVICES_COUNT;
	start_time = 0;
	timeout_period = 10 * 1000;
}

/***************************
 * PUBLIC METHODS
 **/

void RestServer::new_client() {
	// if (process_state == -1) { 
		process_state = 0;
		// Serial.print("[RestServer::new_client] state change to process_state: "); Serial.println(process_state);	
	// }
}

boolean RestServer::handle_requests(Client _client) {
	if (_client.available()) read_request(_client.read());
	parse_request();
	process();
	if (process_state == 2) { return true; }
	return false;
}

boolean RestServer::handle_requests() {
	if (Serial.available()) read_request(Serial.read());
	parse_request();
	process();
	if (process_state == 2) { 
		return true; 
	}
	return false;
}

void RestServer::respond() {
	if (process_state == 2) {
		process_state = 3;
		// Serial.print("[RestServer::respond] state change to process_state: "); Serial.println(process_state);
	}
}

boolean RestServer::handle_response(Client _client) {
	send_response(_client);
	prepare_for_next_client();	
	if (process_state == -1) { return true; }
	return false;
}

boolean RestServer::handle_response() {
	send_response();
	prepare_for_next_client();	
	if (process_state == -1) { 
		process_state = 0;
		return true; 
	}
	return false;
}


/***************************
 TOP-LEVEL METHODS
 */
boolean RestServer::read_request(char new_char) {
	if (process_state == 0) {
		if (request.length == 0) start_time = millis();

		boolean _process_request = false;
		request.add(new_char);
				
		// END SEQUENCE: check if end sequence reached
	    if (!_process_request && new_char == end_sequence[END_SEQ_LENGTH-1]) {
		
			// check if we found a sequence of chars that match the end_pattern
			int msg_end_index = request.match_string(end_sequence, request.length-END_SEQ_LENGTH);
	        if (msg_end_index != NO_MATCH) {
				process_state = 1;

				// remove request end pattern from the request
				request.slice(0, request.length-END_SEQ_LENGTH);
				msg_end_index = request.find(' ', 0) + 1;
	            msg_end_index = request.find(' ', msg_end_index);
	            if (msg_end_index != NO_MATCH) request.slice(0, msg_end_index); 

				// Serial.print("[RestServer::read_request] request completed: "); Serial.print(request.msg);
				// Serial.print(" state changed to process_state: "); Serial.println(process_state);
			}
		}

		// REQUEST LENGTH: check if request reached max length	
		if (request.length >= REQUEST_MAX_LENGTH-1) process_state = 1;
		if ((millis() - start_time > timeout_period) && request.length > 5) process_state = 1;			

		return _process_request;
	}
}

void RestServer::parse_request () {
	if (process_state == 1) {
	    int root_index = 0;
	    for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = false; }
	    for (int i = 0; i < SET_SERVICES_COUNT; i ++) { service_set_requested [i] = false; }

	    // Serial.println("[parse_request] starting to parsing request.msg"); Serial.println(request.msg);

	    int match_index = request.match_string("GET ", root_index);
	    if (match_index != NO_MATCH) {
			request.slice((match_index + 1), request.length);

	        // ROOT REQUEST: check for root request 
	        match_index = request.match_string("/ ", root_index);
	        if (match_index != NO_MATCH || request.length <= 1) {
		        // Serial.println("[RestServer::parse_request] request contains ROOT request ");
	            for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = true; }
	            for (int i = 0; i < SET_SERVICES_COUNT; i ++) { service_set_requested [i] = true; }
	        } 

	        // if the request was not a root request then look for different services
	        else if (match_index == NO_MATCH){
		        // Serial.println("[parse_request] request not root request: ");
		        match_index = request.match_string("/all", root_index);
				if (match_index != NO_MATCH) {
			        // Serial.println("[parse_request] request contains ALL request ");
		            for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = true; }
		            for (int i = 0; i < SET_SERVICES_COUNT; i ++) { service_set_requested [i] = true; }
				}
	            read_services();
	        }
	    } 
		process_state = 2;
		// Serial.print("[RestServer::parse_request] END: process_state "); Serial.print(process_state);		
		// Serial.print(" request.msg: '"); Serial.print(request.msg); Serial.println("'");
	}
}

void RestServer::process() {
	if (process_state == 2) {
		boolean requested = false;
		for (int i = 0; i < GET_SERVICES_COUNT; i++) if (service_get_requested [i] == true) requested = true;
		for (int i = 0; i < SET_SERVICES_COUNT; i++) {
			if (service_set_requested [i] == true) requested = true;
			if (service_set_updated [i] == true) requested = true;
		}	

		if (CALLBACK == 0 || !requested) {
			process_state = 3;    
			// Serial.print("[RestServer::process] state change to process_state: "); Serial.println(process_state);	
		} 		
	}
}

void RestServer::send_response(Client _client) {
	if (process_state == 3) {
	    _client.println("HTTP/1.1 200 OK");
	    _client.println("Content-Type: text/html");
	    _client.println();

	    // _client.println("GET services: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 6; i++) {
	        if (service_get_requested[i]) {
	            _client.print(services_sense_names_arrays[i]);
	            _client.print(": ");
	            _client.print(service_get_state[i]);
	            _client.println("<br />");
	        }
	    }
	    // _client.println("<br />");

	    // _client.println("SET services: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 4; i++) {
	        if (service_set_requested[i]) {
	            _client.print(services_act_names_arrays[i]);
	            _client.print(": ");
	            _client.print(service_set_state[i]);
	            _client.println("<br />");
	        }
	    }
		send_response();
		process_state = 4;
		// Serial.print("[RestServer::send_response(client)] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::send_response() {
	if (process_state == 3) {
	    Serial.println("HTTP/1.1 200 OK");
	    Serial.println("Content-Type: text/html");
	    Serial.println();

	    // Serial.println("GET services: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 6; i++) {
	        if (service_get_requested[i]) {
	            Serial.print(services_sense_names_arrays[i]);
	            Serial.print(": ");
	            Serial.print(service_get_state[i]);
	            Serial.println("<br />");
	        }
	    }

	    // Serial.println("SET services: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 4; i++) {
	        if (service_set_requested[i]) {
	            Serial.print(services_act_names_arrays[i]);
	            Serial.print(": ");
	            Serial.print(service_set_state[i]);
	            Serial.println("<br />");
	        }
	    }
		process_state = 4;
		// Serial.print("[RestServer::send_response(serial)] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::prepare_for_next_client() {
	if (process_state == 4) {
		request.clear();
		for (int i = 0; i < GET_SERVICES_COUNT; i++) service_get_requested [i] = false;
		for (int i = 0; i < SET_SERVICES_COUNT; i++) {
			service_set_requested [i] = false;
			service_set_updated [i] = false;
		}	
		process_state = -1;
		// Serial.print("[RestServer::prepare_for_next_client] state change to process_state: "); Serial.println(process_state);	
	}
}

/***************************
 * HELPER METHODS
 **/

/* next_element(char*, int)
 *	accepts a string (char array) and a start index. looks for the
 *	next element by searching for divs in the div_chars array.
 *	This array can be user defined based on how they want to structure
 *	and separate their messages. Standard separators include ' ' and '/ '  
 *  returns the location of the next div element if one is found
 *	otherwise, it returns NO_MATCH.
 */
int RestServer::next_element(int _start) {
	_start = check_start(_start);
	if (_start >= request.length) return NO_MATCH;

	// loop through each element of div_chars array to check for element end
	int match_index = NO_MATCH;
	for (int i = 0; i < ELEMENT_DIV_LENGTH; i ++ ) { 
		int new_index = request.find(div_chars[i], _start);
		// if no previous match, or current match index is smaller, then update match_index
		if (new_index != NO_MATCH) {
			if (match_index == NO_MATCH || (new_index < match_index)) {
				match_index = new_index;	
			} 
		} 		 
	}
	return match_index;
}

/* check_for_state_msg(char*, int)
 *	accepts a string (char array) and a start index. looks for state
 *	information sent as a number. If number is found it is returned.  
 *  Otherwise a NO_MATCH is returned.
 */
int RestServer::check_for_state_msg(int _start) {
	_start = check_start(_start);
	if (_start >= request.length) return NO_MATCH;
    
	int end_index = next_element(_start);
    if (end_index == NO_MATCH) { 
		end_index = request.length - 1;
    } else if (request.msg[end_index] == '/' || request.msg[end_index] == ' ') {
		end_index -= 1; 
	}
   
	int new_num = request.to_i(_start, end_index);
    return new_num;
}

/* check_start(char*, int)
 *	accepts a string (char array) and a start index. looks at the first
 *	element of the message to see if it is a separator, ' ' and '/ ',
 *	and if so, it moves the start index one space over.  
 *  returns the location of the element not including the starting
 *	separator element.
 */
int RestServer::check_start(int _start) {
	if (check_start_single(_start) == _start + 1) return check_start_single(_start + 1);
	return _start;
}

int RestServer::check_start_single(int _start) {
	for (int i = 0; i < ELEMENT_DIV_LENGTH; i ++ ) {
		if (request.msg[_start] == div_chars[i]) return (_start + 1);
	}
	return _start;	
}

void RestServer::read_services() {        
                           
    // Serial.print("[RestServer::read_services] parse services from new request: "); Serial.println(request.msg);
	
	int next_start_pos = 0;
	boolean processing_request = true;
	
    while(processing_request == true) {

		// re-initializing the start and end position of current element 
		int cur_start_pos = next_start_pos;
		next_start_pos = next_element(cur_start_pos);

		// if no nex
		if (next_start_pos == NO_MATCH) {
			next_start_pos = request.length - 1;
			processing_request = false;
		}

		// Loop through services of the two different types
		for (int j = 0; j < SERVICE_TYPES; j++) {
			// Loop through each service of current service type
			for (int i = 0; i < services[j]; i++) {
				int match_index = service_match(j, cur_start_pos, i);
				if (match_index != NO_MATCH) { 
					next_start_pos = match_index; 
					if (next_start_pos >= request.length) break;
				}
			} 
		}
    }    
}

int RestServer::service_match(int _service_type, int _start_pos, int _service_array_index) {
	int match_index = NO_MATCH;
	_start_pos = check_start(_start_pos);
	
	// match resquest for GET services
    if (_service_type == GET_SERVICES) {
		get_service_GET(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) { 
			service_get_requested[_service_array_index] = true;
			// Serial.print("[RestServer::service_match] matched a GET new service: "); Serial.println(current_service);	
		}

	// match resquest for UPDATE services
    } else if (_service_type == SET_SERVICES) {
		get_service_UPDATE(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) {
			service_set_requested[_service_array_index] = true;
			// Serial.print("[RestServer::service_match] matched a new service: "); Serial.println(current_service);	
			
			// check if there is a set state message (number) following the service name
			match_index = state_match(_service_type, (match_index + 1), _service_array_index);
		}
    }	
	return match_index;
}

// state_match: check if the next element in the
// request is a number to set this service's/resource's current state.
int RestServer::state_match(int _service_type, int _start_pos, int _service_array_index) {
	// check if for a state message (a number following an UPDATE-capable service)
	int new_number = check_for_state_msg(_start_pos);
	if (new_number != NO_MATCH) {
		// if match exists, then (1) set updated array to true, (2) update state array 
		service_set_updated[_service_array_index] = true;
		service_set_state[_service_array_index] = new_number; 
		// check the position of the next element
		_start_pos = next_element(_start_pos);
		if (_start_pos == NO_MATCH) _start_pos = request.length;
		// Serial.print("[RestServer::state_match] found a state message: "); Serial.println(_start_pos);	
	}  
	return _start_pos;
}






