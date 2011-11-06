
void clear_request(){
    for (int i = 0; i < REQUEST_LENGTH; i++) { request_msg[i] = '\0'; } 
    request_msg_index = 0;   
}

// returns index of next occurence of a char
int index_of(char _char_search, char* _char_source,  int start) {
    int location = -1;
    int length = strlen(_char_source);
    if (length <= start) return location;
    for (int i = start; i < length; i++) {       
      if (_char_search == _char_source[i]) {
          location = i;
          break;
      }
    }
    return location;
}


// matches two strings starting at start location on master. 
// returns -1 if no match is found, and the index number of 
// where the match ends.
int match_string_end(char* _char_search, char* _char_source, int start) {
    int length_master = strlen(_char_source);
    int length_search = strlen(_char_search);
    int location = -1;

    if (length_master < length_search + start) return -1;
    for (int i = 0; i < length_search; i++) {       
      if (_char_search[i] == _char_source[i+start]) {
           location = i + start;
        } else {
            location = -1;
            break;
        }
    }
    return location;  
}

//int match_string_start(char* _char_search, char* _char_source, int start) {
//    int length_master = strlen(_char_source);
//    int length_search = strlen(_char_search);
//    int location = -1;
//    boolean match = false;
//
//    for (int i = start; i < length_master; i++) {
//        if (length_master < length_search + i) return -1;
//        if (match_string_end(_char_search, _char_source, i) != -1) {
//            location = i;
//            break;
//        } 
//    }
//    Serial.print("[match_string_start] seaching for ");
//    Serial.print(_char_search);
//    Serial.print(" in: ");
//    Serial.print(_char_source);
//    Serial.print(" ends at: ");
//    Serial.println(location);
//    return location;
//}

char element_div[] = {'/',' ','\0'};

int next_element(char* _char_source, int start) {
  start = start + 1;
  int end_index = -1;
  for (int i = 0; i < 3; i ++ ) { 
    int temp_index = index_of(element_div[i], _char_source, start);
    if (temp_index != -1 && end_index != -1) {
        if (temp_index < end_index) { end_index = temp_index; }
    } else if (temp_index != -1 && end_index == -1) {
        end_index = temp_index;
    }
  }
//  Serial.print("[next_element] returning next element start: ");
//  Serial.println(end_index);
  return end_index;
}

int return_number (char* _char_source, int start) {
    start = start + 1;
    int master_length = strlen(_char_source);
    if ((master_length-start) < 2) return -1;
    
    int end_index = next_element(_char_source, start);
    if (end_index == -1) { end_index = master_length - 1;
    } else if (end_index == 0) {
        start += 1;
        return return_number(_char_source, start);      
    } else { if (_char_source[end_index] == '/' || _char_source[end_index] == ' ') end_index -= 1; }
   
    int new_number = convert_string2int(_char_source, start, end_index);

//    Serial.print("[return_number] seaching for number in: ");
//    Serial.print(_char_source);
//    Serial.print(" start: ");
//    Serial.print(start);
//    Serial.print(" end: ");
//    Serial.print(end_index);
//    Serial.print(" number equals: ");
//    Serial.println(new_number);
    
    return new_number;
}

int delete_end (char* _char_source, int start) {
    int master_length = strlen(_char_source);
    for (int i = start; i < master_length; i++) {
        _char_source[i] = '\0';  
    }
}

int delete_start (char* _char_source, int start) {
    int master_length = strlen(_char_source);
    for (int i = 0; i < master_length; i++) {
        int cur_index = start + i;
        if (cur_index < master_length) { _char_source[i] = _char_source[cur_index]; }
        else { _char_source[i] = '\0'; }
    }
}

int convert_string2int(char* _char_source, int start_index, int end_index) {
  int return_num = 0;  
  int reverse_counter = (end_index - start_index);

  for(int i = 0; i <= (end_index - start_index); i++) {
      char cur_char = _char_source[i+start_index];      
      if (int(cur_char) < 48 || int(cur_char) > 57) { return -1; }
      int mult = 1;
      for(int j = 0; j < reverse_counter; j++) { mult = mult * 10; }
      return_num += (int(cur_char)-48) * mult; 
      reverse_counter--;
  }

//  Serial.print("[convert_string2int] start index: ");
//  Serial.print(start_index);
//  Serial.print(" end ind: ");
//  Serial.print(end_index);
//  Serial.print(" int _char_source: ");
//  Serial.println(return_num);
  return return_num;
}
