#include "config_rest.h"

void get_service_GET(int index, char* test_string) {	
	if (index < GET_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_get_names[index][i]); }
	}
	// Serial.print("[get_service_GET] get service: "); Serial.println(test_string);		
}

void get_service_SET(int index, char* test_string) {	
	if (index < SET_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_set_names[index][i]); }
	}
	// Serial.print("[get_service_SET] update service: "); Serial.println(test_string);		
}