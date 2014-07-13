/* RFC 3986 */

#include <ls.h>
#include <ctype.h>

static inline bool is_rfc3986_unreserved(unsigned char c)
{
	switch (c) {
	case 'a'...'z':
	case 'A'...'Z':
	case '0'...'9':
	case '-':
	case '_':
	case '.':
	case '~':
		return true;
	default:
		return false;
	}
}

/* URI producers are discouraged from percent-encoding unreserved characters. */
char *percent_encode(const void *raw, size_t size)
{
	size_t len = 0, i;
	const unsigned char *uc = raw;
	unsigned char *enc;

	for (i = 0; i < size; i++) {
		if (is_rfc3986_unreserved(uc[i]))
			++len;
		else
			len += 3;
	}

	enc = malloc(len + 1);
	if (!enc)
		return NULL;

	len = 0;
	for (i = 0; i < size; i++) {
		if (is_rfc3986_unreserved(uc[i])) {
			enc[len++] = uc[i];
		} else {
			enc[len++] = '%';
			enc[len++] = hex_asc_upper_hi(uc[i]);
			enc[len++] = hex_asc_upper_lo(uc[i]);
		}
	}
	enc[len] = '\0';

	return (char *)enc;
}

ssize_t percent_decode_in_place(void *enc, size_t len)
{
	unsigned char *to, *from, *end;

	to = from = enc;
	end = from + len;
	while (from < end) {
		if (*from == '%') {
			if (from + 2 >= end || !isxdigit(from[1]) ||
					!isxdigit(from[2]))
				return -1;
			*to = (hex_to_bin(from[1]) << 4) | hex_to_bin(from[2]);
			from += 2;
		} else {
			if (to != from)
				*to = *from;
		}
		++to;
		++from;
	}

	return to - (unsigned char *)enc;
}

#ifndef NDEBUG
UNIT_TEST void percent_encode_test(void)
{
	char *enc = percent_encode("I love you", 10);

	printf("%s\n", __FUNCTION__);
	assert(enc);
	assert(strcmp(enc, "I%20love%20you") == 0);
	assert(percent_decode_in_place(enc, 14) == 10);
	enc[10] = '\0';
	assert(strcmp(enc, "I love you") == 0);
	free(enc);
}
#endif /* NDEBUG */
