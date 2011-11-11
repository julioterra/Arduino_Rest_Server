/* next_element(char*, int)
 *	accepts a string (char array) and a start index. looks for the
 *	next element by searching for divs in the element_div array.
 *	This array can be user defined based on how they want to structure
 *	and separate their messages. Standard separators include ' ' and '/ '  
 *  returns the location of the next div element if one is found
 *	otherwise, it returns NO_MATCH.
 */
int next_element(int _start) {
	_start = check_start(_start);
	if (_start >= request.length) return NO_MATCH;

	// loop through each element of element_div array to check for element end
	int match_index = NO_MATCH;
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) { 
		int new_index = request.find(element_div[i], _start);
		// if no previous match, or current match index is smaller, then update match_index
		if (new_index != NO_MATCH) {
			if (match_index == NO_MATCH || (new_index < match_index)) {
				match_index = new_index;	
			} 
		} 		 
	}
	return match_index;
}

/* check_for_state_msg(char*, int)
 *	accepts a string (char array) and a start index. looks for state
 *	information sent as a number. If number is found it is returned.  
 *  Otherwise a NO_MATCH is returned.
 */
int check_for_state_msg(int _start) {
	_start = check_start(_start);
	if (_start >= request.length) return NO_MATCH;
    
	int match_index = next_element(_start);
    if (match_index == NO_MATCH) { 
		match_index = request.length - 1;
    } else if (request.msg[match_index] == '/' || request.msg[match_index] == ' ') {
		match_index -= 1; 
	}
   
	int new_num = request.to_i(_start, match_index);
    return new_num;
}

/* check_start(char*, int)
 *	accepts a string (char array) and a start index. looks at the first
 *	element of the message to see if it is a separator, ' ' and '/ ',
 *	and if so, it moves the start index one space over.  
 *  returns the location of the element not including the starting
 *	separator element.
 */
int check_start(int _start) {
	if (check_start_single(_start) == _start + 1) {
		return check_start_single(_start + 1);
	}
	return _start;
}

int check_start_single(int _start) {
	for (int i = 0; i < ELEMENT_DIV_COUNT; i ++ ) {
		if (request.msg[_start] == element_div[i]) {
			return (_start + 1);
		}
	}
	return _start;	
}
