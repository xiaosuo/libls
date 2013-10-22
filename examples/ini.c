#include <ls.h>
#include <stdio.h>

static char *current_section = NULL;

static int dump_ini(int line_number, const char *section, const char *name,
		const char *value, void *user)
{
	if (section) {
		if (!current_section || strcmp(current_section, section) != 0) {
			free(current_section);
			current_section = strdup(section);
			if (!current_section) {
				fprintf(stderr, "OOM\n");
				return -1;
			}
			printf("[%s]\n", current_section);
		}
	}
	printf("%s=\"%s\"\n", name, value);

	return 0;
}

int main(int argc, char *argv[])
{
	char path[PATH_MAX];
	char *sep;

	strcpy(path, argv[0]);
	sep = strrchr(path, '/');
	if (sep)
		*(sep + 1) = '\0';
	strcat(path, "sample.ini");
	if (ini_parse(path, dump_ini, NULL))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
