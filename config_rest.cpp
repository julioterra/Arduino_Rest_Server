#include "config_rest.h"

void get_service(int index, char* test_string) {
	if (index < SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(resource_list[index].name[i]); }
	}
}
