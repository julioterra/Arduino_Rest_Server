void parse_request () {
	if (process_state == 1) {
	    int root_index = 0;
	    for (int i = 0; i < 4; i ++) { services_act_requested [i] = false; }
	    for (int i = 0; i < 6; i ++) { services_sense_requested [i] = false; }

	    Serial.println("[parse_request] parsing request.msg"); Serial.println(request.msg);

	    int match_index = request.match_string("GET ", root_index);
	    if (match_index != NO_MATCH) {

	        // request_index = match_index + 1;
			request.slice((match_index + 1), request.length);
		    // Serial.print("[parse_request] request type GET confirmed, now deleted from message: '");
		    // Serial.print(request.msg); Serial.println("'");

	        // ROOT REQUEST: check for root request. 
	        // If so, then update requested array to true.
	        match_index = request.match_string("/ ", root_index);
	        if (match_index != NO_MATCH || request.length <= 1) {
		        Serial.println("[parse_request] request contains ROOT request ");
	            for (int i = 0; i < 4; i ++) { services_act_requested [i] = true; }
	            for (int i = 0; i < 6; i ++) { services_sense_requested [i] = true; }
	        } 
	        // if the request was not a root request then read through each one
	        else if (match_index == NO_MATCH){
		        // Serial.println("[parse_request] request not root request: ");
		        match_index = request.match_string("/all", root_index);
				if (match_index != NO_MATCH) {
			        // Serial.println("[parse_request] request contains ALL request ");
		            for (int i = 0; i < 4; i ++) { services_act_requested [i] = true; }
		            for (int i = 0; i < 6; i ++) { services_sense_requested [i] = true; }
				}
	            read_services();
	        }
	    } 
		process_state = 2;
		Serial.print("[parse_request] END: process_state "); Serial.println(process_state);		
	}
}


void read_services() {        
                           
    // Serial.print("[read_services] New request received. Length: "); Serial.print(request.length);
    // Serial.print(" Request: "); Serial.println(request.msg);
	
	int element_end_pos = 0;
	boolean processing_request = true;
	
    while(processing_request == true) {
		// find where the next element ends assuming it starts at the 
		int element_start_pos = element_end_pos;
		element_end_pos = next_element(element_start_pos);

		// Serial.print("[read_services] processing element from position "); Serial.print(element_start_pos);
		// Serial.print(" to "); Serial.println(element_end_pos);

		if (element_end_pos == NO_MATCH) {
			element_end_pos = request.length - 1;
			processing_request = false;
		}

		// Loop through services of the two different types
		int match_index;
		for (int j = 0; j < SERVICE_TYPES; j++) {
			// Loop through each service of current service type
			for (int i = 0; i < services[j]; i++) {
				if ((match_index = service_match(j, element_start_pos, i))  != NO_MATCH) {				
					element_end_pos = match_index;					    
					if ((match_index = state_match(j, element_end_pos, i)) != NO_MATCH) {
						element_end_pos = match_index;					 
					}
				} 
			} 
		}
    }    
}

int service_match(int _service_type, int _start_pos, int _service_array_index) {
	int match_index = NO_MATCH;
    
    // FIND SERVICES IN REQUESTS: check an element of the request msg to the available
    // service names on this arduino. If match found then set requested array to true.
	_start_pos = check_start(_start_pos);
    if (_service_type == 0) {
        match_index = request.match_string(services_sense_names_arrays[_service_array_index], _start_pos);
        if (match_index != NO_MATCH) services_sense_requested[_service_array_index] = true;
    } else if (_service_type == 1) {
        match_index = request.match_string(services_act_names_arrays[_service_array_index], _start_pos);
        if (match_index != NO_MATCH) services_act_requested[_service_array_index] = true;
    }	

	// if match was found look for a state set message (in number format)
	if (match_index != NO_MATCH) { 
		match_index += 1; 
		int temp_index = state_match(_service_type, match_index, _service_array_index);
		if (temp_index != NO_MATCH) {
			match_index = temp_index;					 
		}
	}	
	return match_index;
}

// state_match: check if the next element in the
// request is a number to set this service's/resource's current state.
int state_match(int _service_type, int _start_pos, int _service_array_index) {
	// First move the cur_index forward by one element, so that it points to 
	// the first char of the new element (not the last char of the previous one).
	// int match_index = element_end_pos + 1;
	int new_number = check_for_state_msg(_start_pos);
	if (new_number != NO_MATCH) {

		// NUMBER FOUND: a number if available, so if this is an  
		// actuator service then set the current value.
		if (_service_type == 1) { 
			services_act_values[_service_array_index] = new_number; 
			Serial.print("[state_match] service state found and saved: "); Serial.println(new_number);
		}
		// Check if more elements exist in the request
		new_number = next_element(_start_pos);
	}  
	return new_number;
}
