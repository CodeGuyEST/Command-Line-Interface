#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "built-in.h"
#include "line-parser.h"

void main_loop(void);

extern char *current_path;
char *current_path;

int main(int argc, char *argv[]) {
	current_path =(char*)_getcwd(current_path, 0);
	main_loop();
	return 0;
}

void main_loop(void) {
	while (1) {
		printf("%s ", current_path);
		char *line = shell_read_line();
		char **tokens = shell_split_string(line, " ");
		if(tokens[0] == NULL){
			continue;
		}
		if (is_builtin(tokens[0])) {
			built_in_execute(tokens);
		}
		else {
			fprintf(stderr, "No command matching input./n Use 'help' command for command references.");
		}
	}
}



