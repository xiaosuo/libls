#include <ls/ini.h>
#include <ls/string.h>

#include <stdio.h>
#include <string.h>
#include <limits.h>

int ini_parse(const char *filename, int (*callback)(int line_number,
	      const char *section, const char *name, const char *value,
	      void *user), void *user)
{
	FILE *fh;
	char line[LINE_MAX], section_buffer[LINE_MAX], *section = NULL;
	int retval = -1, line_number = 0;

	fh = fopen(filename, "r");
	if (!fh)
		goto err;
	while (fgets(line, sizeof(line), fh)) {
		int len = strlen(line);
		char *name, *value;

		if (len == 0 || line[len - 1] != '\n') {
			if (!feof(fh)) /* The size of line isn't big enough. */
				goto err2;
		} else {
			line[--len] = '\0';
		}
		++line_number;

		/* Ignore the comments start with ';' and '#'. */
		if (line[0] == ';' || line[0] == '#')
			continue;

		/* Ignore the leading and trailing white spaces. */
		name = strstrip(line);
		len = strlen(name);
		if (len == 0)
			continue;

		/* Parse the section section. */
		if (name[0] == '[') {
			if (name[len - 1] != ']' || len <= 2)
				goto err2;
			if (!section)
				section = section_buffer;
			memcpy(section, name + 1, len - 2);
			section[len - 2] = '\0';
			continue;
		}

		/* Parse the name and value. */
		value = strchr(line, '=');
		if (!value)
			goto err2;
		*value++ = '\0';
		/* Ignore spaces. */
		name = strrstrip(name);
		value = strlstrip(value);
		if (!*name)
			goto err2;
		if (*value == '"') {
			len = strlen(value);
			if (len >= 2 && value[len - 1] == '"') {
				value++;
				value[len - 2] = '\0';
			}
		}

		if (callback(line_number, section, name, value, user))
			goto err2;
	}
	if (ferror(fh) || !feof(fh))
		goto err2;
	retval = 0;
err2:
	fclose(fh);
err:
	return retval;
}
