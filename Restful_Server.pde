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

#include <SPI.h>
#include <Ethernet.h>
#include "message.h"
#include "config_rest.h"
#include "rest_server.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x68, 0xF8 };
byte ip[] = {192,168,2,200};
byte gateway[] = {192,168,2,1};
byte subnet[] = {255,255,0,0};
Server server(7999);

int service_get_pins [] = {A0, A1, A2, A3, A4, A5};
int service_act_pins [] = {3,5,6,9};

long request_started = 0;
int request_wait_time = 15 * 1000;
RestServer request_server = RestServer();

long loop_counter = 0;

void setup() {
	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip, gateway, subnet);
	server.begin();
	Serial.begin(9600);
	for(int i = 0; i < 6; i++) { pinMode(service_get_pins[i], INPUT); }
	for(int i = 0; i < 4; i++) { pinMode(service_act_pins[i], OUTPUT); }
}

void loop() {
	// if (loop_counter == 5000) request_server.div_and_end();
	// loop_counter++;

	// listen for incoming clients
	Client client = server.available();
	
	// CONNECTED TO CLIENT
	if (client) {
		request_server.new_client();
		request_started = millis();
	
		while (client.connected()) {

			// read data from client, if available
			if (request_server.handle_requests(client)) {
				read_data();
				write_data();
				request_server.respond();
			}			
			if (request_server.handle_response(client)) break;
			if (millis() - request_started > request_wait_time) break;
		}
		// give the web browser time to receive the data and close connection
		delay(1);
		client.stop();
	}
}

void read_data() {
	for (int j = 0; j < request_server.services[GET_INDEX]; j++) {
		request_server.service_get_state[j] = analogRead(service_get_pins[j]);
	}			
}

void write_data() {
	for (int j = 0; j < request_server.services[UPDATE_INDEX]; j++) {
		request_server.service_set_state[j] = constrain(request_server.service_set_state[j], 0, 1000);
		analogWrite(service_act_pins[j], map(request_server.service_set_state[j], 0, 1000, 0, 255));
	}					
}

