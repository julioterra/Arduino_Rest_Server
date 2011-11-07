/*
  Web  Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 4 Sep 2010
 by Tom Igoe
 
 */

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
#define REQUEST_LENGTH 75
#define ELEMENT_DIV_COUNT  2

char element_div[] = {'/',' '};
char services_sense_names_arrays [][15] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};
char services_act_names_arrays [][15] = {"output_1", "output_2", "output_3", "output_4"};

int services_sense_values [] = {0,0,0,0,0,0};
boolean services_sense_requested [] = {false,false,false,false,false,false,};
int services_act_values [] = {0,0,0,0};
boolean services_act_requested [] = {false,false,false,false};

char request_msg [REQUEST_LENGTH];
int request_msg_index = 0;

long last_reading = 0;
long reading_interval = 3000000;
int end_of_request_counter = 0;

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
  run();
  if (client) {

    // CONNECTED TO CLIENT
    while (client.connected()) {

      // DATA AVAILABLE FROM CLIENT: if there is a client connected then receive 
      // their request and processes it.
      if (client.available()) {

        // read data from client and save data into the request_msg array
        char c = client.read();        
        boolean process_request = false;
        request_msg [request_msg_index] = c;
        request_msg_index++;        

        // REQUEST DONE: once we received "\r\n\r\n" the http message has ended 
        // delete the last 4 chars from the message and set process_request to true
//** MAKE END OF MESSAGE CUSTOMIZABLE - use find start method with full sequence of chars
        if (!process_request && c == '\r' || c == '\n') {
            end_of_request_counter++;  
            if (end_of_request_counter >= 4) {
                process_request = true;
                request_msg_index = 0;
                int msg_end_index = index_of(' ', request_msg,(index_of(' ', request_msg, 0)+1));
                if (msg_end_index != -1) delete_end(request_msg, msg_end_index + 1);
            }       
        } else {
            end_of_request_counter=0;  	
		}

        // PROCESS REQUEST: if process_request is set to true then parse the request
        if (process_request == true) {
          // send a standard http response header
          parse_request(request_msg, request_msg_index);
          send_response(client);

          clear_request();
          process_request = false;
          end_of_request_counter = 0;
          request_msg_index = 0;

          break;          
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
