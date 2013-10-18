#ifndef __LS_BITOPS___FLS_H
#define __LS_BITOPS___FLS_H

#include <sys/cdefs.h>

/**
 * __fls - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static __always_inline unsigned long __fls(unsigned long word)
{
	return (sizeof(word) * 8) - 1 - __builtin_clzl(word);
}

#endif /* __LS_BITOPS___FLS_H */
