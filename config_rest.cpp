#include "config_rest.h"

void get_service_GET(int index, char* test_string) {	
	if (index < GET_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_get_names[index][i]); }
	}
	// Serial.print("[get_service_GET] get service: "); Serial.println(test_string);		
}

void get_service_POST(int index, char* test_string) {	
	if (index < POST_SERVICES_COUNT) {
		for (int i = 0; i < NAME_LENGTH; i++) { test_string[i] = char(services_set_names[index][i]); }
	}
	// Serial.print("[get_service_SET] update service: "); Serial.println(test_string);		
}

// void printPclient(const prog_uchar *str, Client _client) {
//   // copy data out of program memory into local storage, write out in
//   // chunks of 32 bytes to avoid extra short TCP/IP packets
//   uint8_t buffer[32];
//   size_t bufferEnd = 0;
//   
//   while (buffer[bufferEnd++] = pgm_read_byte(str++)) {
//     if (bufferEnd == 32) {
//       _client.write(buffer, 32);
//       bufferEnd = 0;
//     }
//   }
// 
//   // write out everything left but trailing NUL
//   if (bufferEnd > 1)
//     _client.write(buffer, bufferEnd - 1);
// }
// 
// void printPserial(const prog_uchar *str) {
//   // copy data out of program memory into local storage, write out in
//   // chunks of 32 bytes to avoid extra short TCP/IP packets
//   uint8_t buffer[32];
//   size_t bufferEnd = 0;
//   
//   while (buffer[bufferEnd++] = pgm_read_byte(str++)) {
//     if (bufferEnd == 32) {
//       Serial.write(buffer, 32);
//       bufferEnd = 0;
//     }
//   }
// 
//   // write out everything left but trailing NUL
//   if (bufferEnd > 1)
//     Serial.write(buffer, bufferEnd - 1);
// }