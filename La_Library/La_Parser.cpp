#include "La_Parser.h"
#include <string.h>
#include <ctype.h>
#include <float.h>

int ReplaceChars(char* string_in, char* string_out, const char* replace_chars, char rep_char, bool case_on)
{
	// this function simply replaces the characters from the input string that
	// are listed in replace with the replace char, the results are stored in 
	// string_out, string_in and isn't touched, the number of replacments is 
	// returned. if case_on = 1 then case is checked, other it's case insensitive

	int num_replacements = 0,  // tracks number of characters replaced
		index_in = 0,      // curr index into input
		index_out = 0,      // curr index into output
		sindex,                // loop var into strip array
		slength = strlen(replace_chars); // length of strip string

	// do some error checking
	if (!string_in || !string_out || strlen(string_in) == 0)
		return(0);

	// nothing to replace
	if (!replace_chars || strlen(replace_chars) == 0)
	{
		strcpy(string_out, string_in);
		return(0);
	}

	// determine if case is important
	if (case_on == 1)
	{
		// perform char by char copy
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
				if (string_in[index_in] == replace_chars[sindex])
				{
					// replace it
					string_out[index_out++] = rep_char;
					index_in++;
					num_replacements++;
					break;
				} // end if

		 // was a replacement performed?, no just copy then
			if (sindex >= slength)
				string_out[index_out++] = string_in[index_in++];
		}
	}
	else
	{
		// perform char by char copy with case insensitivity
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
				if (toupper(string_in[index_in]) == toupper(replace_chars[sindex]))
				{
					// replace it
					string_out[index_out++] = rep_char;
					index_in++;
					num_replacements++;
					break;
				} // end if

		 // was a strip char found?
			if (sindex >= slength)
				string_out[index_out++] = string_in[index_in++];

		} // end while
	} // end if case_off

	// terminate output string
	string_out[index_out] = 0;

	// return extracts
	return(num_replacements);
}

/// <summary>
/// 
/// </summary>
/// <param name="string_in"></param>
/// <param name="string_out"></param>
/// <param name="strip_chars"></param>
/// <param name="case_on">
/// ??????????????
/// </param>
/// <returns>
/// ??????????????
/// </returns>
int StripChars(const char* string_in, char* string_out, const char* strip_chars, bool case_on)
{
	// this function simply strips/extracts the characters from the input string that
	// are listed in strip, the results are stored in string_out, string_in
	// isn't touched, the number of extractions or returned
	// if case_on = 1 then case is checked, other it's case insensitive

	int num_extracts = 0,  // tracks number of characters extracted
		index_in = 0,  // curr index into input
		index_out = 0,  // curr index into output
		sindex,            // loop var into strip array
		slength = strlen(strip_chars); // length of strip string

	// do some error checking
	if (!string_in || !string_out || strlen(string_in) == 0)
		return(0);

	// nothing to replace
	if (!strip_chars || strlen(strip_chars) == 0)
	{
		strcpy(string_out, string_in);
		return(0);
	} 

 // determine if case is importants
	if (case_on)
	{
		// perform char by char copy
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
				if (string_in[index_in] == strip_chars[sindex])
				{
					// jump over input char, it's stripped
					index_in++;
					num_extracts++;
					break;
				} 

		 // was a strip char found?
			if (sindex >= slength)
				string_out[index_out++] = string_in[index_in++];

		} 
	} 
	else
	{
		// perform char by char copy with case insensitivity
		while (string_in[index_in])
		{
			for (sindex = 0; sindex < slength; sindex++)
				if (toupper(string_in[index_in]) == toupper(strip_chars[sindex]))
				{
					// jump over input char, it's stripped
					index_in++;
					num_extracts++;
					break;
				} 

		 // was a strip char found?
			if (sindex >= slength)
				string_out[index_out++] = string_in[index_in++];

		} 
	} 

	// terminate output string
	string_out[index_out] = 0;

	// return extracts
	return(num_extracts);
} 

int IsInt(char* istring)
{
	// validates the sent string as a int and converts it, if it's not valid
	// the function sends back INT_MIN, the chances of this being the number
	// validated is slim
	// [whitespace] [sign]digits

	char* string = istring;

	// must be of the form
	// [whitespace] 
	while (isspace(*string)) string++;

	// [sign] 
	if (*string == '+' || *string == '-') string++;

	// [digits] 
	while (isdigit(*string)) string++;

	// the string better be the same size as the other one
	if (strlen(istring) == (int)(string - istring))
		return(atoi(istring));
	else
		return(INT_MIN);
} 

float IsFloat(char* fstring)
{
	// validates the sent string as a float and converts it, if it's not valid
	// the function sends back FLT_MIN, the chances of this being the number
	// validated is slim
	// [whitespace] [sign] [digits] [.digits] [ {d | D | e | E }[sign]digits]

	char* string = fstring;

	// must be of the form
	// [whitespace] 
	while (isspace(*string)) string++;

	// [sign] 
	if (*string == '+' || *string == '-') string++;

	// [digits] 
	while (isdigit(*string)) string++;

	// [.digits] 
	if (*string == '.')
	{
		string++;
		while (isdigit(*string)) string++;
	}

	// [ {d | D | e | E }[sign]digits]
	if (*string == 'e' || *string == 'E' || *string == 'd' || *string == 'D')
	{
		string++;

		// [sign] 
		if (*string == '+' || *string == '-') string++;

		// [digits] 
		while (isdigit(*string)) string++;
	}

	// the string better be the same size as the other one
	if (strlen(fstring) == (int)(string - fstring))
		return(atof(fstring));
	else
		return(FLT_MIN);
}

/// <summary>
/// ??????????????
/// </summary>
/// <param name="string"></param>
/// <returns></returns>
char* StringRtrim(char* string)
{
	// trims whitespace from right side, note is destructive
	int sindex = 0;

	int slength = strlen(string);

	if (!string || slength == 0) return(string);

	// index to end of string
	sindex = slength - 1;

	// trim whitespace by overwriting nulls
	while (isspace(string[sindex]) && sindex >= 0)
		string[sindex--] = 0;

	// string doens't need to be moved, so simply return pointer
	return(string);
} 

/// <summary>
/// ????????????
/// </summary>
/// <param name="string"></param>
/// <returns></returns>
char* StringLtrim(char* string)
{
	// trims whitespace from left side, note is destructive
	int sindex = 0;

	int slength = strlen(string);

	if (!string || slength == 0) return(string);

	// trim whitespace by advancing pointer
	while (isspace(string[sindex]) && sindex < slength)
		string[sindex++] = 0; // not needed actually

   // copy string to left
	memmove((void*)string, (void*)&string[sindex], (slength - sindex) + 1);

	// now return pointer
	return(string);

}

// reset file system ////////////////////////////////////////////
void CPARSERV1::reset()
{
	if (fstream)
		fclose(fstream);

	fstream = NULL;

	// clear and reset buffer
	memset(buffer, 0, sizeof(buffer));
	length = 0;
	num_lines = 0;

	// set comment
	strcpy(comment, PARSER_DEFAULT_COMMENT);
}


// get line //////////////////////////////////////////////////////

char* CPARSERV1::getline(int mode)
{
	char* string;

	// gets a single line from the stream
	if (fstream)
	{
		// check translation mode
		if (mode & PARSER_STRIP_EMPTY_LINES)
		{
			// get lines until we get a real one with data on it
			while (1)
			{
				// have we went to the end of the file without getting anything?
				if ((string = fgets(buffer, PARSER_BUFFER_SIZE, fstream)) == NULL)
					break;

				// we have something, strip ws from it
				int slength = strlen(string);
				int sindex = 0;

				// eat up space
				while (isspace(string[sindex]))
					sindex++;

				// is there anything left?
				if ((slength - sindex) > 0)
				{
					// copy the string into place
					memmove((void*)buffer, (void*)&string[sindex], (slength - sindex) + 1);
					string = buffer;
					slength = strlen(string);

					// strip comments also?
					if (mode & PARSER_STRIP_COMMENTS)
					{
						// does this begin with a comment or end with a comment?
						char* comment_string = strstr(string, comment);

						// 3 cases, no comment, comment at beginning, comment at end
						if (comment_string == NULL)
							break; // line is valid exit with line

						 // compute index into string from beginning where comment begins
						int cindex = (int)(comment_string - string);

						// comment at beginning then continue

						if (cindex == 0)
							continue; // this line is a comment, ignore completely, get another
						else
						{
							// comment at end, strip it, insert null where it begins
							comment_string[0] = 0;
							break;
						} 
					} 

				// exit loop, we have something :)
					break;
				} 
			} 
		} 
		else
		{
			// just get the next line, don't worry about stripping anything
			string = fgets(buffer, PARSER_BUFFER_SIZE, fstream);
		} 

	// was the line valid?
		if (string)
		{
			// increment line count
			num_lines++;

			// final stripping of whitspace
			if (mode & PARSER_STRIP_WS_ENDS)
			{
				StringLtrim(buffer);
				StringRtrim(buffer);
			}
		 // compute line length
			length = strlen(buffer);

			ERROR_MSG("\nString[%d]:%s", length, string);
			// return the pointer, copy of data already in buffer
			return(string);

		}
		else
		{
			ERROR_MSG("\nEOF");
			return(nullptr);
		}

	}
	else
	{
		ERROR_MSG("\nFstream NULL.");
		return(nullptr);
	}
}


// find pattern in line //////////////////////////////////////////

int CPARSERV1::match(const char* string, const char* pattern, ...)
{
	// this function tries to match the pattern sent in pattern with
	// the sent string, the results are sent back to the sender in the
	// variable arguments as well as stored in the parameter passing area

	// string literal                        = ['string']
	// floating point number                 = [f]
	// integer number                        = [i]
	// match a string exactly ddd chars      = [s=ddd] 
	// match a string less than ddd chars    = [s<ddd] 
	// match a string greater than ddd chars = [s>ddd]
	// for example to match "vertex: 34.234 56.34 12.4
	// ['vertex'] [f] [f] [f]

	char token_type[PATTERN_MAX_ARGS];         // type of token, f,i,s,l
	char token_string[PATTERN_MAX_ARGS][PATTERN_BUFFER_SIZE];   // for literal strings this holds them
	char token_operator[PATTERN_MAX_ARGS];     // holds and operators for the token, >, <, =, etc.
	int  token_numeric[PATTERN_MAX_ARGS];      // holds any numeric data to qualify the token

	char buffer[PARSER_BUFFER_SIZE]; // working buffer

	// a little error testing
	if ((!string || strlen(string) == 0) || (!pattern || strlen(pattern) == 0))
		return(0);

	// copy line into working area
	strcpy(buffer, string);

	int tok_start = 0,
		tok_end = 0,
		tok_restart = 0,
		tok_first_pass = 0,
		num_tokens = 0;

	// step 1: extract token list
	while (1)
	{
		// eat whitepace
		while (isspace(pattern[tok_start]))
			tok_start++;

		// end of line?
		if (tok_start >= strlen(pattern))
			break;

		// look for beginning of token '['
		if (pattern[tok_start] == '[')
		{
			// now look for token code
			switch (pattern[tok_start + 1])
			{
			case PATTERN_TOKEN_FLOAT:  // float
			{
				// make sure token is well formed
				if (pattern[tok_start + 2] != ']')
					return(0); // error

				 // advance token scanner
				tok_start += 3;

				// insert a float into pattern
				token_type[num_tokens] = PATTERN_TOKEN_FLOAT;  // type of token, f,i,s,l
				strcpy(token_string[num_tokens], "");           // for literal strings this holds them
				token_operator[num_tokens] = 0;                // holds and operators for the token, >, <, =, etc.
				token_numeric[num_tokens] = 0;

				// increment number of tokens
				num_tokens++;

#ifdef PARSER_DEBUG_ON
				printf("\nFound Float token");
#endif
			} break;

			case PATTERN_TOKEN_INT:  // integer
			{
				// make sure token is well formed
				if (pattern[tok_start + 2] != ']')
					return(0); // error

				 // advance token scanner
				tok_start += 3;

				// insert a int into pattern
				token_type[num_tokens] = PATTERN_TOKEN_INT;  // type of token, f,i,s,l
				strcpy(token_string[num_tokens], "");         // for literal strings this holds them
				token_operator[num_tokens] = 0;              // holds and operators for the token, >, <, =, etc.
				token_numeric[num_tokens] = 0;

				// increment number of tokens
				num_tokens++;

#ifdef PARSER_DEBUG_ON
				printf("\nFound Int token");
#endif

			} break;

			case PATTERN_TOKEN_LITERAL: // literal string
			{
				// advance token scanner to begining literal string
				tok_start += 2;
				tok_end = tok_start;

				// eat up string
				while (pattern[tok_end] != PATTERN_TOKEN_LITERAL)
					tok_end++;

				// make sure string is well formed
				if (pattern[tok_end + 1] != ']')
					return(0);

				// insert a string into pattern              

				// literal string lies from (tok_start - (tok_end-1)
				memcpy(token_string[num_tokens], &pattern[tok_start], (tok_end - tok_start));
				token_string[num_tokens][(tok_end - tok_start)] = 0; // null terminate

				token_type[num_tokens] = PATTERN_TOKEN_LITERAL;  // type of token, f,i,s,'
				token_operator[num_tokens] = 0;                 // holds and operators for the token, >, <, =, etc.
				token_numeric[num_tokens] = 0;

#ifdef PARSER_DEBUG_ON
				printf("\nFound Literal token = %s", token_string[num_tokens]);
#endif

				// advance token scanner
				tok_start = tok_end + 2;

				// increment number of tokens
				num_tokens++;

			} break;

			case PATTERN_TOKEN_STRING: // ascii string varying length
			{
				// look for comparator
				if (pattern[tok_start + 2] == '=' ||
					pattern[tok_start + 2] == '>' ||
					pattern[tok_start + 2] == '<')
				{
					// extract the number
					tok_end = tok_start + 3;

					while (isdigit(pattern[tok_end]))
						tok_end++;

					// check for well formed
					if (pattern[tok_end] != ']')
						return(0);

					// copy number in ascii to string and convert to real number
					memcpy(buffer, &pattern[tok_start + 3], (tok_end - tok_start));
					buffer[tok_end - tok_start] = 0;

					// insert a string into pattern
					token_type[num_tokens] = PATTERN_TOKEN_STRING;     // type of token, f,i,s,l
					strcpy(token_string[num_tokens], "");               // for literal strings this holds them
					token_operator[num_tokens] = pattern[tok_start + 2]; // holds and operators for the token, >, <, =, etc.
					token_numeric[num_tokens] = atoi(buffer);

				} // end if
				else
					return(0); // not well formed

#ifdef PARSER_DEBUG_ON
				printf("\nFound String token, comparator: %c, characters: %d", token_operator[num_tokens], token_numeric[num_tokens]);
#endif
				// advance token scanner
				tok_start = tok_end + 1;

				// increment number of tokens
				num_tokens++;

			} break;

			default: break;

			} // end switch

		} // end if

	 // end of line?
		if (tok_start >= strlen(pattern))
			break;

	} // end while

#ifdef PARSER_DEBUG_ON
	printf("\nstring to parse: %s", string);
	printf("\nPattern to scan for: %s", pattern);
	printf("\nnumber of tokens found %d", num_tokens);
#endif

	// at this point we have the pattern we need to look for, so look for it
	int pattern_state = PATTERN_STATE_INIT; // initial state for pattern recognizer
	int curr_tok = 0;                 // test for num_tokens
	char token[PATTERN_BUFFER_SIZE];  // token under consideration

	// enter scan state machine
	while (1)
	{
		switch (pattern_state)
		{
		case PATTERN_STATE_INIT:
		{
			// initial state for pattern
			strcpy(buffer, string);

			tok_start = 0;
			tok_end = 0;
			tok_restart = 0;
			tok_first_pass = 1;
			curr_tok = 0;

			// reset output arrays
			num_pints = num_pfloats = num_pstrings = 0;

			// transition to restart         
			pattern_state = PATTERN_STATE_RESTART;

		} break;

		case PATTERN_STATE_RESTART:
		{
			// pattern may still be here?
			curr_tok = 0;
			tok_first_pass = 1;

			// error detection
			if (tok_end >= strlen(buffer))
				return(0);

			// restart scanner after first token from last pass
			tok_start = tok_end = tok_restart;

			// start validating tokens
			pattern_state = PATTERN_STATE_NEXT;

		} break;

		case PATTERN_STATE_NEXT:
		{
			// have we matched pattern yet?
			if (curr_tok >= num_tokens)
			{
				pattern_state = PATTERN_STATE_MATCH;
			}
			else
			{
				// get next token
				if (tok_end >= strlen(buffer))
					return(0);

				tok_start = tok_end;
				while (isspace(buffer[tok_start])) tok_start++;
				tok_end = tok_start;

				while (!isspace(buffer[tok_end]) && tok_end < strlen(buffer)) tok_end++;

				// copy token
				memcpy(token, &buffer[tok_start], tok_end - tok_start);
				token[tok_end - tok_start] = 0;

				// check for error
				if (strlen(token) == 0)
					return(0);

				// remember position of first token, so we can restart after it on next pass
				// if need
				if (tok_first_pass)
				{
					tok_first_pass = 0;
					tok_restart = tok_end;
				} // end if

			// we have the token, set state to check for that token
				switch (token_type[curr_tok])
				{
				case PATTERN_TOKEN_FLOAT:
				{
					pattern_state = PATTERN_STATE_FLOAT;
				} break;
				case PATTERN_TOKEN_INT:
				{
					pattern_state = PATTERN_STATE_INT;
				} break;
				case PATTERN_TOKEN_STRING:
				{
					pattern_state = PATTERN_STATE_STRING;
				} break;
				case PATTERN_TOKEN_LITERAL:
				{
					pattern_state = PATTERN_STATE_LITERAL;
				} break;

				default: break;

				} // end switch

			} // end else

		} break;

		case PATTERN_STATE_FLOAT:
		{
			// simply validate this token as a float
			float f = IsFloat(token);

			if (f != FLT_MIN)
			{
				pfloats[num_pfloats++] = f;

				// get next token
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				// error pattern doesn't match, restart
				pattern_state = PATTERN_STATE_RESTART;
			}

		} break;

		case PATTERN_STATE_INT:
		{
			// simply validate this token as a int
			int i = IsInt(token);

			if (i != INT_MIN)
			{
				pints[num_pints++] = i;

				// get next token
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			} // end if                
			else
			{
				// error pattern doesn't match, restart
				pattern_state = PATTERN_STATE_RESTART;
			}

		} break;

		case PATTERN_STATE_LITERAL:
		{
			// simply validate this token by comparing to data in table
			if (strcmp(token, token_string[curr_tok]) == 0)
			{
				// increment number of pstrings found and insert into table
				strcpy(pstrings[num_pstrings++], token);

				// get next token
				curr_tok++;
				pattern_state = PATTERN_STATE_NEXT;
			}
			else
			{
				// error pattern doesn't match, restart
				pattern_state = PATTERN_STATE_RESTART;
			}

		} break;

		case PATTERN_STATE_STRING:
		{
			// need to test for non-space chars 
			// get comparator

			switch (token_operator[curr_tok])
			{
			case '=':
			{
				// we need exactly
				if (strlen(token) == token_numeric[curr_tok])
				{
					// put this string into table
					strcpy(pstrings[num_pstrings++], token);

					// get next token
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				} // end if    
				else
				{
					// error pattern doesn't match, restart
					pattern_state = PATTERN_STATE_RESTART;
				}

			} break;

			case '>':
			{
				// we need greater than
				if (strlen(token) > token_numeric[curr_tok])
				{
					// put this string into table
					strcpy(pstrings[num_pstrings++], token);

					// get next token
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				} // end if  
				else
				{
					// error pattern doesn't match, restart
					pattern_state = PATTERN_STATE_RESTART;
				} // end else

			} break;

			case '<':
			{
				// we need less than
				if (strlen(token) < token_numeric[curr_tok])
				{
					// put this string into table
					strcpy(pstrings[num_pstrings++], token);

					// get next token
					curr_tok++;
					pattern_state = PATTERN_STATE_NEXT;
				} // end if    
				else
				{
					// error pattern doesn't match, restart
					pattern_state = PATTERN_STATE_RESTART;
				} // end else

			} break;

			default: break;

			} // end switch

		} break;

		case PATTERN_STATE_MATCH:
		{
			// we have matched the string, output vars into variable arg list
			ERROR_MSG("\nPattern: %s matched!", pattern);


			return(1);
		} break;

		case PATTERN_STATE_END: { } break;

		default: break;

		}
	}
}



char* ExtractFilenameFromPath(const char* filepath, char* filename)
{
	// this function extracts the filename from a complete path and file
	// "../folder/.../filname.ext"
	// the function operates by scanning backward and looking for the first 
	// occurance of "\" or "/" then copies the filename from there to the end
	// test of filepath is valid
	if (!filepath || strlen(filepath) == 0)
		return(NULL);

	int index_end = strlen(filepath) - 1;

	// find filename
	while ((filepath[index_end] != '\\') &&
		(filepath[index_end] != '/') &&
		(filepath[index_end] > 0))
		index_end--;

	// copy file name out into filename var
	memcpy(filename, &filepath[index_end + 1], strlen(filepath) - index_end);

	// return result
	return(filename);

} 
