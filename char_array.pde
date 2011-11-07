
void clear_request(){
    for (int i = 0; i < REQUEST_LENGTH; i++) { request_msg[i] = '\0'; } 
    request_msg_index = 0;   
}

// returns index of next occurence of a char
int index_of(char _char_search, char* _msg_source,  int start) {
    int length = strlen(_msg_source);
    if (length <= start) return -1;
    for (int i = start; i < length; i++) {       
      if (_char_search == _msg_source[i]) return i;
    }
	return -1;
}

// matches two strings starting at start location on master. 
// returns -1 if no match is found, and the index number of 
// where the match ends.
int match_string_end(char* _char_search, char* _msg_source, int start) {
    int length_master = strlen(_msg_source);
    int length_search = strlen(_char_search);
    int location = -1;

    if (length_master < length_search + start) return -1;
    for (int i = 0; i < length_search; i++) {       
      if (_char_search[i] == _msg_source[i+start]) {
           location = i + start;
        } else {
            return -1;
        }
    }
    return location;  
}

//int match_string_start(char* _char_search, char* _msg_source, int start) {
//    int length_master = strlen(_msg_source);
//    int length_search = strlen(_char_search);
//    int location = -1;
//    boolean match = false;
//
//    for (int i = start; i < length_master; i++) {
//        if (length_master < length_search + i) return -1;
//        if (match_string_end(_char_search, _msg_source, i) != -1) {
//            location = i;
//            break;
//        } 
//    }
//    Serial.print("[match_string_start] seaching for ");
//    Serial.print(_char_search);
//    Serial.print(" in: ");
//    Serial.print(_msg_source);
//    Serial.print(" ends at: ");
//    Serial.println(location);
//    return location;
//}

// finds the end of the next element, which is identified by
// a "/", a " ", or and end of line.
int next_element(char* _msg_source, int start) {
  // Serial.print("[next_element] START seaching for next_element in: ");
  // Serial.print(_msg_source);
  // Serial.print(" start: ");
  // Serial.println(start);

	int end_index = -1;

	start = check_start(_msg_source, start);
	if (start >= strlen(_msg_source)) return -1;

	// loop through each element of element_div array to
	// check where is the nearest element div, such as '/' or ' '
	 for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) { 
		// check whether the current element_div exists in the request
		// if an element_div was found, and one was already found
		// then set the end_index to the nearest div
		// otherwise if an element_div was found and no other one
		// was already found then set end_index to current div location.
		int temp_index = index_of(element_div[i], _msg_source, start);

///////////
// check whether it is a space or '/' variable
		if (temp_index == start) return next_element(_msg_source, (start + 1));

		if (temp_index != -1 && end_index != -1) {
		    if (temp_index < end_index) { 
				end_index = temp_index; 
			}
		} else if (temp_index != -1 && end_index == -1) {
			end_index = temp_index;
		} else if (temp_index == -1 && end_index == -1) {
			// if ((strlen(_msg_source) - end_index) >= 2){
			// 
			// }
		}
	 }
  Serial.print("[next_element] END seaching starting : ");
  Serial.print(start);
  Serial.print(" found at: ");
  Serial.println(end_index);
  return end_index;
}

int check_start(char* _msg_source, int start) {
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) {
		if (_msg_source[start] == element_div[i]) {
			start += 1;
			break;
		}
	}
	return start;
}

/////////////////////////////
// change name to read_state
int return_number (char* _msg_source, int start) {

	Serial.print("[return_number] starting at location : ");
	Serial.print(start);
	start = check_start(_msg_source, start);
	Serial.print(" updated to: ");
	Serial.println(start);

	int master_length = strlen(_msg_source);
	if (start >= master_length) return -1;
    
	Serial.print("[return_number] start of the element after : ");
	Serial.print(start);
	int end_index = next_element(_msg_source, start);
	Serial.print(" is: ");
	Serial.println(end_index);

    if (end_index == -1) { 
		end_index = master_length - 1;
    } else if (_msg_source[end_index] == '/' || _msg_source[end_index] == ' ') {
		end_index -= 1; 
	}
   
	int new_num = convert_string2int(_msg_source, start, end_index);
  Serial.print("[return_number] attempt to : ");
  Serial.print(_msg_source);
  Serial.print(" start: ");
  Serial.print(start);
  Serial.print(" end: ");
  Serial.print(end_index);
  Serial.print(" new number: ");
  Serial.println(new_num);

    return new_num;
}

// void delete_end (char* _msg_source, int start) {
//     int master_length = strlen(_msg_source);
//     for (int i = start; i < master_length; i++) {
//         _msg_source[i] = '\0';  
//     }
// }
// 
// void delete_start (char* _msg_source, int start) {
//     int master_length = strlen(_msg_source);
//     for (int i = 0; i < master_length; i++) {
//         int cur_index = start + i;
//         if (cur_index < master_length) { _msg_source[i] = _msg_source[cur_index]; }
//         else { _msg_source[i] = '\0'; }
//     }
// }

void slice (char* _msg_source, int _start_index, int _end_index) {
    int master_length = strlen(_msg_source);

	Serial.print("[slice] master length: "); Serial.print(master_length); 
	Serial.print(" start: "); Serial.print(_start_index); 
	Serial.print(" end: "); Serial.print(_end_index);

	if (_end_index > master_length) 
		{ _end_index = master_length; } 
	if (_start_index > _end_index || _start_index < 0 || _end_index < 0) 
		{ return; }

    for (int i = 0; i < master_length; i++) {
        int shift_start_index = _start_index + i;
        if (shift_start_index < _end_index) { 
			_msg_source[i] = _msg_source[shift_start_index]; 
		} else { 
			_msg_source[i] = '\0'; 
		}
    }

	Serial.print(" new message: "); Serial.println(_msg_source);
}

int convert_string2int(char* _msg_source, int start_index, int end_index) {
  int return_num = 0;  
  int reverse_counter = (end_index - start_index);

  for(int i = 0; i <= (end_index - start_index); i++) {
      char cur_char = _msg_source[i+start_index];      
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
//  Serial.print(" int _msg_source: ");
//  Serial.println(return_num);
  return return_num;
}
