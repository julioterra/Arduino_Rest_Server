#include "config_rest.h"

void get_service(int index, char* test_string) {
	if (index < SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(resources_spec[index].name[i]); }
	}
}

char *get_service(int index) {
	char static string[NAME_LENGTH];
	if (index < SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { string[i] = char(resources_spec[index].name[i]); }
	}
	return string;
}

// char *getPstring(const prog_uchar *str, boolean next_segment) {
// 	char static buffer[32];
// 	int static offset;
// 	int bufferEnd = 0;
//   
// 	if (next_segment) {
// 		if (offset % 32 != 0) offset = 0;
// 		for (int i = 0; i < 32; i++) buffer[i] = '\0';
// 
// 		while (bufferEnd <= 32 && pgm_read_byte(str += offset)) {
// 			buffer[bufferEnd++] = char(pgm_read_byte(str)); 
// 		}
// 
// 		offset += 32;
// 	}
// 
// 	return buffer;
// }