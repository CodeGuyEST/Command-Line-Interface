#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "line-parser.h"

#define RL_BUFSIZE 128

char *shell_read_line(void) {
	int bufsize = RL_BUFSIZE;
	int position = 0;
	char *buffer;
	int c;

	buffer = (char*)malloc(sizeof(char) * bufsize);

	if (!buffer) {
		fprintf(stderr, "Shell:allocation error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getchar();
		if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		}
		else {
			buffer[position] = c;
		}

		position++;

		if (position >= bufsize) {
			bufsize += RL_BUFSIZE;
			buffer = (char *)realloc(buffer, bufsize);
			if (!buffer) {
		        fprintf(stderr, "Allocation error.\n");
		        exit(EXIT_FAILURE);
	        }
		}
	}
}

#define TOK_SIZE 32

char **shell_split_string(char *str, const char *delimiter) {
	int position = 0;
	int bufsize = TOK_SIZE;
	char *next_token;
	char **tokens = (char**)malloc(bufsize * sizeof(char*));

	if (!tokens) {
		fprintf(stderr, "Allocation error");
		exit(EXIT_FAILURE);
	}

	char *token = (char*)strtok_s(str, delimiter, &next_token);
	while (token != NULL) {
		tokens[position] = token;
		if (position >= bufsize) {
			bufsize += TOK_SIZE;
			tokens = (char**)realloc(tokens, bufsize * sizeof(char));
			if (!tokens) {
				fprintf(stderr, "Allocation error");
				exit(EXIT_FAILURE);
			}
		}
		position++;
		token = strtok_s(NULL, delimiter, &next_token);
	}
	tokens[position] = NULL;
	return tokens;
}
