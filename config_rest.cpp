#include "config_rest.h"

void get_service_GET(int index, char* test_string) {	
	if (index < GET_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_sense_names_arrays[index][i]); }
	}
	// Serial.print("[get_service_GET] get service: "); Serial.println(test_string);		
}

void get_service_UPDATE(int index, char* test_string) {	
	if (index < SET_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_act_names_arrays[index][i]); }
	}
	// Serial.print("[get_service_UPDATE] update service: "); Serial.println(test_string);		
}