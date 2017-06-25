#include <stdio.h>
#include <stdlib.h>
#include "dirent.h"
#include "file.h"

char *concatenate_file_paths(char *path_1, char *path_2) {
	size_t bufsize = strlen(path_1) + strlen(path_2) + 1;
	if (path_1[0] != '\\') {
		bufsize++;
	}
	char *new_path = malloc(bufsize);
	if (!new_path) {
		perror("Allocation error.\n");
		exit(EXIT_FAILURE);
	}
	strcpy_s(new_path, bufsize, path_1);
	if (path_2[0] != '\\') {
		strcat_s(new_path, bufsize, "\\");
	}
	strcat_s(new_path, bufsize, path_2);
	return new_path;
}

int file_exists(char *filename) {
	FILE *file;
	if (fopen_s(&file, filename, "r") != 0) {
		return 0;
	}
	fclose(file);
	return 1;
}

int can_create_dir(char *path) {
	DIR *dir = opendir(path);
	if (dir)
	{	/* Directory exists. */
		closedir(dir);
		return 0;
	}
	else if (ENOENT == errno)
	{
		/* Directory does not exist. */
		return 1;
	}
	else
	{
		/* opendir() failed for some other reason. */
		return 0;
	}
}

int dir_exists(char *path) {
	DIR *dir = opendir(path);
	if (dir){
		closedir(dir);
		return 1;
	}
	return 0;
}

int remove_directory(const char *path)
{
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;

	if (d)
	{
		struct dirent *p;

		r = 0;

		while (!r && (p = readdir(d)))
		{
			int r2 = -1;
			char *buf;
			size_t len;

			/* Skip the names "." and ".." as we don't want to recurse on them. */
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
			{
				continue;
			}

			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);

			if (!buf) {
				fprintf(stderr, "Allocation error.\n");
				exit(EXIT_FAILURE);
			}

			else
			{
				struct stat statbuf;

				snprintf(buf, len, "%s\\%s", path, p->d_name);

				if (!stat(buf, &statbuf)){
					if (S_ISDIR(statbuf.st_mode)) {
						r2 = remove_directory(buf);
					}
					else {
						r2 = _unlink(buf);
					}
				}

				free(buf);
			}

			r = r2;
		}

		closedir(d);
	}

	if (!r)
	{
		r = rmdir(path);
	}

	return r;
}
