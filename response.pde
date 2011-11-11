void run() {
  if (millis() - last_reading > reading_interval) {
//      Serial.println("[run] reading data from sensors and writing to actuators");
      last_reading = millis();
      read_data();
      write_data();
  }
}

void process() {
	if (process_state == 2) {
		if (!GET_CALLBACK && !UPDATE_CALLBACK) {
			process_state = 3;    
			Serial.print("[process] state change to process_state: "); Serial.println(process_state);	
		}	
	}
}

boolean request_ready() {
	if (process_state == 2) return true;
	return false;    
}

void respond() {
	if (process_state == 2) {
		process_state = 3;
		Serial.print("[respond] state change to process_state: "); Serial.println(process_state);	
	}
}

void send_response(Client _client) {
	if (process_state == 3) {
	    _client.println("HTTP/1.1 200 OK");
	    _client.println("Content-Type: text/html");
	    _client.println();
	    Serial.println("HTTP/1.1 200 OK");
	    Serial.println("Content-Type: text/html");
	    Serial.println();

	    _client.println("Sensor Resource States: <br />");
	    Serial.println("Sensor Resource States: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 6; i++) {
	        if (services_sense_requested[i]) {
	            _client.print(services_sense_names_arrays[i]);
	            _client.print(" = ");
	            _client.print(services_sense_values[i]);
	            _client.println("<br />");
	            Serial.print(services_sense_names_arrays[i]);
	            Serial.print(" = ");
	            Serial.print(services_sense_values[i]);
	            Serial.println("<br />");
	        }
	    }
	    _client.println("<br />");

	    _client.println("Actuator Resource States: <br />");
	    Serial.println("Actuator Resource States: <br />");
	    // output the value of each analog input pin
	    for(int i = 0; i < 4; i++) {
	        if (services_act_requested[i]) {
	            _client.print(services_act_names_arrays[i]);
	            _client.print(" = ");
	            _client.print(services_act_values[i]);
	            _client.println("<br />");
	            Serial.print(services_act_names_arrays[i]);
	            Serial.print(" = ");
	            Serial.print(services_act_values[i]);
	            Serial.println("<br />");
	        }
	    }
		process_state = 4;
		Serial.print("[send_response] state change to process_state: "); Serial.println(process_state);	
	}
}

// *************** //
// create API here 
	// consider adding:
		// -individual array accessor and getters
		// -callback methods for requests to set and get data 
void write_data() {
    for(int i = 0; i < 4; i++) {
        if (services_act_pins[i] == 3 || services_act_pins[i] == 5 || 
            services_act_pins[i] == 6 || services_act_pins[i] == 9) { 
            // analogWrite(services_act_pins[i], services_act_values[i]);
        } else {
            // digitalWrite(services_act_pins[i], constrain(services_act_values[i], 0, 1));
        }
    }  
}

void read_data() {
    for(int i = 0; i < 6; i++) {
        if (services_sense_pins[i] >= A0) { 
            services_sense_values[i] = analogRead(services_sense_pins[i]); 
        } else { 
            services_sense_values[i] = digitalRead(services_sense_pins[i]); 
        }
    } 
}


