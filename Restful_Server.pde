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

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
Server server(7999);

int char_count = 0;
String request;
int end_of_request_counter = 0;

long last_reading = 0;
long reading_interval = 30000;

int services_sense_values [] = {0,0,0,0,0,0};
String services_sense_names [] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};

boolean services_sense_requested [] = {false,false,false,false,false,false,};
int services_sense_pins [] = {A0, A1, A2, A3, A4, A5};

int services_act_values [] = {0,0,0,0};
String services_act_names [] = {"output_1", "output_2", "output_3", "output_4"};

boolean services_act_requested [] = {false,false,false,false};
int services_act_pins [] = {3,5,6,9};


void setup()
{
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.begin(9600);
  
  for(int i = 0; i < 6; i++) {
      pinMode(services_sense_pins[i], INPUT);
  }

  for(int i = 0; i < 4; i++) {
      pinMode(services_act_pins[i], OUTPUT);
  }
  
  char_count = 0;
  request = String(" ");
}

void loop()
{
  // listen for incoming clients
  Client client = server.available();
  run();
  if (client) {
    // an http request ends with a blank line
    while (client.connected()) {

      if (client.available()) {
        char c = client.read();        
        if (char_count != 0) request = request + c;
        else request = c;
        char_count ++;
        boolean end_of_request = false;

        // once we received "\r\n\r\n" the http message has ended 
        // start processing the request
        if (!end_of_request && c == '\r' || c == '\n') {
            end_of_request_counter++;  
            if (end_of_request_counter >=4) {
                end_of_request = true;
                request = request.substring(0, (request.length()-4));
            }       
        }
        if (end_of_request == true) {
          // send a standard http response header
          parse_request(request);
          send_response(client);

          end_of_request = false;
          end_of_request_counter = 0;
          char_count = 0;
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
