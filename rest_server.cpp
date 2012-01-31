/*
 Copyright (c) 2011, Julio Terra

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

// RestSever: RESTfull request Library for Arduino.
// Atmega328 required due to RAM requirements

#include "rest_server.h"

/********************************************************
 PUBLIC METHODS 
	Methods that provide the API for the RestServer. These methods are the 
	only ones that are avaiable from outside of the RestServer class.  

 ********************************************************/

RestServer::RestServer(){	
	request = Message();
	div_chars[0] = '/'; div_chars[1] = ' '; div_chars[2] = '='; div_chars[3] = '&';
	eol_sequence[0] = '\r'; eol_sequence[1] = '\n';
	eoh_sequence[0] = '\r'; eoh_sequence[1] = '\n'; eoh_sequence[2] = '\r'; eoh_sequence[3] = '\n';
	// server_options = B00000001;		
	server_options = 0 | CALLBACK;		
}

void RestServer::register_resources(resource_description_t *_resources_descriptions, int _resources_count){	
	resources_count = _resources_count;

	resources = (resource_t*) malloc(sizeof(resource_t) * int(resources_count));
	if (resources) memset(resources, 0, sizeof(sizeof(resource_t) * int(resources_count)));

	for (int i = 0; i < int(resources_count); i++) {
		for (int k = 0; k < NAME_LENGTH; k ++) resources[i].name[k] = _resources_descriptions[i].name[k];
		resources[i].post_enabled = _resources_descriptions[i].post_enabled;
		resources[i].range.min = _resources_descriptions[i].range.min;
		resources[i].range.max = _resources_descriptions[i].range.max;
		resources[i].state = 0;
	}
	
	// Serial.print("[RestServer::register_resources] resources count = ");
	// Serial.print(int(resources_count));

	prepare_for_next_client();	
}

void RestServer::set_callback(boolean _flag) {
	_flag ? (server_options = server_options | CALLBACK) : (server_options = server_options & (CALLBACK^0x00));
}

void RestServer::set_post_with_get(boolean _flag) {
	_flag ? (server_options = server_options | POST_WITH_GET) : (server_options = server_options & (POST_WITH_GET^0x00));
}

int RestServer::get_server_state() {
	return server_state;
}

boolean RestServer::handle_requests(Stream &_client) {
	if (_client.available()) {
		start_timer();
		read_request(_client.read());
	}
	parse_request();
	process();	
	if (server_state == PROCESS) return true;
	else return false;
}

void RestServer::respond() {
	if (server_state == PROCESS) server_state = RESPOND;
}

boolean RestServer::handle_response(Stream &_client) {
	send_response(_client);
	check_timer();
	prepare_for_next_client();	
	if (server_state == LISTENING) return true;
	else return false;
}

int RestServer::resource_get_state(char *resource_name) {
	for (int i = 0; i < int(resources_count); i++) {
		if (strcmp(resources[i].name, resource_name) == 0) {
			return resources[i].state;
		}
	}
}

int RestServer::resource_get_state(int resource_num) {
	return resources[resource_num].state;	
}

void RestServer::resource_set_state(char *resource_name, int new_state) {
	for (int i = 0; i < int(resources_count); i++) {
		if (strcmp(resources[i].name, resource_name) == 0) {
			resources[i].state = constrain(new_state, 
										   resources[i].range.min, 
										   resources[i].range.max);
		}
	}
}

void RestServer::resource_set_state(int resource_num, int new_state) {
	resources[resource_num].state = constrain(new_state, 
								   resources[resource_num].range.min, 
								   resources[resource_num].range.max);
}

boolean RestServer::resource_post_enabled(char *resource_name) {
	for (int i = 0; i < int(resources_count); i++) {
		if (strcmp(resources[i].name, resource_name) == 0) {
			return resources[i].post_enabled;
		}
	}
}

boolean RestServer::resource_post_enabled(int resource_num) {
	return resources[resource_num].post_enabled;	
}

boolean RestServer::resource_requested(char* resource_name) {
	for (int i = 0; i < int(resources_count); i++) {
		if (strcmp(resources[i].name, resource_name) == 0) {
			return resources[i].get;
		}
	}
}

boolean RestServer::resource_requested(int resource_num) {
	return resources[resource_num].get;	
}

boolean RestServer::resource_updated(char* resource_name){
	for (int i = 0; i < int(resources_count); i++) {
		if (strcmp(resources[i].name, resource_name) == 0) {
			return resources[i].post;
		}
	}
}

boolean RestServer::resource_updated(int resource_num) {
	return resources[resource_num].post;	
}


/********************************************************
 INITIALIZATION METHODS
 ********************************************************/

void RestServer::prepare_for_next_client() {
	if (server_state == RESET) {
		request.clear();

		timeout_start_time = 0; 

		post_read_state = POST_NOT_PROCESSED;
		post_length_expected = 0;
		post_length_actual = 0;

		request_options = B00000000;
	 	for (byte i = 0; i < int(resources_count); i++) {
			resources[i].get = false;
			resources[i].post = false;
		}
		server_state = LISTENING;
	}
}

void RestServer::start_timer() {
	if (server_state == LISTENING) {
		if (request.length == 0) timeout_start_time = millis();
		server_state = READ_VERB;
	} 
}

void RestServer::check_timer() {
	if (millis() - timeout_start_time > TIMEOUT_INTERVAL) server_state = RESET;
}
	
/********************************************************
 READ METHODS
 ********************************************************/
void RestServer::read_request(char new_char) {
	Serial.print(new_char);
	if (server_state == READ_VERB) get_verb(new_char);	

	else if (server_state == READ_RESOURCE) {
		if (request_type == GET_REQUESTS) read_get_requests(new_char);
		else if (request_type == POST_REQUESTS) read_post_requests(new_char);
		if (request.length == REQUEST_MAX_LENGTH-1) server_state = PARSE;
	}
}

void RestServer::get_verb(char new_char) {
	if (server_state == READ_VERB) {
		request.add(new_char);
		
	    int match_index = request.match_string("GET ", request.length-4);
		if (match_index == NO_MATCH) match_index = request.match_string("POST ", request.length-5);
	
		// if a match is found then process the request
	    if (match_index != NO_MATCH) {
			// set the type of request that was found
			if (request.msg[0] == 'G') request_type = GET_REQUESTS;
			else request_type = POST_REQUESTS;

			server_state = READ_RESOURCE;			
			request.clear();
		}
	}
}

void RestServer::read_get_requests(char new_char) {
	if (request_type == GET_REQUESTS) {
		request.add(new_char);
	    if (new_char == eol_sequence[EOL_LENGTH-1]) {
			// check for full end sequence, if match found change server_state and remove end seq
			int msg_end_index = request.match_string(eol_sequence, request.length-EOL_LENGTH);
	        if (msg_end_index != NO_MATCH) {
				request.slice(0, request.length-EOL_LENGTH);

				// remove unused content from end of request (after second space)
	            msg_end_index = request.find(' ', check_start_single(0));
	            if (msg_end_index != NO_MATCH) request.slice(0, msg_end_index); 

				server_state = PARSE;
			}
		}
	}
}

void RestServer::read_post_requests(char new_char) {
	if (request_type == POST_REQUESTS) {
		if (post_read_state == POST_NOT_PROCESSED) {
			if(add_char_and_match(new_char, "Length: ") == true) post_read_state = POST_LENGTH_FOUND;
		}

		else if (post_read_state == POST_LENGTH_FOUND) {
			if ((match_div_char(new_char) || match_eol_char(new_char)) && request.length > 0) {
				post_length_expected = request.to_i(0, request.length-1);
				if (post_length_expected != NO_MATCH) post_read_state = POST_LENGTH_READY;
				request.clear();					
			} 
			else request.add(new_char);								
		} 

		else if (post_read_state == POST_LENGTH_READY) {		
			if (match_eoh_sequence(new_char) == true) post_read_state = POST_READ;
		}

		else if (post_read_state == POST_READ) {		
			request.add(new_char);
			post_length_actual++;
			if (post_length_expected <= post_length_actual) server_state = PARSE;			
		}
	}
}

boolean RestServer::match_eoh_sequence(char new_char) {
	for (byte i = 0; i < EOH_LENGTH - 1; i++) request.msg[i] = request.msg[i+1];
	request.msg[EOH_LENGTH - 1] = new_char;

	if(strncmp(request.msg, eoh_sequence, EOH_LENGTH) == 0) {
		request.clear();
		return true;
	}
	else return false;	
}

boolean RestServer::add_char_and_match(char new_char, char *_match_string) {
	byte match_string_length = byte(strlen(_match_string));
	for (byte i = 0; i < int(match_string_length) - 1; i++) request.msg[i] = request.msg[i+1];
	request.msg[int(match_string_length) - 1] = new_char;

	if(strncmp(request.msg, _match_string, match_string_length) == 0) {
		request.clear();
		return true;		
	}
	else return false;
}


/********************************************************
 PARSE METHODS
 ********************************************************/
void RestServer::parse_request() {
	if (server_state == PARSE) {
	    int start_index = 0;

        // Check for root request 
        int match_index = request.match_string("/", start_index);
        if (match_index != NO_MATCH && request.length == 1) { 
			for (int i = 0; i < int(resources_count); i++) resources[i].get = true;				
			server_state = PROCESS;
			return;
        } 

        // Check if this is a resource information request 
        match_index = request.match_string("/resource_info", start_index);
        if (match_index != NO_MATCH) { 
			request_options = request_options | RESOURCE_REQ;
			server_state = RESPOND;
			return;
        } 

		// see if this is a json request is present
        match_index = request.match_string("/json", start_index);
		if (match_index != NO_MATCH) {
			start_index = match_index + 1;
			if (match_div_char(request.msg[start_index]) || (request.length <= 6)) {
				request_options = request_options | JSON_FORMAT;
				if (request.length <= 6) {
					for (int i = 0; i < int(resources_count); i++) resources[i].get = true;
				}	
			}			
		}
		
		// see if an /all request is present
        match_index = request.match_string("/all", start_index);
		if (match_index != NO_MATCH) {
			start_index = match_index + 1;
			if (match_div_char(request.msg[start_index]) || (request.length <= 5)) {
				for (int i = 0; i < int(resources_count); i++) resources[i].get = true;
			}
		}

		// look for individual service/resource requests
        parse_resources();

		server_state = PROCESS;
	}
}

/* parse_resources()
	Processes the services/resources that are part of a request. This method walks
	through each element from a request and attempts to match each one with the
	available services on the Arduino. Based on these matches this method also 
	updates the services_xxx_requested and services_set_updated arrays, which identify what 
	services were requested or updated by the current request.
	Accepts: n/a
	Returns: n/a
 */
void RestServer::parse_resources() {                              
	int next_start_pos = 0;
	boolean processing_request = true;

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
		for (int i = 0; i < int(resources_count); i++) {
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
int RestServer::service_match(int _r_index, int _start_pos) {
	// check that start pos is not a div char, and that it is smaller than the request's length
	_start_pos = check_start(_start_pos);
	if (_start_pos == NO_MATCH) return NO_MATCH;
	int match_index = NO_MATCH;

	// get current resource name and try to match to current request element
	match_index = request.match_string(resources[_r_index].name, _start_pos);

	if (match_index != NO_MATCH) { 
		resources[_r_index].get = true;
		
		if (resources[_r_index].post_enabled) { 
			if (request_type == POST_REQUESTS || ((server_options & POST_WITH_GET) != 0)) {
				match_index = state_match(_r_index, (match_index + 1));	
			}
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
int RestServer::state_match(int _r_index, int _start_pos) {
	// check if for a state message (a number following an UPDATE-capable service)
	int new_number = check_for_state_msg(_start_pos);

	// if match exists, then (1) set updated array to true, (2) update state array 
	if (new_number != NO_MATCH) {
		resources[_r_index].post = true;
		resources[_r_index].state = constrain(new_number, 
											  resources[_r_index].range.min, 
											  resources[_r_index].range.max);

		// check the position of the next element
		_start_pos = next_element(_start_pos);
		if (_start_pos == NO_MATCH) _start_pos = request.length;
	}  
	return _start_pos;
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
	if (_start == NO_MATCH || _start > request.length-1) return NO_MATCH;
    
	// search for end position of current element and adjust as required
	int end_index = next_element(_start);
    if (end_index == NO_MATCH) end_index = request.length - 1;
    else end_index -= 1; 	// move end index to location before the division character
   
	// try to convert current element into a number by calling request.to_i method
	int new_num = request.to_i(_start, end_index);
	
    return new_num;
}

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



/********************************************************
 PROCESS METHODS
 ********************************************************/
void RestServer::process() {
	if (server_state == PROCESS) {

		boolean service_active = false;
		for (int i = 0; i < int(resources_count); i++) {
			if (resources[i].get || resources[i].post) service_active = true;
		}

		// Update process state if callback is turned off, or no services have been requested or updated
		if (server_options & CALLBACK == 0 || !service_active) server_state = RESPOND;   
	}
}



/********************************************************
 RESPONSE METHODS
 ********************************************************/
void RestServer::send_response(Stream &_client) {
	if (server_state == RESPOND) {

		// handle resource info/description requests
		if ((request_options & RESOURCE_REQ) == RESOURCE_REQ) print_resource_description(_client);

		// handle standard GET and POST requests
		else {
			// handle requests in JSON format
			if ((request_options & JSON_FORMAT) == JSON_FORMAT) print_json(_client);
			// handle HTML requests
			else if ((request_options & JSON_FORMAT) == 0) print_html(_client);
		}

		Serial.print("__END__");

		server_state = RESET;
	}
}

void RestServer::print_html(Stream &_client) {
	if ((request_options & JSON_FORMAT) == 0) {
		print_flash_string(PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"), _client);

	    for(int i = 0; i < int(resources_count); i++) {
			if (resources[i].get || resources[i].post) {
				_client.print(resources[i].name);
				print_flash_string(PSTR(": "), _client); 
				_client.print(resources[i].state); 
				print_flash_string(PSTR("<br />\r\n"), _client);
	        }
	    }
		print_form(_client);
		print_flash_string(PSTR("\r\n\r\n\r\n"), _client); 
	}
}

void RestServer::print_json(Stream &_client) {
	print_flash_string(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"), _client);

	// determine number of of resources that have been requested (via get or post methods)
	byte resources_get_post_count = 0; 	
    for(int i = 0; i < int(resources_count); i++) {
		if (resources[i].get || resources[i].post) resources_get_post_count ++;
	}

	print_flash_string(PSTR("[\r\n"), _client); 
    for(int i = 0; i < int(resources_count); i++) {
		if (resources[i].get || resources[i].post) {
			print_flash_string(PSTR("{\r\n\"resource_name\":\""), _client); 
			_client.print(resources[i].name);
			print_flash_string(PSTR("\",\r\n\"state\":"), _client); 
			_client.print(resources[i].state); 
			print_flash_string(PSTR("\r\n}"), _client);
			if (i < (int(resources_get_post_count) - 1)) {
				print_flash_string(PSTR(","), _client);
			}
			print_flash_string(PSTR("\r\n"), _client);					
        }
    }
	print_flash_string(PSTR("]\r\n\r\n\r\n"), _client); 			
}

void RestServer::print_resource_description(Stream &_client) {
	print_flash_string(PSTR("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"), _client);

	print_flash_string(PSTR("[\r\n"), _client); 
	for(byte i = 0; i < int(resources_count); i++) {
		print_flash_string(PSTR("{\r\n\"resource_name\":\""), _client); 
		_client.print(resources[i].name);
		print_flash_string(PSTR("\",\r\n\"post_enabled\":\""), _client); 
		if (resources[i].post_enabled) print_flash_string(PSTR("true"), _client); 
		else print_flash_string(PSTR("false"), _client); 
		print_flash_string(PSTR("\",\r\n\"range\":{\"min\":"), _client); 
		_client.print(resources[i].range.min);
		print_flash_string(PSTR(",\"max\":"), _client); 
		_client.print(resources[i].range.max);					
		print_flash_string(PSTR("}\r\n}"), _client);
		if (i < (int(resources_count) - 1)) print_flash_string(PSTR(","), _client);
		print_flash_string(PSTR("\r\n"), _client);				
	}
	print_flash_string(PSTR("]\r\n\r\n\r\n"), _client); 
}

void RestServer::print_form(Stream &_client) {	
	if ((request_options & JSON_FORMAT) == 0) {	
		print_flash_string(PSTR("<br />Update State<br />\r\n"), _client);
		print_flash_string(PSTR("<form style='display:inline;' action='"), _client); 
	    for(byte i = 0; i < int(resources_count); i++) { 
			if (resources[i].get && resources[i].post_enabled) {
				print_flash_string(PSTR("/"), _client);  
				_client.print(resources[i].name);	
			}
		}
		print_flash_string(PSTR("' method='POST'>"), _client);

	    for(byte i = 0; i < int(resources_count); i++) {
			if (resources[i].get && resources[i].post_enabled) {
				_client.print(resources[i].name);
				print_flash_string(PSTR(": <input type='text' name='"), _client);
				_client.print(resources[i].name);
				print_flash_string(PSTR("'/><br />\r\n"), _client); 
			}
	    }

		print_flash_string(PSTR("<input type='submit' value='update state'/></form>"), _client);
	}
}

void RestServer::print_flash_string(PGM_P string_const, Stream &_client) {
    char cur_char;
    while ((cur_char = pgm_read_byte(string_const++)) != 0) {
        _client.print(cur_char);
	}
}



/********************************************************
 HELPER METHODS (low-level)
	Methods that provide support for other methods from various different
	categories listed above.  
 /********************************************************/

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
	
	if (match_div_char(request.msg[_start])) return _start + 1;
	else return _start;	
}

// check if _new_char matches any resource div characters (from div_chars array)
boolean RestServer::match_div_char(char _new_char) {
	for (byte i = 0; i < DIV_ELEMENTS; i ++ ) {
		if (_new_char == div_chars[i]) return true;
	}	
	return false;	
}

// check if _new_char matches any end of line characters (from eol_sequence array)
boolean RestServer::match_eol_char(char _new_char) {
	for (byte i = 0; i < EOL_LENGTH; i ++ ) {
		if (_new_char == eol_sequence[i]) return true;
	}	
	return false;	
}