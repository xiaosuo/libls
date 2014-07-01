#include <config.h>

#include <ls/base64.h>

#include <stdlib.h>

static const char base64_encode_table[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

/* RFC3548. */
char *base64_encode(const char *data, unsigned int len,
		unsigned int *res_len)
{
	const unsigned char *decoded = (const unsigned char *)data;
	unsigned int i, round = len / 3;
	char *encoded = malloc((len + 2) / 3 * 4 + 1);

	if (!encoded)
		return NULL;

	for (i = 0; i < round; i++) {
		encoded[i * 4] = base64_encode_table[decoded[i * 3] >> 2];
		encoded[i * 4 + 1] = base64_encode_table[((decoded[i * 3] & 0x03) << 4) | (decoded[i * 3 + 1] >> 4)];
		encoded[i * 4 + 2] = base64_encode_table[((decoded[i * 3 + 1] & 0x0f) << 2) | (decoded[i * 3 + 2] >> 6)];
		encoded[i * 4 + 3] = base64_encode_table[decoded[i * 3 + 2] & 0x3f];
	}

	switch (len - round * 3) {
	case 0:
		encoded[i * 4] = '\0';
		if (res_len)
			*res_len = i * 4;
		break;
	case 1:
		encoded[i * 4] = base64_encode_table[decoded[i * 3] >> 2];
		encoded[i * 4 + 1] = base64_encode_table[(decoded[i * 3] & 0x03) << 4];
		encoded[i * 4 + 2] = '=';
		encoded[i * 4 + 3] = '=';
		encoded[i * 4 + 4] = '\0';
		if (res_len)
			*res_len = i * 4 + 4;
		break;
	case 2:
		encoded[i * 4] = base64_encode_table[decoded[i * 3] >> 2];
		encoded[i * 4 + 1] = base64_encode_table[((decoded[i * 3] & 0x03) << 4) | (decoded[i * 3 + 1] >> 4)];
		encoded[i * 4 + 2] = base64_encode_table[(decoded[i * 3 + 1] & 0x0f) << 2];
		encoded[i * 4 + 3] = '=';
		encoded[i * 4 + 4] = '\0';
		if (res_len)
			*res_len = i * 4 + 4;
		break;
	default:
		abort();
	}

	return encoded;
}

static const signed char base64_decode_table[] = {
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -1, -2, -2,
	-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
	-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
};

char *base64_decode(const char *data, unsigned int len,
		unsigned int *res_len)
{
	const unsigned char *encoded;
	unsigned int i, round;
	unsigned char *decoded;
	int code;
	unsigned int decoded_len;

	if (len % 4)
		goto err;
	round = len / 4;
	decoded = malloc(round * 3 + 1);
	if (!decoded)
		goto err;
	encoded = (const unsigned char *)data;
	for (i = 0; i < round; i++) {
		code = base64_decode_table[encoded[i * 4]];
		if (code < 0)
			goto err2;
		decoded[i * 3] = code << 2;

		code = base64_decode_table[encoded[i * 4 + 1]];
		if (code < 0)
			goto err2;
		decoded[i * 3] |= code >> 4;
		decoded[i * 3 + 1] = code << 4;

		code = base64_decode_table[encoded[i * 4 + 2]];
		if (code < -1) {
			goto err2;
		} else if (code == -1) {
			decoded_len = round * 3 - 2;
			break;
		} else {
			decoded[i * 3 + 1] |= code >> 2;
			decoded[i * 3 + 2] = code << 6;
		}

		code = base64_decode_table[encoded[i * 4 + 3]];
		if (code < -1) {
			goto err2;
		} else if (code == -1) {
			decoded_len = round * 3 - 1;
			break;
		} else {
			decoded[i * 3 + 2] |= code;
		}
	}
	if (i == round)
		decoded_len = round * 3;
	else if (i != round - 1)
		goto err2;
	decoded[decoded_len] = '\0';
	if (res_len)
		*res_len = decoded_len;

	return (char *)decoded;
err2:
	free(decoded);
err:
	return NULL;
}
