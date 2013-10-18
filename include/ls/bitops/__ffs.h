#ifndef __LS_BITOPS____FFS_H
#define __LS_BITOPS____FFS_H

#include <sys/cdefs.h>

/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static __always_inline unsigned long __ffs(unsigned long word)
{
	return __builtin_ctzl(word);
}

#endif /* __LS_BITOPS____FFS_H */
