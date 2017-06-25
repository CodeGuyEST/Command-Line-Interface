#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "dirent.h"
#include "built-in.h"
#include "command-descriptions.h"
#include "file.h"

void shell_del_d(char **tokens);
void shell_del_f(char **tokens);
void shell_cd(char **tokens);
void shell_create_d(char **tokens);
void shell_create_f(char **tokens);
void shell_exit(char **tokens);
void shell_help(char **tokens);
void shell_show_contents(char **tokens);
void shell_write_f(char **tokens);

char *current_path;

char *builtin_str[] = {
	"del_d",
	"del_f",
	"cd",
	"create_d",
	"create_f",
	"exit",
	"help",
	"show",
	"write_f"
};

void(*builtin_func[]) (char**) = {
	&shell_del_d,
	&shell_del_f,
	&shell_cd,
	&shell_create_d,
	&shell_create_f,
	&shell_exit,
	&shell_help,
	&shell_show_contents,
	&shell_write_f
};

int num_builtins();
char *join_tokens(char **tokens, char *delimiter, int start_position, int end_position);

int num_builtins() {
	return sizeof(builtin_str) / sizeof(char*);
}

int is_builtin(const char *command) {
	if (command == NULL) {
		return 0;
	}
	int builtins = num_builtins();
	for (int i = 0; i < builtins; i++) {
		if (strcmp(builtin_str[i], command) == 0) {
			return 1;
		}
	}
	return 0;
}

void built_in_execute(char **tokens) {
	const char *command = tokens[0];
	int builtins = num_builtins();
	for (int i = 0; i < builtins; i++) {
		if (strcmp(builtin_str[i], command) == 0) {
			(*builtin_func[i])(tokens);
		}
	}
}

char *join_tokens(char **tokens, char *delimiter, int start_position, int end_position) {
	char *next_token;

	size_t pathsize = 0;
	for (int i = start_position; i <= end_position; i++) {
		next_token = tokens[i];
		if (next_token == NULL)
			break;
		pathsize += strlen(next_token);
	}
	
	pathsize++;//For terminator
	pathsize += (end_position - start_position) * sizeof(delimiter);
	char *path = (char*)malloc(pathsize);
	strcpy_s(path, pathsize, tokens[start_position]);
	for (int i = start_position + 1; i <= end_position; i++) {
		if (tokens[i] == NULL)
			break;
		strcat_s(path, pathsize, delimiter);
		strcat_s(path, pathsize, tokens[i]);
	}
	return path;
}

void shell_del_f(char **tokens) {
	if (tokens[1] == NULL) {
		fprintf(stderr, "'del_f' expects a path.\n");
		return;
	}

	int token_count = 2;
	char *token = tokens[2];
	while (token != NULL) {
		token_count++;
		token = tokens[token_count];
	}
	char *path = join_tokens(tokens, " ", 1, token_count - 1);
	free(token);

	char *filename = concatenate_file_paths(current_path, path);

	//Check if file exists.
	if (!file_exists(filename)) {
		filename = path;
		if (!file_exists(filename)) {
			fprintf(stderr, "Couldn't find specified file.\n");
			return;
		}
	}
	
	//Remove file.
	int ret = remove(filename);
	if (ret == 0) {
		printf("%s deleted successfully.\n", filename);
	}
	else {
		perror("Unable to delete the file");
	}
}

void shell_cd(char **tokens){
	if (tokens[1] == NULL) {
		fprintf(stderr, "'cd' expects a path.\n");
		return;
	}

	char *path = join_tokens(tokens, " ", 1, 1000000);

	if (dir_exists(concatenate_file_paths(current_path, path))) {
		current_path = concatenate_file_paths(current_path, path);
	}
	else if (dir_exists(path)) {
		current_path = path;
	}
	else {
		fprintf(stderr, "Couldn't find specified path.\n");
	}
}

void shell_create_d(char **tokens) {
	char *path = join_tokens(tokens, " ", 1, 1000000);
	if (path == NULL) {
		fprintf(stderr, "'create_d' expects a path.\n");
		return;
	}
	int dir; 
	dir = _mkdir(concatenate_file_paths(current_path,path));
	if (dir != 0) {
		if (can_create_dir(path)) {
			dir = _mkdir(path);
		}
	}
	if (dir == 0) {
		printf("Directory created successfully.\n");
	}
	else {
		fprintf(stderr, "Unable to create directory.\n");
	}
}

void shell_create_f(char **tokens) {
	if (tokens[1] == NULL) {
		fprintf(stderr, "'create_f' expects a path.\n");
		return;
	}
	char *path = join_tokens(tokens, " ", 1, 10000);

	if (file_exists(path) || file_exists(concatenate_file_paths(current_path,path))) {
		fprintf(stderr, "Shell: File already exists.\n");
		free(path);
		return;
	}

	FILE *file;

	if (fopen_s(&file, concatenate_file_paths(current_path, path), "w") != 0) {
		if (fopen_s(&file, path, "w") != 0) {
			fprintf(stderr, "Shell: Unable to create file.\n");
			free(path);
			return;
		}
	}
	printf("File successfully created.\n");
	free(path);
	fclose(file);
}

void shell_del_d(char **tokens) {
	char *path = join_tokens(tokens, " ", 1, 1000000);
	if (path == NULL) {
		fprintf(stderr, "'del_dir' requires a path.\n");
		return;
	}
	char *absolute_path = concatenate_file_paths(current_path, path);
	if(dir_exists(absolute_path)){
		if (!remove_directory(absolute_path)) {
			printf("Directory successfully removed.\n");
			return;
		}
		else {
			fprintf(stderr,"Couldn't remove directory.\n");
		}
	}
	else if (dir_exists(path)) {
		if (!remove_directory(path)) {
			printf("Directory successfully removed.\n");
			return;
		}
		else {
			fprintf(stderr, "Couldn't remove directory.\n");
		}
	}
	else {
		fprintf(stderr, "Directory not found.\n");
	}
}

void shell_exit(char **tokens) {
	if (tokens[1] != NULL) {
		fprintf(stderr, "Too many parameters.\n");
	}
	else { exit(EXIT_SUCCESS); }
}

void shell_help(char **tokens) {
	if (tokens[1] != NULL) {
		fprintf(stderr,"Too many parameters.\n");
		return;
	}
	int i;
	int command_count = num_builtins();
	for (i = 0; i < command_count; i++) {
		printf(command_descriptions[i]);
	}
}

void shell_show_contents(char **tokens) {
	char *path = tokens[1] == NULL ? "" : join_tokens(tokens, " ", 1, 1000000);
	char *absolute_path = concatenate_file_paths(current_path, path);
	DIR *dir;
	if (dir_exists(absolute_path)) {
		dir = opendir(absolute_path);
	}
	else if (dir_exists(path)) {
		dir = opendir(path);
	}
	else {
		fprintf(stderr, "Couldn't find specified path.\n");
		return;
	}
	struct dirent *p;
	while ((p = readdir(dir)) != NULL) {
		if (!strcmp(".", p->d_name) || !strcmp("..", p->d_name)) {
			continue;
		}
		printf("%s\n", p->d_name);
	}
}

void shell_write_f(char **tokens){
	if (tokens[1] == NULL) {
		fprintf(stderr,"Not enough parameters.\n");
		return;
	}
	if (tokens[2] == NULL) {
		fprintf(stderr,"Not enough parameters.\n");
		return;
	}
	char *text = 0;
	char *path = 0;
	size_t pathsize = 1;
	int appending = 0;
	if (strcmp(tokens[1], "--a") == 0) {
		appending = 1;
	}
	int token_count = appending ? 2 : 1;
	char *token = tokens[token_count];
	while (token != NULL && token[0] != '\"') {
		token_count++;
		pathsize += strlen(token) * sizeof(char*) + 1;
		token = tokens[token_count];
	}
	if (pathsize <= 1) {
		fprintf(stderr, "No path specified.\n");
		return;
	}
	path = (char*)malloc(pathsize);
	if (!path) {
		fprintf(stderr,"Allocation error!\n");
		exit(EXIT_FAILURE);
	}
	strcpy_s(path,pathsize,join_tokens(tokens, " ", appending ? 2 : 1, token_count - 1));

	int textstart = token_count;
	size_t textbuffer = 0;
	token = tokens[token_count];
	if (token == NULL) {
		fprintf(stderr, 
			"Not enough parameters. Make sure that text is enclosed between double quotes(\"\").\n");
		free(path);
		return;
	}
	while (token != NULL) {
		textbuffer += strlen(token) * sizeof(char*) + 1;
		token_count++;
		token = tokens[token_count];
	}
	text = (char*)malloc(textbuffer);
	strcpy_s(text, textbuffer, join_tokens(tokens, " ", textstart, token_count - 1));

	if (text[strlen(text) - 1] != '\"') {
		fprintf(stderr,
			"Shell: Make sure that text is enclosed between double quotes(\") and no more parameters are entered.\n");
		free(text);
		free(path);
		return;
	}

	FILE *file;

	if (file_exists(path)) {
		if (fopen_s(&file, path, appending ? "a" : "w") != 0) {
			fprintf(stderr, "Shell: Unable to open specified file.\n");
			free(path);
			free(text);
			return;
		}
	}

	else if(file_exists(concatenate_file_paths(current_path, path))){
		if (fopen_s(&file, concatenate_file_paths(current_path, path), appending ? "a" : "w") != 0) {
			fprintf(stderr, "Shell: Unable to open specified file.\n");
			free(path);
			free(text);
			return;
		}
	}

	else {
		fprintf(stderr, "Shell: Couldn't find specified file.\n");
		free(path);
		free(text);
		return;
	}

	text[strlen(text) - 1] = '\0';
	fputs(&text[1], file);
	fclose(file);
	free(path);
	free(text);
	return;
}
