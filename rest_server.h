/*
	The MIT License (MIT)  
	Copyright © Julio Terra, http://www.julioterra.com

	Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
	and associated documentation files (the "Software"), to deal in the Software without restriction, 
	including without limitation the rights to use, copy, modify, merge, publish, distribute, 
	sublicense, and/or sell copies of the Software, and to permit persons to whom the Software 
	is furnished to do so, subject to the following conditions:  
	  
	The above copyright notice and this permission notice shall be included in all copies or 
	substantial portions of the Software.  
	  
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
	BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
	DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  

	RestSever: RESTfull request Library for Arduino.
	Atmega328 required due to RAM requirements
*/

#ifndef __Restful_server_h__
#define __Restful_server_h__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
#endif

#include <stream.h> 
#include <string.h>
#include <avr/pgmspace.h>
#include "config_rest.h"
#include "utility/message.h"

class RestServer {

	private:
		// resource_active_t contains following resource description and state information		
		resource_t *resources;

		byte resources_count;					// holds the number of resources

		Message request;	// holds the resource requests and temporary data during POST req. reading process
					
		boolean debug_code;
		Print &debug_log;

		byte server_state;						// holds state of RestServer		
		byte server_options;					// each bit holds a separate server option including:
		#define CALLBACK			1			//	1. callback enabled (B00000001)
		#define POST_WITH_GET		2			//	2. post with get and form not displayed (B00000010)
		#define JSON_LOCK			4			//	3. set all responses to json format (B00000100)

		byte request_type;						// holds request type (GET or POST)
		byte request_options;					// each bit holds a separate request option including:	
		#define JSON_FORMAT			1			// 	2. return json format (B00000001)
		#define RESOURCE_REQ		2			//	1. resource description request (B00000010)

		byte post_read_state;					// holds current state when reading post requests
		#define POST_NOT_PROCESSED	0			//	0: post not yet processed 
		#define POST_LENGTH_FOUND	1			//	1: post length found
		#define POST_LENGTH_READY	2			//	2: post length has been retrieved
		#define POST_READ			3			//	3: post data has been read
		byte post_length_expected;				// holds the expected post length based on header
		byte post_length_actual;				// holds actual post length 

		// constants that identify the number and type of requests that are
		// supported by the RestServer library
		#define GET_REQUESTS			 0		// set GET_REQUESTS equals 0
		#define POST_REQUESTS			 1		// set POST_REQUESTS equals 1

		// constant used to identify non-matches in several methods from RestServer and Message classes
		#define NO_MATCH				-1		

		char eol_sequence[EOL_LENGTH + 1];		// request end sequence match chars
		char eoh_sequence[EOH_LENGTH + 1];		// request end sequence match chars
		char div_chars[DIV_ELEMENTS + 1];		// element division chars

		long timeout_start_time;				// start time for timeout timer 

		// methods for reading request 
		void read_request(char);
		void get_verb(char);
		void read_get_requests(char);
		void read_post_requests(char);
		boolean match_div_char(char);
		boolean match_eol_char(char);
		boolean match_eoh_sequence(char);
		boolean add_char_and_match(char, char*);

		// methods for parsing request 
		void parse_request();
		void parse_resources();
		int service_match(int, int);
		int state_match(int, int);
		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);

		// methods for processing data in prepartion to respond to request
		void process();

		// methods for responding to request
		void send_response(Stream &_client);
		void print_json(Stream &_client);
		void print_html(Stream &_client);
		void print_resource_description(Stream &_client);
		void print_form(Stream &_client);
		void print_flash_string(PGM_P, Stream &_client);

		// methods for initializing data after and managing timeout
		void prepare_for_next_client();
		void start_timer();
		void check_timer();
		
	public:
		// server_state constants
		#define LISTENING			0
		#define READ_VERB			1
		#define READ_RESOURCE		2	
		#define PARSE				3
		#define PROCESS				4
		#define RESPOND				5
		#define RESET				6

		// initialization and state inquiry methods
		RestServer(Print &_serial);						// initializes the RestServer, pass the Serial port for debugging 
		void register_resources(resource_description_t *, int);
		void set_callback(boolean);						// sets callback option
		void set_post_with_get(boolean);				// sets get with post option
		void set_json_lock(boolean);					// sets json lock option
		void set_debug_code(boolean);					// prints debug messages
		int get_server_state();							// returns current server state

		// client handling methods
		boolean handle_requests(Stream &_client); 		// reads request from client
		void respond();									// notifies rest_server when ready to respond
		boolean handle_response(Stream &_client); 		// sends response to client

		// resource state getter and setter methods
		int resource_get_state(char*);					// get state of named resource
		int resource_get_state(int);					// get state of numbered resource
		void resource_set_state(char*, int);			// set state of named resource
		void resource_set_state(int, int);				// set state of numbered resource

		boolean resource_post_enabled(char*);			// identify if named resource can be updated
		boolean resource_post_enabled(int);				// identify if numbered resource can be updated

		boolean resource_requested(char*);				// identify if named resource was requested
		boolean resource_requested(int);				// identify if numbered resource was requested
		boolean resource_updated(char*);				// identify if named resource's state was updated
		boolean resource_updated(int);					// identify if numbered resource's state was update

};

#endif // endif __Restful_server_h__
