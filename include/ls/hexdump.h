#ifndef __LS_HEXDUMP_H
#define __LS_HEXDUMP_H

#include <ls/types.h>

#include <stdbool.h>

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

extern const char hex_asc[];
#define hex_asc_lo(x)	hex_asc[((x) & 0x0f)]
#define hex_asc_hi(x)	hex_asc[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack(char *buf, u8 byte)
{
	*buf++ = hex_asc_hi(byte);
	*buf++ = hex_asc_lo(byte);
	return buf;
}

extern const char hex_asc_upper[];
#define hex_asc_upper_lo(x)	hex_asc_upper[((x) & 0x0f)]
#define hex_asc_upper_hi(x)	hex_asc_upper[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack_upper(char *buf, u8 byte)
{
	*buf++ = hex_asc_upper_hi(byte);
	*buf++ = hex_asc_upper_lo(byte);
	return buf;
}

int hex_to_bin(char ch);
int hex2bin(u8 *dst, const char *src, size_t count);
void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
			int groupsize, char *linebuf, size_t linebuflen,
			bool ascii);
void print_hex_dump(const char *level, const char *prefix_str, int prefix_type,
		    int rowsize, int groupsize,
		    const void *buf, size_t len, bool ascii);
void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
			  const void *buf, size_t len);

#endif /* __LS_HEXDUMP_H */
