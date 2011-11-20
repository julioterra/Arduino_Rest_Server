#ifndef __Restful_server_h__
#define __Restful_server_h__

#include "WProgram.h"
#include "Stream.h" 

#include "config_rest.h"
#include "utility/message.h"

#include <string.h>

#include <../Streaming/Streaming.h>

class RestServer {

	private:
		resource_description_t *resources_description;	// holds description of resources available
		byte resources_count;							// holds the number of resources

		Message request;	// holds the resource requests and temporary data during POST req. reading process
															
		byte process_state;						// holds state of RestServer		
		byte request_type;						// holds request type (GET or POST)
		byte server_options;					// each bit holds a separate server option including:
		#define CALLBACK			1			//	1. callback enabled (B00000001)
		#define POST_WITH_GET		2			//	2. post with get and form not displayed (B00000010)

		byte request_options;					// each bit holds a separate request option including:	
		#define RESOURCE_REQ		1			//	1. resource description request (B00000001)
		#define JSON_FORMAT			2			// 	2. return json format (B00000010)

		char eol_sequence[EOL_LENGTH + 1];		// request end sequence match chars
		char eoh_sequence[EOH_LENGTH + 1];		// request end sequence match chars
		char div_chars[DIV_ELEMENTS + 1];		// element division chars

		long timeout_start_time;				// start time for timeout timer 

		byte post_read_state;					// holds current state when reading post requests
		#define POST_NOT_PROCESSED	0			//	0: post not yet processed 
		#define POST_LENGTH_FOUND	1			//	1: post length found
		#define POST_LENGTH_READY	2			//	2: post length has been retrieved
		#define POST_READ			3			//	3: post data has been read
		byte post_length_expected;				// holds the expected post length based on header
		byte post_length_actual;				// holds actual post length 

		void read_request(char);
		void parse_request();
		void process();
		void send_response(Stream &_client);
		void prepare_for_next_client();
		void check_timer();

		void get_verb(char);
		void read_get_requests(char);
		void read_post_requests(char);

		void read_services();
		int service_match(int, int);
		int state_match(int, int);

		void get_form(Stream &_client);

		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);

		boolean match_div_char(char);
		boolean match_eol_char(char);
		boolean match_eoh_sequence(char);
		boolean add_char_and_match(char, char*);

		void print_flash_string (PGM_P, Stream &_client);
		
	public:
		// process_state constants
		#define WAITING				0
		#define READ_VERB			1
		#define READ_RESOURCE		2	
		#define PARSE				3
		#define PROCESS				4
		#define RESPOND				5
		#define RESET				6

		// resource_active_t contains following members: int state, boolean get, boolean post		
		resource_active_t *resources;

		RestServer(resource_description_t *, int);		// initializes the RestServer
		void set_callback(boolean);						// sets callback option
		void set_post_with_get(boolean);				// sets get with post option
		boolean handle_requests(Stream &_client); 		// reads request from Ethernet client
		void respond();									// notifies rest_server when ready to respond
		boolean handle_response(Stream &_client); 		// sends response to Ethernet client

};



#endif // endif __Restful_server_h__
