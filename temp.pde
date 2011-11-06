void read_services_array(char* _request_msg, 
                         int _request_index,
                         int sense_act_flag) {        
                           
                           
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

        if (end_index == _request_index) {
            _request_index += 1;

        } else {
            if (end_index == -1){
                end_index = request_length - 1;
                reading = false;
            }
            
            int items = 0;
            if (sense_act_flag == 0) items = 6;
            else if (sense_act_flag == 1) items = 4;
            
//            Serial.print("[read_services_array] number of items to loop through ");
//            Serial.println(items);
 
            for (int i = 0; i < items; i++) {
                int cur_index = -1;
                if (sense_act_flag == 0) {
                    cur_index = match_string_end(services_sense_names_arrays[i], _request_msg, _request_index);
                    if (cur_index != -1) services_sense_requested[i] = true;
                } else if (sense_act_flag == 1) {
                    cur_index = match_string_end(services_act_names_arrays[i], _request_msg, _request_index);
                    if (cur_index != -1) services_act_requested[i] = true;
                }

//                Serial.print("[read_services_array] results of attempts to find services ");
//                Serial.println(cur_index);

                if (cur_index != -1) {
                    // check a state value (int) follows the service number
                    int new_number = return_number(_request_msg, cur_index + 1);
                    // if there is a number then process it
                    if (new_number != -1) {
//                        Serial.print("[read_services_array] MATCH FOUND and NUMBER = ");
//                        Serial.println(new_number);
                        if (sense_act_flag == 1) { 
                            services_act_values[i] = new_number; 
                        }
 
                        cur_index = next_element(_request_msg, cur_index + 1);

                        // it we have reached the end of the copy
                        // set i to items in order to end loop                        
                        if (cur_index == -1) {
                            reading = false;
                            i = items;   
                        }
                        
                        // prepare the _request_index variable for next element
                        // set i to items in order to end loop
                        else if (cur_index < request_length) {
                            _request_index = cur_index;
                            i = items;   
                        } 
                        break;
                    }  
                } else if (i == (items-1)){
                     _request_index = next_element(_request_msg, _request_index);
                     if (_request_index == -1) {
                         reading = false;
                     }
                } 
            }            
        }
    }    
}
