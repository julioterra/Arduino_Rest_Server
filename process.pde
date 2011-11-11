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
		Serial.print("[parse_request] END: process_state "); Serial.print(process_state);		
	    Serial.print(" request.msg: '"); Serial.print(request.msg); Serial.println("'");
	}
}


void read_services() {        
                           
    // Serial.print("[read_services] New request received. Length: "); Serial.print(request.length);
    // Serial.print(" Request: "); Serial.println(request.msg);
	
	int next_start_pos = 0;
	boolean processing_request = true;
	
    while(processing_request == true) {

		// re-initializing the start and end position of current element 
		int cur_start_pos = next_start_pos;
		next_start_pos = next_element(cur_start_pos);

		// Serial.print("[read_services] processing element from position "); Serial.print(cur_start_pos);
		// Serial.print(" to "); Serial.println(next_start_pos);

		// if no nex
		if (next_start_pos == NO_MATCH) {
			next_start_pos = request.length - 1;
			processing_request = false;
		}

		// Loop through services of the two different types
		for (int j = 0; j < SERVICE_TYPES; j++) {
			// Loop through each service of current service type
			for (int i = 0; i < services[j]; i++) {
				int match_index = service_match(j, cur_start_pos, i);
				if (match_index != NO_MATCH) next_start_pos = match_index;					    
			} 
		}
    }    
}

int service_match(int _service_type, int _start_pos, int _service_array_index) {
	int match_index = NO_MATCH;
	_start_pos = check_start(_start_pos);
	
	// match resquest for GET services
    if (_service_type == 0) {
        match_index = request.match_string(services_sense_names_arrays[_service_array_index], _start_pos);
        if (match_index != NO_MATCH) services_sense_requested[_service_array_index] = true;

	// match resquest for UPDATE services
    } else if (_service_type == 1) {
        match_index = request.match_string(services_act_names_arrays[_service_array_index], _start_pos);
        if (match_index != NO_MATCH) {
			services_act_requested[_service_array_index] = true;
			
			// check if there is a set state message (number) following the service name
			int temp_index = state_match(_service_type, (match_index + 1), _service_array_index);
			if (temp_index != NO_MATCH) {
				match_index = temp_index;					 
			}
		}
    }	
	return match_index;
}

// state_match: check if the next element in the
// request is a number to set this service's/resource's current state.
int state_match(int _service_type, int _start_pos, int _service_array_index) {
	// check if for a state message (a number following an UPDATE-capable service)
	int new_number = check_for_state_msg(_start_pos);
	if (new_number != NO_MATCH) {
		// if match exists, then 
		services_act_values[_service_array_index] = new_number; 
		new_number = next_element(_start_pos);
	}  
	return new_number;
}
