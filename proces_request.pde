void run() {
  
  if (millis() - last_reading > reading_interval) {
      Serial.println("[run] reading data from sensors and writing to actuators");
      last_reading = millis();
      read_data();
      write_data();
  }
}

void write_data() {
    for(int i = 0; i < 4; i++) {
        if (services_act_pins[i] == 3 || services_act_pins[i] == 5 || 
            services_act_pins[i] == 6 || services_act_pins[i] == 9) { 
            analogWrite(services_act_pins[i], services_act_values[i]);
        } else {
            digitalWrite(services_act_pins[i], constrain(services_act_values[i], 0, 1));
        }
//        Serial.print("[write_data] state of actuators ");
//        Serial.print(services_act_names[i]);
//        Serial.print(": ");
//        Serial.println(services_act_values[i]);
    }  
}

void read_data() {
    for(int i = 0; i < 6; i++) {
      if (services_sense_pins[i] >= A0) { 
          services_sense_values[i] = analogRead(services_sense_pins[i]); 
      } else { 
          services_sense_values[i] = digitalRead(services_sense_pins[i]); 
      }
//      Serial.print("[read_data] state of sensors ");
//      Serial.print(services_sense_names[i]);
//      Serial.print(": ");
//      Serial.println(services_sense_values[i]);
    } 
}

void send_response(Client client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();

    read_data();
    client.println("Sensor Resource States: <br />");
    // output the value of each analog input pin
    for(int i = 0; i < 6; i++) {
        if (services_sense_requested[i]) {
            client.print(services_sense_names[i]);
            client.print(" = ");
            client.print(services_sense_values[i]);
            client.println("<br />");
            Serial.print(services_sense_names[i]);
            Serial.print(" = ");
            Serial.print(services_sense_values[i]);
            Serial.println("<br />");
        }
    }
    client.println("<br />");

    write_data();
    client.println("Actuator Resource States: <br />");
    // output the value of each analog input pin
    for(int i = 0; i < 4; i++) {
        if (services_act_requested[i]) {
            client.print(services_act_names[i]);
            client.print(" = ");
            client.print(services_act_values[i]);
            client.println("<br />");
        }
    }

}

