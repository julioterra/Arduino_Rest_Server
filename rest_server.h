#ifndef __Restful_server_h__
#define __Restful_server_h__

#include "WProgram.h"
#include "config_rest.h"
#include <utility/message.h>
#include <Ethernet.h>

class RestServer {

	private:
		Message request;		// Current request message
		int process_state;		// state of RestServer based on following criteria
								// 	-1: waiting for client	 	2: processing request 
								//	 0: reading request		 	3: sending response
								//	 1: parsing request			4: cleaning up for next request

		int request_type;
		
		char end_sequence[ELEMENT_DIV_LENGTH + 1];	// request end sequence match chars
		char div_chars[END_SEQ_LENGTH + 1];			// element division chars
		char current_service [NAME_LENGTH];			// name of the current service

		long timeout_start_time;					// timeout timer start time
		int timeout_period;							// timeout timer interval period

		boolean read_request(char);
		void parse_request();
		void process();
		void send_response(Client);
		void send_response();
		void prepare_for_next_client();

		void read_services();
		int service_match(int, int, int);
		int state_match(int, int, int);

		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);

	public:
		int services[SERVICE_TYPES];
		int service_get_state [GET_SERVICES_COUNT];
		int service_set_state [POST_SERVICES_COUNT];
		boolean service_get_requested [GET_SERVICES_COUNT];
		boolean service_set_requested [POST_SERVICES_COUNT];
		boolean service_set_updated [ELEMENT_DIV_LENGTH];

		RestServer();						// constructor
		void respond();						// notifies rest_server when ready to respond
		boolean handle_requests(Client); 	// reads request from Ethernet client
		boolean handle_response(Client); 	// sends response to Ethernet client
		boolean handle_requests(); 			// reads request from Serial client
		boolean handle_response(); 			// sends response to Serial client

};

#endif // endif __Restful_server_h__
