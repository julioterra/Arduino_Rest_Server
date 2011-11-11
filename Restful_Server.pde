/*
  Restful Request Processor
 
 This sketch is the first prototype of a restful request processor
 that takes incoming requests that are structured like restfull requests
 and then responds with the appropriate information. The data and processor
 are linked through simple callback methods and accessible array that hold
 the state for all services/resources.
 
 built from templated created by 
 on 18 Dec 2009 by David A. Mellis and
 modified 4 Sep 2010 by Tom Igoe
 modified 6 Nov 2011 by Julio Terra
 
 */

#include "message.h"
#include "config.h"
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x68, 0xF8 };
byte ip[] = {192,168,2,200};
byte gateway[] = {192,168,2,1};
byte subnet[] = {255,255,0,0};
Server server(7999);

int services_sense_pins [] = {A0, A1, A2, A3, A4, A5};
int services_act_pins [] = {3,5,6,9};

/**********************************************************
 ** Variables that handle the restful message processing **/
// #define REQUEST_MAX_LENGTH 		75
// #define ELEMENT_DIV_COUNT  		2

#define END_SEQ_LENGTH	2
char req_end_pattern[] = {"\r\n"};
char element_div[] = {'/',' '};
char services_sense_names_arrays [][15] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};
char services_act_names_arrays [][15] = {"output_1", "output_2", "output_3", "output_4"};
int services[] = {GET_SERVICES, UPDATE_SERVICES};

int services_sense_values [] = {0,0,0,0,0,0};
boolean services_sense_requested [] = {false,false,false,false,false,false,};
int services_act_values [] = {0,0,0,0};
boolean services_act_requested [] = {false,false,false,false};

char request_msg [REQUEST_MAX_LENGTH];
int request_msg_index = 0;

Message request;

long last_reading = 0;
long reading_interval = 3000000;

// boolean process_request = false;
int process_state;

/** Variables that handle the restful message processing **
 **********************************************************/

void setup()
{
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip, gateway, subnet);
	server.begin();
	Serial.begin(9600);
	for(int i = 0; i < 6; i++) { pinMode(services_sense_pins[i], INPUT); }
	for(int i = 0; i < 4; i++) { pinMode(services_act_pins[i], OUTPUT); }

	process_state = -1;
	request = Message();
  
}

void loop()
{
  // listen for incoming clients
  Client client = server.available();
  // run();
  if (client) {
	new_client();
    // CONNECTED TO CLIENT
    while (client.connected()) {
		read_data();
      // DATA AVAILABLE FROM CLIENT: if there is a client connected then receive 
      // their request and processes it.
      if (client.available()) {
        // read data from client and save data into the request_msg array
        // char c = client.read();        
		if (handle_requests(client.read(), client) == false) {
			write_data();
			break;
		}
		// process_request = server_request(c);
		// if (process_request) {
		// 	// send a standard http response header
		// 	parse_request();
		// 	process_request();
		//  send_response(client);
		// 	prepare_for_next_client();
		// 	break;          
		//         }
		/* END: INSIDE THE NEW LIBRARY */
		}
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

void new_client() {
	if (process_state == -1) { 
		process_state = 0;
		Serial.print("[new_client] state change to process_state: "); Serial.println(process_state);	
	}
}

boolean handle_requests(char _c, Client _client) {
	int starting_state = process_state;
	server_request(_c);
	parse_request();
	process();
	send_response(_client);
	prepare_for_next_client();
	if(process_state == -1) return false;
	else return true;
}

void respond_to_request(Client _client) {
	process();
	send_response(_client);
	prepare_for_next_client();	
}

boolean server_request(char new_char) {
	if (process_state == 0) {

		boolean _process_request = false;
		request.add(new_char);
		
		// Serial.print("[server_request] ADD: "); Serial.print(new_char);
		// Serial.print(" process_state "); Serial.println(process_state);

	    if (!_process_request && new_char == req_end_pattern[END_SEQ_LENGTH-1]) {
			_process_request = true;
		
			// check if we found a sequence of chars that match the end_pattern
			int msg_end_index = request.match_string(req_end_pattern, request.length-END_SEQ_LENGTH);
	        if (msg_end_index != NO_MATCH) {
				process_state = 1;
				// remove request end pattern from the request
				request.slice(0, request.length-END_SEQ_LENGTH);
				msg_end_index = request.find(' ', 0) + 1;
	            msg_end_index = request.find(' ', msg_end_index);
	            if (msg_end_index != NO_MATCH) { 
					request.slice(0, msg_end_index); 
				}

				Serial.print("[server_request] END: "); Serial.print(new_char);
				Serial.print(" state change to process_state: "); Serial.println(process_state);		
			}
		}

		return _process_request;
	}
}

void prepare_for_next_client() {
	if (process_state == 4) {
		process_state = -1;
		request.clear();
		Serial.print("[prepare_for_next_client] state change to process_state: "); Serial.println(process_state);	
	}
}
