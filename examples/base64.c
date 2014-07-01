#include <ls.h>

int main()
{
	unsigned int res_len;
	char *encoded;
	char *decoded;

	/* RFC3548: 7. */
	encoded = base64_encode("\x14\xfb\x9c\x03\xd9\x7e", 6, &res_len);
	assert(strcmp(encoded, "FPucA9l+") == 0);
	assert(res_len == 8);
	decoded = base64_decode(encoded, 8, &res_len);
	assert(strcmp(decoded, "\x14\xfb\x9c\x03\xd9\x7e") == 0);
	assert(res_len == 6);
	free(encoded);
	free(decoded);

	encoded = base64_encode("\x14\xfb\x9c\x03\xd9", 5, &res_len);
	assert(strcmp(encoded, "FPucA9k=") == 0);
	assert(res_len == 8);
	decoded = base64_decode(encoded, 8, &res_len);
	assert(strcmp(decoded, "\x14\xfb\x9c\x03\xd9") == 0);
	assert(res_len == 5);
	free(encoded);
	free(decoded);

	encoded = base64_encode("\x14\xfb\x9c\x03", 4, &res_len);
	assert(strcmp(encoded, "FPucAw==") == 0);
	assert(res_len == 8);
	decoded = base64_decode(encoded, 8, &res_len);
	assert(strcmp(decoded, "\x14\xfb\x9c\x03") == 0);
	assert(res_len == 4);
	free(encoded);
	free(decoded);

	return 0;
}
