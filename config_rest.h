#ifndef config_rest
#define config_rest

#include "WProgram.h"

#define REQUEST_MAX_LENGTH 		 75
#define END_SEQ_LENGTH			 2
#define ELEMENT_DIV_COUNT  		 2
#define NAME_LENGTH				 15

#define SERVICE_TYPES		 	 2
#define GET_SERVICES			 6
#define GET_CALLBACK		 	 false
#define UPDATE_SERVICES		 	 4
#define UPDATE_CALLBACK		 	 false

#define GET_INDEX				 0
#define UPDATE_INDEX			 1

#define NO_MATCH				-1


const char req_end_pattern[ELEMENT_DIV_COUNT+1] = {"\r\n"};
const char element_div[END_SEQ_LENGTH+1] = {'/',' '};
const char services_sense_names_arrays [GET_SERVICES][NAME_LENGTH] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};
const char services_act_names_arrays [UPDATE_SERVICES][NAME_LENGTH] = {"output_1", "output_2", "output_3", "output_4"};

void get_service_GET(int, char*);
void get_service_UPDATE(int, char*);
void get_end_sequence(char*);
void get_element_div(char*);
	
/* DO NOT CHANGE STUFF BELOW THIS LINE, unless you know what you are doing */

#endif // #endif _config_h

// char [NAME_LENGTH] get_service_GET(int);
// char [NAME_LENGTH] get_service_UPDATE(int);
// char [ELEMENT_DIV_COUNT] get_end_sequence();
// char [END_SEQ_COUNT] get_element_div();
