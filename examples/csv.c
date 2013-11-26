#include <ls.h>

static int dump_csv(int line_number, const char * const * field, unsigned int n,
	       	void *user)
{
	int i;

	printf("|");
	for (i = 0; i < n; i++)
		printf("%s|", field[i]);
	printf("\n");

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
	strcat(path, "sample.csv");
	if (csv_parse(path, dump_csv, NULL))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
