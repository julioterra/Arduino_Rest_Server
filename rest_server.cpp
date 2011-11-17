#include "rest_server.h"

/****************************
 PUBLIC METHODS & CONSTRUCTOR
 */

RestServer::RestServer() {
	request = Message();
	for (int i = 0; i < GET_SERVICES_COUNT; i++) service_get_state [i] = 0;
	for (int i = 0; i < POST_SERVICES_COUNT; i++) service_set_state [i] = 0;
	prepare_for_next_client();	

	div_chars[0] = '/';
	div_chars[1] = ' ';
	end_sequence[0] = '\r';
	end_sequence[1] = '\n';
	services[0] = GET_SERVICES_COUNT;
	services[1] = POST_SERVICES_COUNT;
	timeout_start_time = 0;
	timeout_period = 10 * 1000;
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


/*
 TOP-LEVEL METHODS
	Methods directly accessed by public methods. These methods provide high-level
	functionality such as reading, parsing and responding to requests.  
 */
boolean RestServer::read_request(char new_char) {

	// if process_state is -1 then start timeout timer, and set process state to 0
	if (process_state == -1) {
		timeout_start_time = millis();
		process_state = 0;
	}

	if (process_state == 0) {
		request.add(new_char);
				
		// Check if this char is equal to the last char of the end sequence
	    if (new_char == end_sequence[END_SEQ_LENGTH-1]) {
		
			// check for full end sequence
			int msg_end_index = request.match_string(end_sequence, request.length-END_SEQ_LENGTH);
			
			// if match was found then change process_state and remove end seq
	        if (msg_end_index != NO_MATCH) {
				process_state = 1;
				request.slice(0, request.length-END_SEQ_LENGTH);

				// removed unused content from request
				msg_end_index = request.find(' ', 0) + 1;
	            msg_end_index = request.find(' ', msg_end_index);
	            if (msg_end_index != NO_MATCH) request.slice(0, msg_end_index); 

				// Serial.print("[RestServer::read_request] request completed: "); Serial.print(request.msg);
				// Serial.print(" state changed to process_state: "); Serial.println(process_state);
			}
		}

		// check if request reached max length, and if so, try to process the request
		if (request.length >= REQUEST_MAX_LENGTH-1) process_state = 1;

		// check if request has timed out, if so try to process the request
		if ((millis() - timeout_start_time > timeout_period) && request.length > 5) process_state = 1;			
	}

	if (process_state == 1) return true; 
	else return false;
}

void RestServer::parse_request () {
	if (process_state == 1) {
	    int root_index = 0;
	    for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = false; }
	    for (int i = 0; i < POST_SERVICES_COUNT; i ++) { service_set_requested [i] = false; }

	    // Serial.println("[parse_request] starting to parsing request.msg"); Serial.println(request.msg);

		// look for GET and POST requests
	    int match_index = request.match_string("GET ", root_index);
		if (match_index == NO_MATCH) match_index = request.match_string("POST ", root_index);
	
		// if a match is found then process the request
	    if (match_index != NO_MATCH) {
			// set the type of request that was found (0 = GET, 1 = POST)
			if (request.msg[0] == 'G') request_type = 0;
			else request_type = 1;

			// remove the GET or POST verb from the beginning of the request
			request.slice((match_index + 1), request.length);

	        // ROOT REQUEST: check for root request 
	        match_index = request.match_string("/ ", root_index);
	        if (match_index != NO_MATCH || request.length <= 1) {
		        // Serial.println("[RestServer::parse_request] request contains ROOT request ");
	            for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = true; }
	            for (int i = 0; i < POST_SERVICES_COUNT; i ++) { service_set_requested [i] = true; }
	        } 

	        // if the request was not a root request then look for different services
	        else if (match_index == NO_MATCH){
		        // Serial.println("[parse_request] request not root request: ");
		        match_index = request.match_string("/all", root_index);
				if (match_index != NO_MATCH) {
			        // Serial.println("[parse_request] request contains ALL request ");
		            for (int i = 0; i < GET_SERVICES_COUNT; i ++) { service_get_requested [i] = true; }
		            for (int i = 0; i < POST_SERVICES_COUNT; i ++) { service_set_requested [i] = true; }
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

		// check if any services were requested or updated
		boolean service_active = false;
		for (int i = 0; i < GET_SERVICES_COUNT; i++) if (service_get_requested [i] == true) service_active = true;
		for (int i = 0; i < POST_SERVICES_COUNT; i++) {
			if (service_set_requested [i] == true) service_active = true;
			if (service_set_updated [i] == true) service_active = true;
		}	

		// Update process state if callback is turned off, or no services have been requested or updated
		if (CALLBACK == 0 || !service_active) process_state = 3;   
		// if (process_state == 3) Serial.print("[RestServer::process] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::send_response(Client _client) {
	if (process_state == 3) {
        _client << "HTTP/1.1 200 OK" << CRLF << "Content-Type: text/html" << CRLF << CRLF;

	    // output the value of each analog input pin
	    for(int i = 0; i < GET_SERVICES_COUNT; i++) {
	        if (service_get_requested[i]) {
				get_service_GET(i, current_service);
	            _client << current_service << ": " << service_get_state[i] << "<br />" << CRLF;
	        }
	    }

	    // output the value of each analog input pin
	    for(int i = 0; i < POST_SERVICES_COUNT; i++) {
	        if (service_set_requested[i]) {
	            get_service_POST(i, current_service);
	            _client << current_service << ": " << service_set_state[i] << "<br />" << CRLF;
	        }
	    }

		send_response();
		process_state = 4;
		// Serial.print("[RestServer::send_response(client)] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::send_response() {
	if (process_state == 3) {
        Serial << "HTTP/1.1 200 OK" << CRLF << "Content-Type: text/html" << CRLF << CRLF;

	    // output the value of each analog input pin
	    for(int i = 0; i < GET_SERVICES_COUNT; i++) {
	        if (service_get_requested[i]) {
				get_service_GET(i, current_service);
	            Serial << current_service << ": " << service_get_state[i] << "<br />" << CRLF;
	        }
	    }

	    // output the value of each analog input pin
	    for(int i = 0; i < POST_SERVICES_COUNT; i++) {
	        if (service_set_requested[i]) {
	            get_service_POST(i, current_service);
	            Serial << current_service << ": " << service_set_state[i] << "<br />" << CRLF;
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
		for (int i = 0; i < POST_SERVICES_COUNT; i++) {
			service_set_requested [i] = false;
			service_set_updated [i] = false;
		}	
		process_state = -1;
		// Serial.print("[RestServer::prepare_for_next_client] state change to process_state: "); Serial.println(process_state);	
	}
}

/********************************************************
 HELPER METHODS
	Methods that provide support for the top-level and user-interface methods. These
	methods provide lower-level functionality, such as helping to identify the next
	restful service request, service state, etc.  
 */

/* next_element(int)
	Looks for the next restful service element by searching for element division characters 
	specified in the the div_chars array.
	Accepts: index where to start searching for next element within the request. 
	Returns: index position of the next div element if one is found, otherwise, returns NO_MATCH.
 */
int RestServer::next_element(int _start) {
	_start = check_start(_start);
	if (_start == NO_MATCH) return NO_MATCH;
	// if (_start >= request.length) return NO_MATCH;
	int match_index = NO_MATCH;

	// loop through each element of div_chars array to search for a match in the request
	for (int i = 0; i < ELEMENT_DIV_LENGTH; i ++ ) { 
		int new_index = request.find(div_chars[i], _start);

		// if match is found then update the match_index if...
		if (new_index != NO_MATCH) {
			// ... match_index equals NO_MATCH, or new_index is smaller then match_index
			if (match_index == NO_MATCH || (new_index < match_index)) {
				match_index = new_index;	
			} 
		} 		 
	}
	return match_index;
}

/* check_for_state_msg(int)
	Checks if element at current location is a state message. Currently, only positive integers
	state messages are supported.
	Accepts: index where to start searching for state message. 
	Returns: integer that reflects state, if one is found; otherwise, NO_MATCH is returned.
 */
int RestServer::check_for_state_msg(int _start) {
	// check that current start location is not a div character
	_start = check_start(_start);
	if (_start == NO_MATCH) return NO_MATCH;
    
	// search for end position of current element and adjust as required
	int end_index = next_element(_start);
    if (end_index == NO_MATCH) end_index = request.length - 1;
    else end_index -= 1; 
   
	// try to convert current element into a number by calling request.to_i method
	int new_num = request.to_i(_start, end_index);
    return new_num;
}

/* check_start(int)
	Checks the char at current position in char array to determine if it is a div char;
	if so, then it moves the current position forward by one and then checks again.
	Accepts: index where to look for div char with char array.  
	Returns: integer that reflects adjusted index position within char array; if end of 
		array is reached during this process then it returns NO_MATCH
 */
int RestServer::check_start(int _start) {
	if (check_start_single(_start) == _start + 1) return check_start_single(_start + 1);
	return _start;
}

/* check_start_single(int)
	Checks the char at current position in char array to determine if it is a div char;
	if so, then it moves the current position forward by one.
	Accepts: index where to look for div char with char array.  
	Returns: integer that reflects adjusted index position within char array; if end of 
		array is reached during this process then it returns NO_MATCH
 */
int RestServer::check_start_single(int _start) {
	// check if start pos is equal to or greater then request length return NO_MATCH
	if (_start >= request.length) return NO_MATCH;

	// go throug div_char array to check start pos for div chars, if found update start pos
	for (int i = 0; i < ELEMENT_DIV_LENGTH; i ++ ) {
		if (request.msg[_start] == div_chars[i]) return (_start + 1);
	}
	return _start;	
}

/* read_services(int)
	Processes the services/resources that are part of a request. This method walks
	through each element from a request and attempts to match each one with the
	available services on the Arduino. Based on these matches this method also 
	updates the services_xxx_requested and services_set_updated arrays, which identify what 
	services were requested or updated by the current request.
	Accepts: n/a
	Returns: n/a
 */
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
				int match_index = service_match(j, i, cur_start_pos);
				if (match_index != NO_MATCH) { 
					next_start_pos = match_index; 
					if (next_start_pos >= request.length) break;
				}
			} 
		}
    }    
}

/* service_match(int, int, int)
	Checks the element at the current location in the request message against 
	one of the available services on the Arduino. If a SET service is matched
	then this method also attempts to read a service state.
	Accepts: an integer that identifies whether the service type being checked is 
		a get service (0), or a set service (1); the position of the specific service 
		to be checked within the get or set services arrays; and, the position of the 
		current element within the request char array.
	Returns: returns location of the next element within the request, if a service 
		is found; otherwise, returns NO_MATCH  
 */
int RestServer::service_match(int _service_type, int _service_array_index, int _start_pos) {
	// check that start pos is not a div char, and that it is smaller than the request's length
	_start_pos = check_start(_start_pos);
	if (_start_pos == NO_MATCH) return NO_MATCH;
	int match_index = NO_MATCH;
	
	// match resquest for GET services
    if (_service_type == GET_SERVICES) {
		get_service_GET(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) { 
			service_get_requested[_service_array_index] = true;
			// Serial.print("[RestServer::service_match] matched a GET new service: "); Serial.println(current_service);	
		}

	// match resquest for SET services
    } else if (_service_type == POST_SERVICES) {
		get_service_POST(_service_array_index, current_service);
        match_index = request.match_string(current_service, _start_pos);
        if (match_index != NO_MATCH) {
			service_set_requested[_service_array_index] = true;
			// Serial.print("[RestServer::service_match] matched a new service: "); Serial.println(current_service);	
			// check if there is a set state message (number) following the service name
			match_index = state_match(_service_type, _service_array_index, (match_index + 1));
		}
    }	
	return match_index;
}

/* state_match(int, int, int)
	Checks the element at the current location in the request message to determine if
	it is a state message. If state message is found it reads the message into the
	service_set_state array.
	Accepts: an integer that identifies whether the service type being checked is 
		a get service (0), or a set service (1); the position of the specific service 
		to be checked within the get or set services arrays; and, the position of the 
		current element within the request char array.
	Returns: returns location of the next element within the request, if a state message 
		is found; otherwise, returns NO_MATCH  
 */
int RestServer::state_match(int _service_type, int _service_array_index, int _start_pos) {
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





