void read_services_array(char* _request_msg, 
                       int request_index) {
        
    // loop through each element in the request
    int request_length = strlen(_request_msg);
    Serial.print("[read_services_array] receiving new request, length: ");
    Serial.print(request_length);
    Serial.print(" request: ");
    Serial.println(_request_msg);

    for(boolean reading = true; reading == true; request_index = request_index) {
        int end_index = index_of('/', _request_msg, request_index);

        Serial.print("[read_services_array] reading index item ");
        Serial.print(request_index);
        Serial.print(" through ");
        Serial.println(end_index);

        if (end_index == request_index) {
            request_index += 1;

        } else {
            if (end_index != -1) {
//                request_index = end_index + 1;
            } else if (end_index == -1){
                end_index = request_length - 1;
                reading = false;
            }

            for (int i = 0; i < 6; i++) {
                int cur_index = match_string_end(services_sense_names_arrays[i], _request_msg, request_index);
                if (cur_index != -1) { 
                    Serial.print("[read_services_array] MATCH FOUND ending at ");
                    Serial.println(cur_index);
                    services_sense_requested[i] = true;
                    request_index = cur_index + 1;

                    // check is a number follows
                    int new_number = return_number(_request_msg, request_index);
                    if (new_number != -1) {
                        services_sense_values[i] = new_number;  
                        cur_index = next_element(_request_msg, request_index);
                        if (cur_index < (request_length - 1)) {
                            request_index = cur_index;  
                        } else {
                            reading = false;
                        }
                        break;
                    } 
                } 
            }            
        }
    }    
}
