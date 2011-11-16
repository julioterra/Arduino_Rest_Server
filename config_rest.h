#ifndef config_rest
#define config_rest

#include "WProgram.h"

#define REQUEST_MAX_LENGTH 		 75		// maximum length of incoming requests
#define NAME_LENGTH				 15		// maximum length of service names
#define GET_SERVICES_COUNT		 6		// number of "get" services
#define SET_SERVICES_COUNT		 4		// number of "set" services
#define CALLBACK		 	  	 1		// set to 1 if respond() callback will be used

const char services_get_names [GET_SERVICES_COUNT][NAME_LENGTH] = {"analog_1", "analog_2", "analog_3", "analog_4", "analog_5", "analog_6"};
const char services_set_names [SET_SERVICES_COUNT][NAME_LENGTH] = {"output_1", "output_2", "output_3", "output_4"};

void get_service_GET(int, char*);
void get_service_SET(int, char*);

#define GET_SERVICES			 0
#define SET_SERVICES			 1

#define NO_MATCH				-1

#define END_SEQ_LENGTH			 2
#define ELEMENT_DIV_LENGTH  	 2

#define SERVICE_TYPES		 	 2

	
#endif // #endif _config_rest