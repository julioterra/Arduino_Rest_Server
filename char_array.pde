

void parse_request_array (char* _request_msg, int request_index) {
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
        match_index = match_string_end("/ ", _request_msg, request_index);
        if (match_index != 1) {
            // then this is an index request
              read_services_array(_request_msg, request_index);
        } else {
//            String act_request = _request_msg,
//            read_next_service(_request_msg, 6, services_sense_names, services_sense_requested, services_sense_values);
//            read_next_service(_request_msg, 4, services_act_names, services_act_requested, services_act_values);
        }
    } 
}

void clear_request(){
    for (int i = 0; i < 50; i++) { request_msg[i] = '\0'; } 
    request_msg_index = 0;   
}

void clear_wip_request(){
    for (int i = 0; i < 25; i++) { wip_request_msg[i] = '\0'; }  
    wip_request_msg_index = 0;  
}

// returns index of next occurence of a char
int index_of(char search_char, char* char_array,  int start) {
    int location = -1;
    int length = strlen(char_array);
    if (length <= start) return location;
    for (int i = start; i < length; i++) {       
      if (search_char == char_array[i]) {
          location = i;
          break;
      }
    }
    Serial.print("[index_of] returning index of char: ");
    Serial.print(search_char);
    Serial.print(" array: ");
    Serial.print(char_array);
    Serial.print(" location: ");
    Serial.println(location);
    return location;
}


// matches two strings starting at start location on master. 
// returns -1 if no match is found, and the index number of 
// where the match ends.
int match_string_end(char* char_search, char* char_master, int start) {
    int length_master = strlen(char_master);
    int length_search = strlen(char_search);
    int location = -1;

    if (length_master < length_search + start) return -1;
    for (int i = 0; i < length_search; i++) {       
      if (char_search[i] == char_master[i+start]) {
           location = i + start;
        } else {
            location = -1;
            break;
        }
    }
//    Serial.print("[match_string_end] seaching for ");
//    Serial.print(char_search);
//    Serial.print(" in: ");
//    Serial.print(char_master);
//    Serial.print(" ends at: ");
//    Serial.println(location);
    return location;  
}

int match_string_start(char* char_search, char* char_master, int start) {
    int length_master = strlen(char_master);
    int length_search = strlen(char_search);
    int location = -1;
    boolean match = false;

    for (int i = start; i < length_master; i++) {
        if (length_master < length_search + i) return -1;
        if (match_string_end(char_search, char_master, i) != -1) {
            location = i;
            break;
        } 
    }
    Serial.print("[match_string_start] seaching for ");
    Serial.print(char_search);
    Serial.print(" in: ");
    Serial.print(char_master);
    Serial.print(" ends at: ");
    Serial.println(location);
    return location;
}

char element_div[] = {'/',' ','\0'};

int next_element(char* char_master, int start) {
  start = start + 1;
  int end_index = -1;
  for (int i = 0; i < 3; i ++ ) { 
    int temp_index = index_of(element_div[i], char_master, start);
    if (temp_index != -1 && end_index != -1) {
        if (temp_index < end_index) { end_index = temp_index; }
    } else if (temp_index != -1 && end_index == -1) {
        end_index = temp_index;
    }
  }
  return end_index;
}

int return_number (char* char_master, int start) {
    start = start + 1;
    int master_length = strlen(char_master);
    if ((master_length-start) < 2) return -1;
    int end_index = index_of('/', char_master, start);

    if (end_index == -1) {
        end_index = index_of(' ', char_master, start);
        if (end_index == -1) { end_index = master_length - 1; }
    } else if (end_index == 0) {
        start += 1;
        return return_number(char_master, start);      
    } 

    int new_number = convert_string2int_array(char_master, start, end_index);

    Serial.print("[return_number] seaching for number in: ");
    Serial.print(char_master);
    Serial.print(" start: ");
    Serial.print(start);
    Serial.print(" end: ");
    Serial.print(end_index);
    Serial.print(" number equals: ");
    Serial.println(new_number);
    
    return new_number;
}

int delete_end (char* char_master, int start) {
    int master_length = strlen(char_master);
    for (int i = start; i < master_length; i++) {
        char_master[i] = '\0';  
    }
}

int delete_start (char* char_master, int start) {
    int master_length = strlen(char_master);
    for (int i = 0; i < master_length; i++) {
        int cur_index = start + i;
        if (cur_index < master_length) { char_master[i] = char_master[cur_index]; }
        else { char_master[i] = '\0'; }
    }
}

int convert_string2int_array(char* number, int start_index, int end_index) {
  int return_num = 0;
  
  int reverse_counter = (end_index - start_index) - 1;

  for(int i = 0; i < (end_index - start_index); i++) {
      char cur_char = number[i+start_index];
      
      if (int(cur_char) < 48 || int(cur_char) > 57) {
        Serial.print("[convert_string2int_array] NOT a NUMBER: ");
        Serial.println(-1);
        return -1;
      }
      int mult = 1;
      for(int j = 0; j < reverse_counter; j++) {
        mult = mult * 10;
      }
      return_num += (int(cur_char)-48) * mult; 
      reverse_counter--;
  }

  Serial.print("[convert_string2int_array] orig number: ");
  Serial.print(number);
  Serial.print(" int number: ");
  Serial.println(return_num);
  return return_num;
}
