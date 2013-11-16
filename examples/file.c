#include "ls.h"

static unsigned int line_number = 0;

int dump(char *line, unsigned int len, void *user)
{
	printf("%u\t", ++line_number);
	fwrite(line, len, 1, stdout);

	return 0;
}

int main(int argc, char *argv[])
{
	if (file_each_line(argv[1], dump, NULL))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
