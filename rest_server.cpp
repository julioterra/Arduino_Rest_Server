#include "rest_server.h"

/****************************
 PUBLIC METHODS & CONSTRUCTOR
 */

RestServer::RestServer(){
	request = Message();
	for (int i = 0; i < SERVICES_COUNT; i++) resources[i].state = 0;
	process_state = WAITING;
	prepare_for_next_client();	

	div_chars[0] = '/'; div_chars[1] = ' '; div_chars[2] = '=';
	eol_sequence[0] = '\r'; eol_sequence[1] = '\n';
	eoh_sequence[0] = '\r'; eoh_sequence[1] = '\n'; eoh_sequence[2] = '\r'; eoh_sequence[3] = '\n';
	timeout_start_time = 0; timeout_period = 2 *1000;
	post_ready_to_read = false;
}

boolean RestServer::handle_requests(Stream &_client) {
	// Serial << "[RestServer::handle_requests] current state " << process_state << CRLF;
	if (_client.available()) read_request(_client.read());
	parse_request();
	process();
	
	check_timer();
	if (process_state == PROCESS) return true;
	else return false;
}

void RestServer::respond() {
	if (process_state == PROCESS) {
		process_state = RESPOND;
	}
}

boolean RestServer::handle_response(Stream &_client) {
	send_response(_client);
	prepare_for_next_client();	
	if (process_state == WAITING) return true;
	else return false;
}

/*
 TOP-LEVEL METHODS
	Methods directly accessed by public methods. These methods provide high-level
	functionality such as reading, parsing and responding to requests.  
 */
boolean RestServer::read_request(char new_char) {
	Serial << new_char;

	if (process_state == WAITING) {
		if (request.length == 0) timeout_start_time = millis();
		process_state = READ_VERB;
	} 

	if (process_state == READ_VERB) get_verb(new_char);	

	else if (process_state == READ_RESOURCE) {

		if (request_type == GET_SERVICES) {
			request.add(new_char);
		    if (new_char == eol_sequence[EOL_LENGTH-1]) {
				// check for full end sequence, if match found change process_state and remove end seq
				int msg_end_index = request.match_string(eol_sequence, request.length-EOL_LENGTH);
		        if (msg_end_index != NO_MATCH) {
					request.slice(0, request.length-EOL_LENGTH);

					// remove unused content from end of request (after second space)
		            msg_end_index = request.find(' ', check_start_single(0));
		            if (msg_end_index != NO_MATCH) request.slice(0, msg_end_index); 

					process_state = PARSE;
				}
			}
		}
		
		if (request_type == POST_SERVICES) {
			if(!post_length_found) {
				if(length_match_found(new_char) == true) {
					post_length_found = true;
					// request.clear();
				}				
			}

			else if(post_length_found && !post_length_read) {
				if (div_found(new_char) || eol_found(new_char) && request.length > 0) {
					post_length_expected = request.to_i(0, request.length-1);
					if (post_length_expected != NO_MATCH) post_length_read = true;
					request.clear();					
					// Serial << "[RestServer::read_request] post message length " << post_length_expected << CRLF;					
				} 
				else request.add(new_char);								
			} 
			
			else if (post_length_read && !post_ready_to_read) {
				if (eoh_match_found(new_char) == true) {
					post_ready_to_read = true;
					// Serial << "[RestServer::read_request] ready to read " << CRLF;
				} 					
			}

			else if (post_ready_to_read) {
				// Serial << "[RestServer::read_request] reading post message " << request.msg  << CRLF;					
				request.add(new_char);
				post_length_actual++;
				if (post_length_expected <= post_length_actual) {
					// Serial << "[RestServer::read_request] POST message read fully " << request.msg  << CRLF;					
					process_state = PARSE;
				}
				
			}
		}		
		if (request.length == REQUEST_MAX_LENGTH-1) process_state = PARSE;
	}

	if (process_state == PARSE) return true; 
	else return false;
}

void RestServer::get_verb(char new_char) {
	if (process_state == WAITING) {
		if (request.length == 0) timeout_start_time = millis();
		process_state = READ_VERB;
	} 
	
	if (process_state == READ_VERB) {
	    int root_index = 0;	
		request.add(new_char);
		
	    int match_index = request.match_string("GET ", request.length-4);
		if (match_index == NO_MATCH) match_index = request.match_string("POST ", request.length-5);
	
		// if a match is found then process the request
	    if (match_index != NO_MATCH) {
			// set the type of request that was found
			if (request.msg[0] == 'G') request_type = GET_SERVICES;
			else request_type = POST_SERVICES;

			process_state = READ_RESOURCE;			
			request.clear();
		}
	}
}

void RestServer::parse_request() {
	if (process_state == PARSE) {
	    int root_index = 0;

        // Check for root request 
        int match_index = request.match_string("/", root_index);
        if (match_index != NO_MATCH && request.length == 1) { 
			for (int i = 0; i < SERVICES_COUNT; i++) resources[i].get = true;				
			// Serial << "[RestServer::parse_request] matched root " << request.msg << CRLF;
			process_state = PROCESS;
			return;
        } 

		// see if an /all request is present
        match_index = request.match_string("/all/", root_index);
		if (match_index != NO_MATCH) {
			for (int i = 0; i < SERVICES_COUNT; i++) resources[i].get = true;
			// Serial << "[RestServer::parse_request] matched /all " << CRLF;
		}

		// look for individual service/resource requests
        read_services();

		process_state = PROCESS;
	}
}

void RestServer::process() {
	if (process_state == PROCESS) {
		// Serial << "[RestServer::process] current msg: " << request.msg << CRLF;	

		boolean service_active = false;
		for (int i = 0; i < SERVICES_COUNT; i++) {
			if (resources[i].get || resources[i].post) service_active = true;
		}

		// Update process state if callback is turned off, or no services have been requested or updated
		if (CALLBACK == 0 || !service_active) process_state = RESPOND;   
		// if (process_state == RESPOND) 
			// Serial.print("[RestServer::process] state change to process_state: "); Serial.println(process_state);	
	}
}

void RestServer::send_response(Stream &_client) {
	if (process_state == RESPOND) {
		_client << "HTTP/1.1 200 OK" << CRLF << "Content-Type: text/html" << CRLF << CRLF;

	    for(int i = 0; i < SERVICES_COUNT; i++) {
			if (resources[i].get || resources[i].post) {
				_client << get_service(i) << ": " << resources[i].state;
				if (resources_spec[i].post_enabled) get_form(i, _client);
				_client << "<br />" << CRLF;
	        }
	    }
		// send_response();
		process_state = RESET;
	}
}

void RestServer::prepare_for_next_client() {
	if (process_state == RESET) {
		request.clear();
		post_ready_to_read = false;

		post_length_found = false;
		post_length_read = false;
		post_length_expected = 0;
		post_length_actual = 0;

	 	for (int i = 0; i < SERVICES_COUNT; i++) {
			resources[i].get = false;
			resources[i].post = false;
		}
		process_state = WAITING;
	}
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
                           
	int next_start_pos = 0;
	boolean processing_request = true;

	// Serial << "[RestServer::read_services] current msg: " << request.msg << CRLF;	
	
    while(processing_request == true) {

		// re-initializing the start and end position of current element 
		int cur_start_pos = next_start_pos;
		next_start_pos = next_element(cur_start_pos);

		// if next_start_pos returns a NO_MATCH then we have reached end of resource request 
		if (next_start_pos == NO_MATCH) {
			next_start_pos = request.length - 1;
			processing_request = false;
		}

		// loop through each resource/service name to look for a match
		for (int i = 0; i < SERVICES_COUNT; i++) {
			// Serial << "[RestServer::read_services] checking new resource array: " << current_service << CRLF;	
			int match_index = service_match(i, cur_start_pos);
			if (match_index != NO_MATCH) { 
				next_start_pos = match_index; 
				if (next_start_pos >= request.length) break;
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
int RestServer::service_match(int _service_array_index, int _start_pos) {
	// check that start pos is not a div char, and that it is smaller than the request's length
	_start_pos = check_start(_start_pos);
	if (_start_pos == NO_MATCH) return NO_MATCH;
	int match_index = NO_MATCH;

	// get current resource name and try to match to current request element
	match_index = request.match_string(get_service(_service_array_index), _start_pos);

	if (match_index != NO_MATCH) { 
		resources[_service_array_index].get = true;
		// Serial << "[RestServer::service_match] matched service " << current_service << CRLF;
		
		// if (resources_spec[_service_array_index].post_enabled) {
		if (resources_spec[_service_array_index].post_enabled && request_type == POST_SERVICES) {
			match_index = state_match(_service_array_index, (match_index + 1));	
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
int RestServer::state_match(int _service_array_index, int _start_pos) {
	// check if for a state message (a number following an UPDATE-capable service)
	int new_number = check_for_state_msg(_start_pos);

	// if match exists, then (1) set updated array to true, (2) update state array 
	if (new_number != NO_MATCH) {
		resources[_service_array_index].post = true;
		resources[_service_array_index].state = new_number;

		// check the position of the next element
		_start_pos = next_element(_start_pos);
		if (_start_pos == NO_MATCH) _start_pos = request.length;
	}  
	return _start_pos;
}

void RestServer::get_form(int resource_num, Stream &_client) {		
	_client << "<form style='display:inline;' action='/" << get_service(resource_num) << "' method='POST'>";
	_client << ", update: <input type='text' name='" << get_service(resource_num) << "'/>";
	_client << "<input type='submit' value='update state'/></form>";
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
	for (int i = 0; i < DIV_ELEMENTS; i ++ ) { 
		int new_index = request.find(div_chars[i], _start);

		// if match is found then update the match_index if...
		if (new_index != NO_MATCH) {
			// ... match_index equals NO_MATCH, or new_index is smaller then match_index
			if (match_index == NO_MATCH || (new_index < match_index)) match_index = new_index;	
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
	
	if (div_found(request.msg[_start])) return _start + 1;
	else return _start;	
}

boolean RestServer::div_found(char _new_char) {
	// go throug div_char array to check start pos for div chars, if found update start pos
	for (int i = 0; i < DIV_ELEMENTS; i ++ ) {
		if (_new_char == div_chars[i]) return true;
	}	
	return false;	
}

boolean RestServer::eol_found(char _new_char) {
	// go throug div_char array to check start pos for div chars, if found update start pos
	for (int i = 0; i < EOL_LENGTH; i ++ ) {
		if (_new_char == eol_sequence[i]) return true;
	}	
	return false;	
}

boolean RestServer::eoh_match_found(char new_char) {
	int match_string_length = 4;
	for (int i = 0; i < match_string_length - 1; i++) request.msg[i] = request.msg[i+1];
	request.msg[match_string_length - 1] = new_char;

	if(strncmp(request.msg, eoh_sequence, match_string_length) == 0) {
		// Serial << "[RestServer::eoh_match_found] returning true " << CRLF;
		request.clear();
		return true;
	}
	else return false;	
}

boolean RestServer::length_match_found(char new_char) {
	int match_string_length = 8;
	for (int i = 0; i < match_string_length - 1; i++) request.msg[i] = request.msg[i+1];
	request.msg[match_string_length - 1] = new_char;
	
	if(strncmp(request.msg, "Length: ", match_string_length) == 0) {
		// Serial << "[RestServer::length_match_found] returning true " << CRLF;
		request.clear();
		return true;		
	}
	else return false;
}

void RestServer::check_timer() {
	if (millis() - timeout_start_time > timeout_period) process_state = RESET;
}





