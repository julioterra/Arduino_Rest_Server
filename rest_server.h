#ifndef __Restful_server_h__
#define __Restful_server_h__

#include "WProgram.h"
#include "config_rest.h"
#include <utility/message.h>
#include <../streaming/Streaming.h>
#include "Stream.h" 
#include <string.h>
// #include <Ethernet.h>
// #include "Print.h" 

class RestServer {

	private:
		resource_description_t *resources_description;	// holds description of resources available

		Message request;	// holds the resource requests and temporary data during POST req. reading process
															
		byte process_state;						// holds state of RestServer		
		byte request_type;						// holds request type (GET or POST)

		char eol_sequence[EOL_LENGTH + 1];		// request end sequence match chars
		char eoh_sequence[EOH_LENGTH + 1];		// request end sequence match chars
		char div_chars[DIV_ELEMENTS + 1];		// element division chars

		long timeout_start_time;				// start time for timeout timer 
		int timeout_period;						// interval period for timeout timer 
		
		#define POST_NOT_PROCESSED	0
		#define POST_LENGTH_FOUND	1
		#define POST_LENGTH_READY	2
		#define POST_READ			3
		
		byte post_process_state;
		byte post_length_expected;
		byte post_length_actual;

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

		void get_form(int, Stream &_client);

		int next_element(int);
		int check_for_state_msg(int);
		int check_start(int);
		int check_start_single(int);
		boolean div_found(char);
		boolean eol_found(char);
		boolean eoh_match_found(char);
		boolean length_match_found(char);
		
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
		resource_active_t resources[SERVICES_COUNT];

		RestServer(resource_description_t *);			// constructor
		void respond();								// notifies rest_server when ready to respond
		boolean handle_requests(Stream &_client); 	// reads request from Ethernet client
		boolean handle_response(Stream &_client); 	// sends response to Ethernet client

};



#endif // endif __Restful_server_h__
