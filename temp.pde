
void parse_request (char* _request_msg, int request_index) {
    Serial.print("[parse_request_array] request: ");
    Serial.println(_request_msg);
    
    for (int i = 0; i < 4; i ++) { services_act_requested [i] = false; }
    for (int i = 0; i < 6; i ++) { services_sense_requested [i] = false; }
    Serial.println("[parse_request_array] beginning to parse");

    int match_index = match_string_end("GET ", _request_msg, request_index);
    if (match_index != 1) {
        Serial.print("[parse_request_array] request type: ");
        Serial.println(_request_msg);

        request_index = match_index + 1;
        delete_start(_request_msg, request_index);
        Serial.print("[parse_request_array] delete begining of message for processing: ");
        Serial.println(_request_msg);

        request_index = 0;

        // check if request is a root request. If so, then update the
        // requested arrays.
        match_index = match_string_end("/ ", _request_msg, request_index);
        if (match_index != -1) {
            for (int i = 0; i < 4; i ++) { services_act_requested [i] = true; }
            for (int i = 0; i < 6; i ++) { services_sense_requested [i] = true; }
        } 

        // if the request was not a root request then read through each one
        else if  (match_index == -1){
            read_services(_request_msg, request_index,0);
            read_services(_request_msg, request_index,1);
        }
    } 
}


void read_services(char* _request_msg, int _request_index, int service_types) {        
                           
    // loop through each element in the request
    int request_length = strlen(_request_msg);
    Serial.print("[read_services_array] receiving new request, length: ");
    Serial.print(request_length);
    Serial.print(" request: ");
    Serial.println(_request_msg);

    for(boolean reading = true; reading == true; _request_index = _request_index) {
        int end_index = index_of('/', _request_msg, _request_index);

//        Serial.print("[read_services_array] reading index item ");
//        Serial.print(_request_index);
//        Serial.print(" through ");
//        Serial.println(end_index);

        // MOVE FORWARD ONE SPOT: and go to next iteration of loop to 
        // start again, when first element is a '/'
        if (end_index == _request_index) {
            _request_index += 1;

        // PROCESS THIS ELEMENT
        } else {
            if (end_index == -1){
                end_index = request_length - 1;
                reading = false;
            }
            
            // NUMBER OF SERVICES: confirm number of services based on service
            // type being processed
            int services = 0;
            if (service_types == 0) services = 6;
            else if (service_types == 1) services = 4;
            
//            Serial.print("[read_services_array] number of services to loop through ");
//            Serial.println(services);
 
            // CHECK EACH SERVICE: loop through each service to and process any
            // matches discovered
            for (int i = 0; i < services; i++) {
                int cur_index = -1;
                
                // FIND SERVICES IN REQUESTS: check an element of the request msg to the available
                // service names on this arduino. If match found then set requested array to true.
                if (service_types == 0) {
                    cur_index = match_string_end(services_sense_names_arrays[i], _request_msg, _request_index);
                    if (cur_index != -1) services_sense_requested[i] = true;
                } else if (service_types == 1) {
                    cur_index = match_string_end(services_act_names_arrays[i], _request_msg, _request_index);
                    if (cur_index != -1) services_act_requested[i] = true;
                }

                // FOUND SERVICES IN REQUESTS: when cur_index returns a number greater then
                // 1 this means that a service name has been found in the request.
                if (cur_index != -1) {
                    // POTENTIAL STATE DATA AVAILABLE: check if the next element in the
                    // request is a number to set the service/resource state
                    int new_number = return_number(_request_msg, cur_index + 1);
                    if (new_number != -1) {

                        // NUMBER FOUND: a number if available, so if this is an  
                        // actuator service then set the current value.
                        if (service_types == 1) { services_act_values[i] = new_number; }
 

                        // Check if more elements exist in the request
                        cur_index = next_element(_request_msg, cur_index + 1);

                        // NO MORE ELEMENTS IN REQUEST: stop iterating by setting reading to false,
                        // and set i to services value, to exit the services loop                        
                        if (cur_index == -1 || cur_index >= request_length) {
                            reading = false;
                            i = services;   
                        }
                        
                        // ANOTHER ELEMENT EXISTS: prepare the _request_index for the next iteration
                        // set i to services value to exit current services loop
                        else if (cur_index < request_length) {
                            _request_index = cur_index;
                            i = services;   
                        } 
                        break;
                    }  
                } else if (i == (services-1)){
                     _request_index = next_element(_request_msg, _request_index);
                     if (_request_index == -1) {
                         reading = false;
                     }
                } 
            }            
        }
    }    
}
