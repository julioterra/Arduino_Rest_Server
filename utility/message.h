#ifndef __RestMessage_h__
#define __RestMessage_h_

#include "WProgram.h"
#include "config_rest.h"
#include <stdlib.h>

class Message {

public:
	// char *msg;
	int length;
	int max_length;

	char msg [REQUEST_MAX_LENGTH];

	Message(int);
	Message();
	char *create_msg();
	void add(char);
	void clear();
	void slice(int, int);
	int find(char, int);
	int match_string(char*, int);
	int to_i(int, int);

};

#endif