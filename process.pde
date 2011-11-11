void parse_request (char* _request_msg) {
    Serial.print("[parse_request] request: ");
    Serial.println(_request_msg);
    int request_index = 0;

    for (int i = 0; i < 4; i ++) { services_act_requested [i] = false; }
    for (int i = 0; i < 6; i ++) { services_sense_requested [i] = false; }
    Serial.println("[parse_request] beginning to parse");

    int match_index = request_match_string("GET ", _request_msg, request_index);
    if (match_index != -1) {

        request_index = match_index + 1;
		request_slice(_request_msg, request_index, strlen(_request_msg));
        Serial.print("[parse_request] request type GET confirmed, now deleted from message: $");
        Serial.println(_request_msg);

        request_index = 0;

        // check if request is a root request. If so, then update the
        // requested arrays.
        match_index = request_match_string("/ ", _request_msg, request_index);
        if (match_index != -1 || strlen(_request_msg) <= 1) {
	        Serial.println("[parse_request] request contains ROOT request ");
            for (int i = 0; i < 4; i ++) { services_act_requested [i] = true; }
            for (int i = 0; i < 6; i ++) { services_sense_requested [i] = true; }
        } 

        // if the request was not a root request then read through each one
        else if (match_index == -1){
	        Serial.println("[parse_request] request not root request: ");
	        match_index = request_match_string("/all", _request_msg, request_index);
			if (match_index != -1) {
		        Serial.println("[parse_request] request contains ALL request ");
	            for (int i = 0; i < 4; i ++) { services_act_requested [i] = true; }
	            for (int i = 0; i < 6; i ++) { services_sense_requested [i] = true; }
			}
            read_services(_request_msg, request_index,0);
            read_services(_request_msg, request_index,1);

        }
    } 
}


void read_services(char* _request_msg, int _request_index, int service_types) {        
                           
    // loop through each element in the request
    int request_length = strlen(_request_msg);
    Serial.print("[read_services] NEW REQUEST length: ");
    Serial.print(request_length);
    Serial.print(" request: ");
    Serial.println(_request_msg);


    for(boolean reading = true; reading == true; ) {
        // STOP RUNNING: if the request index is equal to or greater then the  
		// length of the request then stop running
        if (_request_index >= request_length || _request_index == -1) {
			reading = false;
			break;

        // START::PROCESS THIS ELEMENT: process the current element
        } else {
		   // find where the next element ends assuming it starts at the 
		   // 
		   int end_index = next_element(_request_msg, _request_index);

	       Serial.print("[read_services] PROCESSING ITEM AT "); Serial.print(_request_index);
	       Serial.print(" TO ");Serial.println(end_index);

            if (end_index == -1) {
				end_index = request_length - 1;
				reading = false;
				Serial.print("[read_services] LAST ITEM: "); Serial.print(_request_index);
				Serial.print(" to "); Serial.println(end_index);
            }
            
            // NUMBER OF SERVICES: confirm number of services based on service
            // type being processed
            int services = 0;
            if (service_types == 0) services = 6;
            else if (service_types == 1) services = 4;
            
            // CHECK EACH SERVICE: loop through each service and process any
            // service/resource name matches discovered
            for (int i = 0; i < services; i++) {
                int match_index = -1;
                
                // FIND SERVICES IN REQUESTS: check an element of the request msg to the available
                // service names on this arduino. If match found then set requested array to true.
				_request_index = check_start(_request_msg, _request_index);
                if (service_types == 0) {
                    match_index = request_match_string(services_sense_names_arrays[i], _request_msg, _request_index);
                    if (match_index != -1) {
						services_sense_requested[i] = true;
						Serial.print("[read_services] MATCH found: ");
						Serial.println(services_sense_names_arrays[i]);
						end_index = match_index + 1;
					}
                } else if (service_types == 1) {
                    match_index = request_match_string(services_act_names_arrays[i], _request_msg, _request_index);
                    if (match_index != -1) {
						services_act_requested[i] = true;
						Serial.print("[read_services] MATCH found: ");
						Serial.println(services_act_names_arrays[i]);
						end_index = match_index + 1;
					}
                }

                // FOUND SERVICES IN REQUESTS: when cur_index returns a number greater then
                // 1 this means that a service name has been found in the request.
                if (match_index != -1) {
	
                    // POTENTIAL STATE DATA AVAILABLE: check if the next element in the
                    // request is a number to set this service's/resource's current state.
					// First move the cur_index forward by one element, so that it points to 
					// the first char of the new element (not the last char of the previous one).
					// int match_index = end_index + 1;
                    int new_number = check_for_state_msg(_request_msg, end_index);
                    if (new_number != -1) {
			           Serial.print("[read_services] STATE NUMBER found: ");
			           Serial.println(new_number);

                        // NUMBER FOUND: a number if available, so if this is an  
                        // actuator service then set the current value.
                        if (service_types == 1) { services_act_values[i] = new_number; }
 

                        // Check if more elements exist in the request
                        end_index = next_element(_request_msg, match_index);

                    }  
                    break;
                } 
            } // END:: SERVICES LOOP

           Serial.print("[read_services] UPDATING INDEX from ");
           Serial.print(_request_index);
            _request_index = end_index;
           Serial.print(" to ");
           Serial.println(_request_index);
        } // END::PROCESS THIS ELEMENT
    }    
}
