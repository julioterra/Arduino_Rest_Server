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

char req_end_pattern[] = {"\r\n"};
char element_div[] = {'/',' '};
char services_sense_names_arrays [][15] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};
char services_act_names_arrays [][15] = {"output_1", "output_2", "output_3", "output_4"};

int services_sense_values [] = {0,0,0,0,0,0};
boolean services_sense_requested [] = {false,false,false,false,false,false,};
int services_act_values [] = {0,0,0,0};
boolean services_act_requested [] = {false,false,false,false};

char request_msg [REQUEST_MAX_LENGTH];
int request_msg_index = 0;

long last_reading = 0;
long reading_interval = 3000000;
int end_of_request_counter = 0;

boolean process_request = false;

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
  
}

void loop()
{
  // listen for incoming clients
  Client client = server.available();
  // run();
  if (client) {

    // CONNECTED TO CLIENT
    while (client.connected()) {

      // DATA AVAILABLE FROM CLIENT: if there is a client connected then receive 
      // their request and processes it.
      if (client.available()) {

        // read data from client and save data into the request_msg array
        char c = client.read();        
		process_request = client_request(c);
		if (process_request) {
			// send a standard http response header
			parse_request(request_msg);
			send_response(client);
			// CONSIDER NEW METHOD: save_new_state();
			prepare_for_next_client();
			break;          
        }
		/* END: INSIDE THE NEW LIBRARY */
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}


boolean client_request(char new_char) {
	boolean _process_request = false;
    request_msg [request_msg_index] = new_char;
    request_msg_index++;        

	// CHECK IF REQUEST IS DONE: if done set process_request to true
	int req_end_pattern_length = strlen(req_end_pattern);
    if (!_process_request && new_char == req_end_pattern[req_end_pattern_length-1]) {
        _process_request = true;
		
		// check if we found a sequence of chars that match the end_pattern
		int msg_end_index = request_match_string(req_end_pattern, request_msg, request_msg_index-req_end_pattern_length);
        if (msg_end_index != -1) {
			// remove request end pattern from the request
			request_slice(request_msg, 0, request_msg_index-req_end_pattern_length);
			// remove any content after the second space from the request
			msg_end_index = request_find(' ', request_msg, check_start(request_msg, 0));
            msg_end_index = request_find(' ', request_msg, check_start(request_msg, msg_end_index));
            if (msg_end_index != -1) { 
				request_slice(request_msg, 0, msg_end_index); 
			}
		}
	}
	return _process_request;
}

void prepare_for_next_client() {
    request_clear();
    end_of_request_counter = 0;
    request_msg_index = 0;	
}
