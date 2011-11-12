#include "rest_server.h"

RestServer::RestServer() {
	process_state = -1;
	request = Message();
	for (int i = 0; i < GET_SERVICES; i++) service_get_state [i] = 0;
	for (int i = 0; i < UPDATE_SERVICES; i++) service_set_state [i] = 0;
	for (int i = 0; i < GET_SERVICES; i++) service_get_requested [i] = false;
	for (int i = 0; i < UPDATE_SERVICES; i++) {
		service_set_requested [i] = false;
		service_set_updated [i] = false;
	}	
	for (int i = 0; i < ELEMENT_DIV_COUNT; i++) { div_chars[i] = '\0'; }
	for (int i = 0; i < END_SEQ_LENGTH; i++) { end_sequence[i] = '\0'; }
	services[0] = GET_SERVICES;
	services[1] = UPDATE_SERVICES;
	
}

void RestServer::div_and_end() {
	get_end_sequence(end_sequence);
	get_element_div(div_chars);	
	Serial.print("[div_and_end] div element numbers: "); 
	for (int i = 0; i < ELEMENT_DIV_COUNT; i++) {
		Serial.print(int(div_chars[i]));
		Serial.print(", ");
	}
	Serial.println();

	Serial.print("[end_sequence] div element numbers: "); 
	for (int i = 0; i < END_SEQ_LENGTH; i++) {
		Serial.print(int(end_sequence[i]));
		Serial.print(", ");
	}
	Serial.println();
		
	
}
/***************************
 * PUBLIC METHODS
 **/

void RestServer::new_client() {
	if (process_state == -1) { 
		process_state = 0;
		Serial.print("[RestServer::new_client] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::handle_requests(char _c) {
	int starting_state = process_state;
	Serial.print("[RestServer::handle_requests] handling request: "); Serial.println(_c);	
	server_request(_c);
	parse_request();
	process();
}

boolean RestServer::handle_response(Client _client) {
	send_response(_client);
	prepare_for_next_client();	
	if(process_state == -1) { return false; }
	return true;
}

boolean RestServer::request_ready() {
	if (process_state == 2) { return true; }
	return false;    
}

void RestServer::respond() {
	if (process_state == 2) {
		process_state = 3;
		Serial.print("[respond] state change to process_state: "); Serial.println(process_state);
	}
}


/***************************
 * TOP-LEVEL METHODS
 **/

boolean RestServer::server_request(char new_char) {
	if (process_state == 0) {

		boolean _process_request = false;
		request.add(new_char);
		
		Serial.print("[server_request] new char number: "); Serial.print(int(new_char));
		Serial.print(" end_sequence "); Serial.println(process_state);

		// char request_end [ELEMENT_DIV_COUNT];
		// get_end_sequence(request_end); 
		// convert_const(req_end_pattern, request_end);
		
	    if (!_process_request && new_char == end_sequence[END_SEQ_LENGTH-1]) {
			_process_request = true;
			Serial.print("[server_request] found end_sequence potential match "); Serial.println();
		
			// check if we found a sequence of chars that match the end_pattern
			int msg_end_index = request.match_string(end_sequence, request.length-END_SEQ_LENGTH);
	        if (msg_end_index != NO_MATCH) {
				process_state = 1;
				// remove request end pattern from the request
				request.slice(0, request.length-END_SEQ_LENGTH);
				msg_end_index = request.find(' ', 0) + 1;
	            msg_end_index = request.find(' ', msg_end_index);
	            if (msg_end_index != NO_MATCH) { 
					request.slice(0, msg_end_index); 
				}

				Serial.print("[server_request] state change to process_state: "); Serial.println(process_state);		
			}
		}

		return _process_request;
	}
}

void RestServer::parse_request () {
	if (process_state == 1) {
	    int root_index = 0;
	    for (int i = 0; i < 4; i ++) { service_set_requested [i] = false; }
	    for (int i = 0; i < 6; i ++) { service_get_requested [i] = false; }

	    Serial.println("[parse_request] parsing request.msg"); Serial.println(request.msg);

	    int match_index = request.match_string("GET ", root_index);
	    if (match_index != NO_MATCH) {

	        // request_index = match_index + 1;
			request.slice((match_index + 1), request.length);
		    // Serial.print("[parse_request] request type GET confirmed, now deleted from message: '");
		    // Serial.print(request.msg); Serial.println("'");

	        // ROOT REQUEST: check for root request. 
	        // If so, then update requested array to true.
	        match_index = request.match_string("/ ", root_index);
	        if (match_index != NO_MATCH || request.length <= 1) {
		        Serial.println("[parse_request] request contains ROOT request ");
	            for (int i = 0; i < 4; i ++) { service_set_updated [i] = true; }
	            for (int i = 0; i < 6; i ++) { service_get_requested [i] = true; }
	        } 
	        // if the request was not a root request then read through each one
	        else if (match_index == NO_MATCH){
		        // Serial.println("[parse_request] request not root request: ");
		        match_index = request.match_string("/all", root_index);
				if (match_index != NO_MATCH) {
			        // Serial.println("[parse_request] request contains ALL request ");
		            for (int i = 0; i < 4; i ++) { service_set_requested [i] = true; }
		            for (int i = 0; i < 6; i ++) { service_get_requested [i] = true; }
				}
	            read_services();
	        }
	    } 
		process_state = 2;
		Serial.print("[parse_request] END: process_state "); Serial.print(process_state);		
	    Serial.print(" request.msg: '"); Serial.print(request.msg); Serial.println("'");
	}
}

void RestServer::process() {
	if (process_state == 2) {
		if (!GET_CALLBACK && !UPDATE_CALLBACK) {
			process_state = 3;    
			Serial.print("[process] state change to process_state: "); Serial.println(process_state);	
		}	
	}
}

void RestServer::send_response(Client _client) {
	if (process_state == 3) {
	    _client.println("HTTP/1.1 200 OK");
	    _client.println("Content-Type: text/html");
	    _client.println();
	    Serial.println("HTTP/1.1 200 OK");
	    Serial.println("Content-Type: text/html");
	    Serial.println();

	    _client.println("Sensor Resource States: <br />");
	    Serial.println("Sensor Resource States: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 6; i++) {
	        if (service_get_requested[i]) {
	            _client.print(services_sense_names_arrays[i]);
	            _client.print(" = ");
	            _client.print(service_get_state[i]);
	            _client.println("<br />");
	            Serial.print(services_sense_names_arrays[i]);
	            Serial.print(" = ");
	            Serial.print(service_get_state[i]);
	            Serial.println("<br />");
	        }
	    }
	    _client.println("<br />");

	    _client.println("Actuator Resource States: <br />");
	    Serial.println("Actuator Resource States: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 4; i++) {
	        if (service_set_requested[i]) {
	            _client.print(services_act_names_arrays[i]);
	            _client.print(" = ");
	            _client.print(service_set_state[i]);
	            _client.println("<br />");
	            Serial.print(services_act_names_arrays[i]);
	            Serial.print(" = ");
	            Serial.print(service_set_state[i]);
	            Serial.println("<br />");
	        }
	    }
		process_state = 4;
		Serial.print("[send_response] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::prepare_for_next_client() {
	if (process_state == 4) {
		process_state = -1;
		request.clear();
		Serial.print("[prepare_for_next_client] state change to process_state: "); Serial.println(process_state);	
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
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) { 
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
    
	int match_index = next_element(_start);
    if (match_index == NO_MATCH) { 
		match_index = request.length - 1;
    } else if (request.msg[match_index] == '/' || request.msg[match_index] == ' ') {
		match_index -= 1; 
	}
   
	int new_num = request.to_i(_start, match_index);
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
	if (check_start_single(_start) == _start + 1) {
		return check_start_single(_start + 1);
	}
	return _start;
}

int RestServer::check_start_single(int _start) {
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) {
		if (request.msg[_start] == div_chars[i]) {
			return (_start + 1);
		}
	}
	return _start;	
}

void RestServer::read_services() {        
                           
    // Serial.print("[read_services] New request received. Length: "); Serial.print(request.length);
    // Serial.print(" Request: "); Serial.println(request.msg);
	
	int next_start_pos = 0;
	boolean processing_request = true;
	
    while(processing_request == true) {

		// re-initializing the start and end position of current element 
		int cur_start_pos = next_start_pos;
		next_start_pos = next_element(cur_start_pos);

		// Serial.print("[read_services] processing element from position "); Serial.print(cur_start_pos);
		// Serial.print(" to "); Serial.println(next_start_pos);

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
				if (match_index != NO_MATCH) { next_start_pos = match_index; }
			} 
		}
    }    
}

int RestServer::service_match(int _service_type, int _start_pos, int _service_array_index) {
	int match_index = NO_MATCH;
	_start_pos = check_start(_start_pos);
	
	// match resquest for GET services
    if (_service_type == GET_INDEX) {
		get_service_GET(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) { 
			service_get_requested[_service_array_index] = true;
			Serial.print("[RestServer::service_match] matched a GET new service: "); Serial.println(current_service);	
		}

	// match resquest for UPDATE services
    } else if (_service_type == UPDATE_INDEX) {
		get_service_UPDATE(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) {
			service_set_requested[_service_array_index] = true;
			Serial.print("[RestServer::service_match] matched a SET new service: "); Serial.println(current_service);	
			
			// check if there is a set state message (number) following the service name
			int temp_index = state_match(_service_type, (match_index + 1), _service_array_index);
			if (temp_index != NO_MATCH) {
				service_set_updated[_service_array_index] = true;
				match_index = temp_index;					 
				Serial.print("[RestServer::service_match] matched a SET new state: "); Serial.println(current_service);	
			}
		}
		Serial.print("[RestServer::service_match] matched a new service: "); Serial.println(current_service);	
    }	
	return match_index;
}

// state_match: check if the next element in the
// request is a number to set this service's/resource's current state.
int RestServer::state_match(int _service_type, int _start_pos, int _service_array_index) {
	// check if for a state message (a number following an UPDATE-capable service)
	int new_number = check_for_state_msg(_start_pos);
	if (new_number != NO_MATCH) {
		// if match exists, then 
		service_set_state[_service_array_index] = new_number; 
		new_number = next_element(_start_pos);
	}  
	return new_number;
}






