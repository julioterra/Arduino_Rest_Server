/* next_element(char*, int)
 *	accepts a string (char array) and a start index. looks for the
 *	next element by searching for divs in the element_div array.
 *	This array can be user defined based on how they want to structure
 *	and separate their messages. Standard separators include ' ' and '/ '  
 *  returns the location of the next div element if one is found
 *	otherwise, it returns -1.
 */
int next_element(char* _msg_source, int start) {
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

		if (temp_index != -1 && end_index != -1) {
		    if (temp_index < end_index) { 
				end_index = temp_index; 
			}
		} else if (temp_index != -1 && end_index == -1) {
			end_index = temp_index;
		} 		 
	}
	return end_index;
}

/* check_for_state_msg(char*, int)
 *	accepts a string (char array) and a start index. looks for state
 *	information sent as a number. If number is found it is returned.  
 *  Otherwise a -1 is returned.
 */
int check_for_state_msg (char* _msg_source, int start) {

	int master_length = strlen(_msg_source);
	start = check_start(_msg_source, start);
	if (start >= master_length) return -1;
    
	int end_index = next_element(_msg_source, start);
    if (end_index == -1) { 
		end_index = master_length - 1;
    } else if (_msg_source[end_index] == '/' || _msg_source[end_index] == ' ') {
		end_index -= 1; 
	}
   
	int new_num = convert_string2int(_msg_source, start, end_index);
    return new_num;
}

/* check_start(char*, int)
 *	accepts a string (char array) and a start index. looks at the first
 *	element of the message to see if it is a separator, ' ' and '/ ',
 *	and if so, it moves the start index one space over.  
 *  returns the location of the element not including the starting
 *	separator element.
 */
int check_start(char* _msg_source, int _start) {
	if (check_start_single(_msg_source, _start) == _start + 1) {
		return check_start_single(_msg_source, _start + 1);
	}
	return _start;

	// boolean moved = false;
	// for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) {
	// 	if (_msg_source[start] == element_div[i]) {
	// 		start += 1;
	// 		moved = true;
	// 		break;
	// 	}
	// }
	// if (moved) start = check_start(_msg_source, start);
	// return start;
}

int check_start_single(char* _msg_source, int _start) {
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) {
		if (_msg_source[_start] == element_div[i]) {
			return (_start + 1);
		}
	}
	return _start;	
}
