#ifndef __LS_TYPES_H
#define __LS_TYPES_H

#include <stdint.h>

#include <sys/param.h>
#include <sys/cdefs.h>

#include <linux/types.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define BITS_PER_LONG		__WORDSIZE
#define BIT(nr)			(1UL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	howmany(nr, BITS_PER_BYTE * sizeof(long))

#endif /* __LS_TYPES_H */
