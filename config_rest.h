#ifndef __config_rest__
#define __config_rest__

#include "WProgram.h"

#define REQUEST_MAX_LENGTH 		75		// maximum length of incoming requests
#define NAME_LENGTH				15		// maximum length of service names
#define TIMEOUT_INTERVAL 		2000

/**************************************************************************
 DO NOT MAKE CHANGES BELOW THIS MARKER... 
 ...unless you know what you are doing
	
 The data structures and constants below are not intended to be update, unless you 
 are planning to change the functionality of the RestServer library.
 **************************************************************************/

// data structure that holds range information for each resource description
typedef struct resource_range_t {
	int min;
	int max;
};

// data structure that holds resource descriptions defined by users
typedef struct resource_description_t {
	char name [NAME_LENGTH]; 	// name of resource
	boolean post_enabled;		// flag that notes whether resource supports post requests
	resource_range_t range;		// resource range
};

// data structure for data related to active resources, such as current
// state of each resource and current get or post requests received 
typedef struct resource_active_t {
	int state;					// holds current state of resource
	boolean get;				// get request flag for this resource 
	boolean post;				// post request flag for this resource
};

// constants that identify the number and type of requests that are
// supported by the RestServer library
#define GET_REQUESTS			 0		// set GET_REQUESTS equals 0
#define POST_REQUESTS			 1		// set POST_REQUESTS equals 1

// constant used to identify non-matches in several methods from RestServer and Message classes
#define NO_MATCH				-1		

// constants related to end of line and end of header sequences, and div characters 
#define DIV_ELEMENTS			 4		// number of DIV_ELEMENTS available
#define EOL_LENGTH  	 		 2		// number of characters in end of line sequence (EOL)
#define EOH_LENGTH  	 		 4		// number of characters in end of header sequence (EOH)
	
#endif // #endif __config_rest__
