#include <ls/ini.h>
#include <ls/string.h>
#include <ls/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct ini_parse_context {
	char		*section;
	unsigned int	line_number;
	int		(*callback)(int line_number, const char *section,
				    const char *name, const char *value,
				    void *user);
	void		*user;
};

static int parse_ini(char *line, unsigned int len, void *user)
{
	struct ini_parse_context *ctx = user;
	char *name, *value;

	if (line[len - 1] == '\n')
		line[--len] = '\0';
	ctx->line_number++;

	/* Ignore the comments start with ';' and '#'. */
	if (line[0] == ';' || line[0] == '#')
		goto out;

	/* Ignore the leading and trailing white spaces. */
	name = strstrip(line);
	len = strlen(name);
	if (len == 0)
		goto out;

	/* Parse the section section. */
	if (name[0] == '[') {
		if (name[len - 1] != ']' || len <= 2)
			goto err;
		free(ctx->section);
		ctx->section = strndup(name + 1, len - 2);
		if (!ctx->section)
			goto err;
		goto out;
	}

	/* Parse the name and value. */
	value = strchr(line, '=');
	if (!value)
		goto err;
	*value++ = '\0';
	/* Ignore spaces. */
	name = strrstrip(name);
	value = strlstrip(value);
	if (!*name)
		goto err;
	if (*value == '"') {
		len = strlen(value);
		if (len >= 2 && value[len - 1] == '"') {
			value++;
			value[len - 2] = '\0';
		}
	}

	if (ctx->callback(ctx->line_number, ctx->section, name, value,
				ctx->user))
		goto err;
out:
	return 0;
err:
	return -1;
}

int ini_parse(const char *filename, int (*callback)(int line_number,
	      const char *section, const char *name, const char *value,
	      void *user), void *user)
{
	struct ini_parse_context *ctx;
	int retval = -1;

	ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		goto err;
	ctx->callback = callback;
	ctx->user = user;
	if (file_each_line(filename, parse_ini, ctx))
		goto err2;
	retval = 0;
err2:
	free(ctx->section);
	free(ctx);
err:
	return retval;
}
