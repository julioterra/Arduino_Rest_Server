#include "config_rest.h"

void get_service_GET(int index, char* test_string) {	
	if (index < GET_SERVICES) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_sense_names_arrays[index][i]); }
	}
	// Serial.print("[get_service_GET] get service: "); Serial.println(test_string);		
}

void get_service_UPDATE(int index, char* test_string) {	
	if (index < UPDATE_SERVICES) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_act_names_arrays[index][i]); }
	}
	// Serial.print("[get_service_UPDATE] update service: "); Serial.println(test_string);		
}

void get_end_sequence(char* test_string) {	
	for (int i = 0; i < ELEMENT_DIV_COUNT; i++) { test_string[i] = char(req_end_pattern[i]); }
	// Serial.print("[get_end_sequence] end sequence: "); Serial.println(test_string);		
}

void get_element_div(char* test_string) {	
	for (int i = 0; i < END_SEQ_LENGTH; i++) { test_string[i] = char(element_div[i]); }
	// Serial.print("[get_element_div] element div: "); Serial.println(test_string);		
}