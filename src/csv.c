/* RFC 4180 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <ls/csv.h>
#include <ls/file.h>
#include <ls/string.h>

struct csv_parse_context {
	unsigned int	line_number;
	struct strv	field;
	int		(*callback)(int line_number, const char * const *field,
				    unsigned int n, void *user);
	void		*user;
	bool		quoted;
};

static int append_to_last_field(struct csv_parse_context *ctx, const char *str,
		unsigned int len)
{
	char *new_str = astrncat(ctx->field.strv[ctx->field.len - 1], str, len);

	if (!new_str)
		return -1;
	ctx->field.strv[ctx->field.len - 1] = new_str;

	return 0;
}

static int parse_csv(char *line, unsigned int len, void *user)
{
	struct csv_parse_context *ctx = user;
	char *ptr;

	ctx->line_number++;
	do {
		if (ctx->quoted) {
			ptr = memchr(line, '\"', len);
			if (!ptr) {
				if (append_to_last_field(ctx, line, len))
					goto err;
				goto out;
			}
			if (ptr + 1 < line + len && *(ptr + 1) == '\"') {
				ptr++;
				if (append_to_last_field(ctx, line, ptr - line))
					goto err;
				ptr++;
				len -= ptr - line;
				line = ptr;
			} else {
				if (append_to_last_field(ctx, line, ptr - line))
					goto err;
				ctx->quoted = false;
				ptr++;
				len -= ptr - line;
				line = ptr;
				if (len == 0)
					break;
				if (*line == ',') {
					len--;
					line++;
				} else {
					if (len > 2) {
						goto err;
					} else if (len == 2) {
						if (strcmp(line, "\r\n") != 0)
							goto err;
					} else if (len == 1) {
						if (*line != '\r' &&
						    *line != '\n')
							goto err;
					}
					break;
				}
			}
		} else if (*line == '\"') {
			ctx->quoted = true;
			if (strv_append3(&ctx->field, "", 0))
				goto err;
			line++;
			len--;
		} else {
			ptr = memchr(line, ',', len);
			if (!ptr) {
				if (len > 0) {
					if (line[len - 1] == '\n') {
						line[--len] = '\0';
						if (len > 0 &&
						    line[len - 1] == '\r')
							line[--len] = '\0';
					} else if (line[len - 1] == '\r') {
						line[--len] = '\0';
					}
				}
				if (strv_append3(&ctx->field, line, len))
					goto err;
				break;
			} else {
				if (strv_append3(&ctx->field, line, ptr - line))
					goto err;
				++ptr;
				len -= ptr - line;
				line = ptr;
			}
		}
	} while (len > 0);

	if (ctx->callback(ctx->line_number,
			  (const char * const *)ctx->field.strv,
			  ctx->field.len, ctx->user))
		goto err;
	strv_free(&ctx->field);
	strv_init(&ctx->field);
out:
	return 0;
err:
	return -1;
}

int csv_parse(const char *filename, int (*callback)(int line_number,
			const char * const * field, unsigned int n, void *user),
	       	void *user)
{
	struct csv_parse_context *ctx;
	int retval = -1;

	ctx = calloc(1, sizeof(*ctx));
	if (!ctx)
		goto err;
	ctx->callback = callback;
	ctx->user = user;
	if (file_each_line(filename, parse_csv, ctx))
		goto err2;
	if (ctx->quoted)
		goto err2;
	retval = 0;
err2:
	strv_free(&ctx->field);
err:
	return retval;
}
