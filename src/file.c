#include "ls/file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int file_each_line(const char *filename,
		int (*cb)(char *line, unsigned int len, void *user), void *user)
{
	FILE *fh = fopen(filename, "r");
	char *buf;
	unsigned int buf_size = LINE_MAX;
	unsigned int len = 0;
	int retval = -1;

	if (!fh)
		goto err;
	buf = malloc(LINE_MAX);
	if (!buf)
		goto err2;
	while (fgets(buf + len, buf_size - len, fh)) {
		len += strlen(buf + len);
		if (buf[len - 1] != '\n' && len == buf_size - 1 && !feof(fh)) {
			unsigned int new_buf_size = buf_size * 2;
			char *new_buf = realloc(buf, new_buf_size);

			if (!new_buf)
				goto err3;
			buf = new_buf;
			buf_size = new_buf_size;
			continue;
		}
		if (cb(buf, len, user))
			goto err3;
		len = 0;
	}
	if (ferror(fh))
		goto err3;
	retval = 0;
err3:
	free(buf);
err2:
	fclose(fh);
err:
	return retval;
}
