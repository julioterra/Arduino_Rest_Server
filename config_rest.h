#ifndef __config_rest__
#define __config_rest__

#include "WProgram.h"
#include <avr/pgmspace.h>

#define REQUEST_MAX_LENGTH 		 75		// maximum length of incoming requests
#define NAME_LENGTH				 15		// maximum length of service names
#define CALLBACK		 	  	 1		// set to 1 if respond() callback will be used
#define SERVICES_COUNT		 	 10

typedef struct Resource_range {
	int min;
	int max;
};

struct Resource_description {
	char name [NAME_LENGTH]; 	/* name of resource */
	boolean post_enabled;		/* flag that notes whether it supports post requests */
	Resource_range range;		/* resource range for service requests and mapping */
};

const struct Resource_description resources_spec [SERVICES_COUNT] = {{"output_1", 	false, 	{0, 1024}}, 
																	{"output_2", 	false, 	{0, 1024}}, 
																	{"output_3", 	false, 	{0, 1024}}, 
																	{"output_4", 	false, 	{0, 1024}}, 
																	{"output_5", 	false, 	{0, 1024}}, 
																	{"output_6", 	false, 	{0, 1024}}, 
																	{"input_1", 	true, 	{0, 255}}, 
																	{"input_2", 	true, 	{0, 255}}, 
																	{"input_3", 	true, 	{0, 255}}, 
																	{"input_4", 	true, 	{0, 255}} 
																	};

/**************************************************************************
 DO NOT MAKE CHANGES BELOW THIS MARKER... 
	...unless you know what you are doing
	
 The constants below this line are not intended to be update, unless you 
 want to change the functionality of the RestServer library.
 **************************************************************************/

void get_service(int, char*);			// method that sets a char array with a get service
char *get_service(int);					// method that sets a char array with a get service
int str_to_i(char*, int, int);

// declare a carriage return and line feed constant
#define CRLF "\r\n"

#define SERVICE_TYPES		 	 2		// number of service types
#define GET_SERVICES			 0		// set GET_SERVICES equals 0
#define POST_SERVICES			 1		// set POST_SERVICES equals 1

#define NO_MATCH				-1		// set NO_MATCH to equals -1

#define DIV_ELEMENTS			 3		// set DIV_ELEMENTS equals 0
#define EOL_LENGTH  	 		 2		// set EOL_LENGTH equals 0
#define EOH_LENGTH  	 		 4		// set EOL_LENGTH equals 0

// static string declaration method
// #define P(name)   static const prog_uchar name[] PROGMEM
// char* getPstring(const prog_uchar *str, boolean next_segment);

	
#endif // #endif __config_rest__
