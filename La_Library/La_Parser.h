#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "La_Log.h"
#define PARSER_DEBUG_OFF // enables/disables conditional compilation 

#define PARSER_STRIP_EMPTY_LINES        1   // strips all blank lines
#define PARSER_LEAVE_EMPTY_LINES        2   // leaves empty lines
#define PARSER_STRIP_WS_ENDS            4   // strips ws space at ends of line
#define PARSER_LEAVE_WS_ENDS            8   // leaves it
#define PARSER_STRIP_COMMENTS           16  // strips comments out
#define PARSER_LEAVE_COMMENTS           32  // leaves comments in

#define PARSER_BUFFER_SIZE              256 // size of parser line buffer
#define PARSER_MAX_COMMENT              16  // maximum size of comment delimeter string

#define PARSER_DEFAULT_COMMENT          "#"  // default comment string for parser



// pattern language
#define PATTERN_TOKEN_FLOAT   'f'
#define PATTERN_TOKEN_INT     'i'
#define PATTERN_TOKEN_STRING  's'
#define PATTERN_TOKEN_LITERAL '\''

// state machine defines for pattern matching
#define PATTERN_STATE_INIT       0

#define PATTERN_STATE_RESTART    1
#define PATTERN_STATE_FLOAT      2
#define PATTERN_STATE_INT        3 
#define PATTERN_STATE_LITERAL    4
#define PATTERN_STATE_STRING     5
#define PATTERN_STATE_NEXT       6

#define PATTERN_STATE_MATCH      7
#define PATTERN_STATE_END        8

#define PATTERN_MAX_ARGS         16
#define PATTERN_BUFFER_SIZE      80


// parser class ///////////////////////////////////////////////
class CPARSERV1
{
public:

	CPARSERV1() :fstream(nullptr) { reset(); }
	~CPARSERV1() { close(); }

	// reset file system ////////////////////////////////////////////
	void reset();

	// open file /////////////////////////////////////////////////////
	bool open(const char* filename)
	{
		reset();

		// opens a file
		if ((fstream = fopen(filename, "r")) == nullptr)
		{
			ERROR_MSG("\nCouldn't open file: %s", filename);
			return(false);
		}
		return(true);
	}

	// close file ////////////////////////////////////////////////////
	void close(){ reset(); }

	// get line //////////////////////////////////////////////////////
	char* getline(int mode);

	// sets the comment string ///////////////////////////////////////
	bool setComment(const char* string)
	{
		// sets the comment string
		if (strlen(string) < PARSER_MAX_COMMENT)
		{
			strcpy(comment, string);
			return(true);
		}
		else return(false);
	}

	// find pattern in line //////////////////////////////////////////
	int match(const char* string, const char* pattern, ...);

	// VARIABLE DECLARATIONS /////////////////////////////////////////

public:

	FILE* fstream;                    // file pointer
	char buffer[PARSER_BUFFER_SIZE];  // line buffer
	int  length;                      // length of current line
	int  num_lines;                   // number of lines processed
	char comment[PARSER_MAX_COMMENT]; // single line comment string

// pattern matching parameter storage, easier that variable arguments
// anything matched will be stored here on exit from the call to pattern()
	char  pstrings[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE]; // any strings
	int   num_pstrings;

	float pfloats[PATTERN_MAX_ARGS];                       // any floats
	int   num_pfloats;

	int   pints[PATTERN_MAX_ARGS];                         // any ints
	int   num_pints;

};


int ReplaceChars(char* string_in, char* string_out, const char* replace_chars, char rep_char, bool case_on = true);
int StripChars(const char* string_in, char* string_out, const char* strip_chars, bool case_on = true);
int IsInt(char* istring);
float IsFloat(char* fstring);
char* StringRtrim(char* string);
char* StringLtrim(char* string);
char* ExtractFilenameFromPath(const char* filepath, char* filename);