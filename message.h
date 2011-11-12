#ifndef Message_h
#define Message_h

#include "WProgram.h"
#include "config_rest.h"

class Message {

public:
	char msg [REQUEST_MAX_LENGTH];
	int length;

	Message();
	void add(char);
	void clear();
	void slice(int, int);
	int find(char, int);
	int match_string(char*, int);
	int to_i(int, int);
};

#endif