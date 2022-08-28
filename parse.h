#ifndef BADPARSE_H
#define BADPARSE_H

// helpers has the bool def
#include "helpers.h"

typedef struct ArgThang {
	bool isMatch; // 
	int valueLength; // if > 0 then there is an inline value
	char value[256];
} ArgThang;

ArgThang getOption(char *find, char *line);

ArgThang getOption(char *find, char *line) {
	int findLength = strlen(find);
	int lineLength = strlen(line);
	int matches = 0;
	bool hasValue = false;
	ArgThang v = {0};
	int buffer = sizeof(v.value);
	for(int i = 0; i < findLength; i++) {
		if (hasValue) {
			if (i+matches < findLength) {
				if (i+1 < buffer) { // will this next character fit into the buffer?
					v.value[i] = line[i+matches];
					v.value[i+1] = '\0';
					v.valueLength = i+1;
					continue;
				}
				// segmentation fault goes here
				printf("the value has exceeded buffer size of %i by %i byte(s)\n", buffer, (findLength - matches) - buffer + 1);
				v.value[i] = '\0';
				v.valueLength = i;
				return v;
			}
			continue;
		};
		if (find[i] == line[matches]) {
			if (matches + 1 < lineLength) { // how many characters are left to match from line?
				if (line[matches+1] == '=') { // is this the end of line characters to find?
					if (i+1 < findLength && find[i+1] != ' ') { // there were more characters in the find set!
						while(i < findLength && find[i] != ' ') i++; // jump to the next find set and reset
						v.isMatch = false;
						hasValue = false;
						matches = 0;
						continue;
					};
					// this is a match and the inline value starts here
					hasValue = true;
					v.isMatch = true;
					// the remaining iterations belong to the hasValue god
					i = -1; // the hasValue god smites ye with segfault if'n's the index is not 0
					matches++; // sacrifice the = to the hasValue god
					findLength = lineLength;
				};
			}
			else { // there are no more line characters to find, but are there more items in this find set?
				if (i+1 < findLength && find[i+1] != ' ') { // there more characters to find in this set!
					while(i < findLength && find[i] != ' ') i++; // jump to the next set and reset
					v.isMatch = false;
					hasValue = false;
					matches = 0;
					continue;
				};
				v.isMatch = true; // things seem to match prolly
				return v;
			};
			matches++;
			continue;
		};
		// jump to next find set on no match
		while(i < findLength && find[i] != ' ') i++;
		v.isMatch = false;
		hasValue = false;
		matches = 0;
	};
	return v;
};

#endif