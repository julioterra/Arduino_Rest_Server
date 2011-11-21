/*
 Copyright (c) 2011, Julio Terra

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

// Message: simple string-related Library for Arduino.
// Developed in conjunction to RestServer library

#ifndef __RestMessage_h__
#define __RestMessage_h_

#include "WProgram.h"
#include "config_rest.h"
#include <string.h>

class Message {

public:
	char msg [REQUEST_MAX_LENGTH];

	int length;
	int max_length;

	Message();
	void add(char);
	void clear();
	void slice(int, int);
	int find(char, int);
	int match_string(char*, int);
	int to_i(int, int);

};

#endif