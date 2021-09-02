#include "La_Parser.h"
#include <iostream>
int main(int argc, char** argv)
{
	ASTRING path = "F:\\Git_WorkSpace\\La_Library\\La_Library\\Resource\\demo46\\";
	CPARSERV1 parser;     // create a parser
	char filename[80] = "test1.txt";    // holds fileaname
	char pattern[80];     // holds pattern
	int sel;              // user input

	// enter main loop
	while (1)
	{
		// display menu
		printf("\nParser Menu:\n");
		printf("\n1. Enter in a filename to load.");
		printf("\n2. Display file untouched.");
		printf("\n3. Display file with all comments removed (comments begin with #).");
		printf("\n4. Display file with all blank lines and comments removed.");
		printf("\n5. Search file for pattern.");
		printf("\n6. Exit program.\n");
		printf("\nPlease make a selection?");

		// get user selection
		std::cin >> sel;

		// what to do?
		switch (sel)
		{
		case 1: // enter filename
		{
			printf("\nEnter filename?");
			scanf("%s", filename);
		} break;

		case 2: // display file untouched
		{
			// open the file
			if (!parser.open(path + filename))
			{
				printf("\nFile not found!"); break;
			}

			int line = 0;

			// loop until no more lines
			while (parser.getline(0))
				printf("line %d: %s", line++, parser.buffer);

			parser.close();

		} break;

		case 3: // display file with comments removed
		{
			// open the file
			if (!parser.open(path + filename))
			{
				printf("\nFile not found!"); break;
			}

			int line = 0;

			// loop until no more lines
			while (parser.getline(PARSER_STRIP_COMMENTS))
				printf("line %d: %s", line++, parser.buffer);

			parser.close();
		} break;

		case 4: // Display file with all blank lines and comments removed
		{
			// open the file
			if (!parser.open(path + filename))
			{
				printf("\nFile not found!"); break;
			}

			int line = 0;

			// loop until no more lines
			while (parser.getline(PARSER_STRIP_COMMENTS | PARSER_STRIP_EMPTY_LINES))
				printf("line %d: %s", line++, parser.buffer);

			parser.close();
		} break;

		case 5: // Search file for pattern
		{
			// [i] [i] [i] ≤‚ ‘
			printf("\nEnter pattern to match?");
			std::cin.getline(pattern, 80);
			while (pattern[0] == '\0')
				std::cin.getline(pattern, 80);
			// open the file
			if (!parser.open(path + filename))
			{
				printf("\nFile not found!"); break;
			}

			// loop until no more lines
			while (parser.getline(PARSER_STRIP_COMMENTS | PARSER_STRIP_EMPTY_LINES))
			{
				// does this line have the pattern?
				if (parser.match(parser.buffer, pattern))
				{
					printf("\nPattern: \"%s\" matched, stats:", pattern);

					printf("\n%d ints matched", parser.num_pints);
					for (int i = 0; i < parser.num_pints; i++)
						printf("\nInt[%d]: %i", i, parser.pints[i]);

					printf("\n%d floats matched", parser.num_pfloats);
					for (int f = 0; f < parser.num_pfloats; f++)
						printf("\nFloat[%d]: %f", f, parser.pfloats[f]);

					printf("\n%d strings matched", parser.num_pstrings);
					for (int s = 0; s < parser.num_pstrings; s++)
						printf("\nString[%d]: %s", s, parser.pstrings[s]);
				}
			}
			parser.close();
		} break;


		case 6:
		{
			exit(0);
		} break;

		}
	}
	return argc;
}