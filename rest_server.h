#ifndef __Restful_server_h__
#define __Restful_server_h__

#include "WProgram.h"
#include "config_rest.h"
#include <utility/message.h>
#include <../streaming/Streaming.h>
#include <Ethernet.h>
#include <string.h>

class RestServer {

	private:
		#define POST_BUFFER_LENGTH	8
		Message request;					// Current request message
		
		/* Current state of RestServer - -1: waiting for client, 0: reading verb, 
					1: reading request, 2: parsing request, 3: processing request, 
					4: sending response, 5: cleaning up for next request */	
		int process_state;
		
		/* Current request types - 0: GET request, 1: POST request */
		int request_type;				

 		char post_buffer [POST_BUFFER_LENGTH + 1];	// Temporary buffer
		char eol_sequence[EOL_LENGTH + 1];			// request end sequence match chars
		char eoh_sequence[EOH_LENGTH + 1];			// request end sequence match chars
		char div_chars[DIV_ELEMENTS + 1];			// element division chars
		char current_service [NAME_LENGTH];			// name of the current service

		long timeout_start_time;					// timeout timer start time
		int timeout_period;							// timeout timer interval period
		
		boolean ready_to_read;
		boolean post_length_found;
		boolean post_length_read;
		int post_length_expected;
		int post_length_actual;
		boolean header_read;
		boolean body_read;

		boolean read_request(char);
		void get_verb(char);
		void parse_request();
		void process();
		void send_response(Client);
		void send_response();
		void prepare_for_next_client();

		boolean id_eoh(char);
		boolean id_post_length(char new_char);
		void check_timer();

		void read_services();
		int service_match(int, int);
		int state_match(int, int);

		void get_form(int);
		void get_form(int, Client);

		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);
		boolean div_found(char);
		boolean eol_found(char);
		
	public:
		#define WAITING				-1
		#define READ_VERB			 0
		#define READ_RESOURCE		 1	
		#define PARSE				 2
		#define PROCESS				 3
		#define RESPOND				 4
		#define RESET				 5

		struct Resource {
				int state;
				boolean get;
				boolean post;
			};
		
		struct Resource resources[SERVICES_COUNT];

		RestServer();						// constructor
		void respond();						// notifies rest_server when ready to respond
		boolean handle_requests(Client); 	// reads request from Ethernet client
		boolean handle_response(Client); 	// sends response to Ethernet client
		boolean handle_requests(); 			// reads request from Serial client
		boolean handle_response(); 			// sends response to Serial client

};



#endif // endif __Restful_server_h__
