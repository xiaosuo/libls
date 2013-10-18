#ifndef __LS_BITOPS_FFZ_H
#define __LS_BITOPS_FFZ_H

#include <ls/bitops/__ffs.h>

/*
 * ffz - find first zero in word.
 * @word: The word to search
 *
 * Undefined if no zero exists, so code should check against ~0UL first.
 */
#define ffz(x)  __ffs(~(x))

#endif /* __LS_BITOPS_FFZ_H */
