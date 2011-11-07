/* slice(char*, int, int)
 *	accepts a string (char array) and a start index and an end index. 
 *	it changes the existing array to only contain the information 
 *  within the start and end index. 
 */
void slice (char* _msg_source, int _start_index, int _end_index) {
    int master_length = strlen(_msg_source);

	if (_end_index > master_length) 
		{ _end_index = master_length; } 
	if (_start_index > _end_index || _start_index < 0 || _end_index < 0) 
		{ return; }

    for (int i = 0; i < master_length; i++) {
        int shift_start_index = _start_index + i;
        if (shift_start_index < _end_index) 
			{ _msg_source[i] = _msg_source[shift_start_index]; } 
		else 
			{ _msg_source[i] = '\0'; }
    }
}

/* clear_request()
 *	clears the full request_msg array and resets the request_msg_index to 0.
 */
void clear_request(){
    for (int i = 0; i < REQUEST_LENGTH; i++) { request_msg[i] = '\0'; } 
    request_msg_index = 0;   
}

/* index_of(char, char*, int)
 *	accepts a search character, a source string and a start index.
 *	returns the index of next occurence of a char after the start 
 *	index. If the char does not exist then it returns a -1.
 */
int index_of(char _char_search, char* _msg_source,  int start) {
    int length = strlen(_msg_source);
    if (length <= start) return -1;
    for (int i = start; i < length; i++) {       
      if (_char_search == _msg_source[i]) return i;
    }
	return -1;
}

/* match_string_end(char*, char*, int)
 *	accepts a search string, a source string and a start index. 
 *	searches the source string for a match with the search string
 *	starting at the start index location only. 
 *	if match is found it returns the index number of the last 
 *	matching element. Otherwise, it returns -1.
 */
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

/* convert_string2int(char*, int, int)
 *	accepts a msg string, a start and an end index. Then it attempts 
 *	to convert the character string into digits. If any of the 
 *	characters are not numbers then the method returns a -1.
 *	otherwise the method returns the number as an int.
 */
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
  return return_num;
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