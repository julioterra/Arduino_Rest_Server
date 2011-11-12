#ifndef Restful_server_h
#define Restful_server_h

#include "WProgram.h"
#include "config_rest.h"
#include "message.h"
#include <Ethernet.h>
#include <SPI.h>


class RestServer {

	public:
		/* VARIABLES TO BE USED BY MICROPROCESSOR */
		int services[SERVICE_TYPES];
		Message request;
		int process_state;

		char end_sequence[ELEMENT_DIV_COUNT + 1];
		char div_chars[END_SEQ_LENGTH + 1];
		char current_service [NAME_LENGTH];

		boolean server_request(char);
		void parse_request ();
		void process();
		void send_response(Client);
		void prepare_for_next_client();

		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);

		void read_services();
		int service_match(int, int, int);
		int state_match(int, int, int);

	// public:
		int service_get_state [GET_SERVICES];
		int service_set_state [UPDATE_SERVICES];
		boolean service_get_requested [GET_SERVICES];
		boolean service_set_requested [UPDATE_SERVICES];
		boolean service_set_updated [UPDATE_SERVICES];

		void div_and_end();
		RestServer();
		void new_client();
		boolean handle_requests(Client); 
		void respond();
		boolean handle_response(Client);


};

#endif
