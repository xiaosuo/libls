#include <ls/string.h>
#include <ls/stddef.h>

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**
 * vscnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @args: Arguments for the format string
 *
 * The return value is the number of characters which have been written into
 * the @buf not including the trailing '\0'. If @size is == 0 the function
 * returns 0.
 *
 * If you're not already dealing with a va_list consider using scnprintf().
 *
 * See the vsnprintf() documentation for format string extensions over C99.
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	int i;

	i = vsnprintf(buf, size, fmt, args);

	if (likely(i < size))
		return i;
	if (size != 0)
		return size - 1;
	return 0;
}

/**
 * scnprintf - Format a string and place it in a buffer
 * @buf: The buffer to place the result into
 * @size: The size of the buffer, including the trailing null space
 * @fmt: The format string to use
 * @...: Arguments for the format string
 *
 * The return value is the number of characters written into @buf not including
 * the trailing '\0'. If @size is == 0 the function returns 0.
 */
int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vscnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

char *strlstrip(char *str)
{
	while (isspace(*(unsigned char *)str))
		str++;

	return str;
}

char *strrstrip(char *str)
{
	char *end = str + strlen(str) - 1;

	while (end >= str && isspace(*(unsigned char *)end))
		*end-- = '\0';

	return str;
}

char *strstrip(char *str)
{
	return strrstrip(strlstrip(str));
}

char *astrncat(char *dest, const char *src, unsigned int src_len)
{
	int dest_len = dest ? strlen(dest) : 0;

	dest = realloc(dest, dest_len + src_len + 1);
	if (dest) {
		memcpy(dest + dest_len, src, src_len);
		dest[dest_len + src_len] = '\0';
	}

	return dest;
}

char *astrcat(char *dest, const char *src)
{
	return astrncat(dest, src, strlen(src));
}

void strv_free(struct strv *sv)
{
	int i;

	for (i = 0; i < sv->len; i++)
		free(sv->strv[i]);
	free(sv->strv);
}

int strv_append3(struct strv *sv, const char *str, int len)
{
	if (sv->len >= sv->size) {
		unsigned int new_size;
		char **new_strv;

		new_size = sv->size * 2;
		if (!new_size)
			new_size = 1;
		new_strv = realloc(sv->strv, sizeof(*new_strv) * new_size);
		if (!new_strv)
			goto err;
		sv->strv = new_strv;
		sv->size = new_size;
	}
	if (!(sv->strv[sv->len++] = strndup(str, len)))
		goto err;

	return 0;
err:
	return -1;
}

int strv_append(struct strv *sv, const char *str)
{
	return strv_append3(sv, str, strlen(str));
}
