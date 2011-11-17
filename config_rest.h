#ifndef __config_rest__
#define __config_rest__

#include "WProgram.h"
// #include <avr/pgmspace.h>
// #include <Ethernet.h>

#define REQUEST_MAX_LENGTH 		 75		// maximum length of incoming requests
#define NAME_LENGTH				 15		// maximum length of service names
#define GET_SERVICES_COUNT		 6		// number of "get" services
#define POST_SERVICES_COUNT		 4		// number of "set" services
#define CALLBACK		 	  	 1		// set to 1 if respond() callback will be used

// update this array with the names of the GET services your Arduino will support
const char services_get_names [GET_SERVICES_COUNT][NAME_LENGTH] = {	"analog_1", 
																  	"analog_2", 
																	"analog_3", 
																	"analog_4", 
																	"analog_5", 
																	"analog_6"};

// update this array with the names of the SET services your Arduino will support
const char services_set_names [POST_SERVICES_COUNT][NAME_LENGTH] = {"output_1", 
																	"output_2", 
																	"output_3", 
																	"output_4"};

#define SERVICES_COUNT		 	 10

struct Resource_description {
		char name [NAME_LENGTH]; 	/* last name initial */
		boolean post_enabled;		/* childs age */
	};

const struct Resource_description resource_list [SERVICES_COUNT] = {	{"output_1", false}, 
																		{"output_2", false}, 
																		{"output_3", false}, 
																		{"output_4", false}, 
																		{"output_5", false}, 
																		{"output_6", false}, 
																		{"input_1", true}, 
																		{"input_2", true}, 
																		{"input_3", true}, 
																		{"input_4", true}, 
																	};

// update this array with the names of the GET services your Arduino will support
// const char service_names [SERVICES_COUNT][NAME_LENGTH] = {	"analog_1", 
// 																"analog_2", 
// 																"analog_3", 
// 																"analog_4", 
// 																"analog_5", 
// 																"analog_6"};
// 
// const boolean post_enabled [SERVICES_COUNT] = {false, false, false, false, false};
// 

/**************************************************************************
 DO NOT MAKE CHANGES BELOW THIS MARKER... 
	...unless you know what you are doing
	
 The constants below this line are not intended to be update, unless you 
 want to change the functionality of the RestServer library.
 **************************************************************************/

void get_service(int, char*);			// method that sets a char array with a get service

#define CRLF "\r\n"

#define SERVICE_TYPES		 	 2		// number of service types
#define GET_SERVICES			 0		// set GET_SERVICES equals 0
#define POST_SERVICES			 1		// set POST_SERVICES equals 1

#define NO_MATCH				-1		// set NO_MATCH to equals -1

#define END_SEQ_LENGTH			 2		// set END_SEQ_LENGTH equals 0
#define ELEMENT_DIV_LENGTH  	 2		// set ELEMENT_DIV_LENGTH equals 0

	
#endif // #endif __config_rest__
