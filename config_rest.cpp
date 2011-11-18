#include "config_rest.h"

char *get_service(int index) {
	char static string[NAME_LENGTH];
	if (index < SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { string[i] = char(resources_spec[index].name[i]); }
	}
	return string;
}

// void get_service(int index, char* test_string) {
// 	if (index < SERVICES_COUNT) {
// 		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(resources_spec[index].name[i]); }
// 	}
// }

// int str_to_i(char* string, int _start_index, int end_index) {
//   int return_num = 0;  
//   int reverse_counter = (end_index - _start_index);
// 
//   for(int i = 0; i <= (end_index - _start_index); i++) {
//       char cur_char = string[i+_start_index];      
//       if (int(cur_char) < 48 || int(cur_char) > 57) { return -1; }
//       int mult = 1;
//       for(int j = 0; j < reverse_counter; j++) { mult = mult * 10; }
//       return_num += (int(cur_char)-48) * mult; 
//       reverse_counter--;
//   }
//   return return_num;
// }

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