#ifndef __LS_BITOPS_HWEIGHT_H
#define __LS_BITOPS_HWEIGHT_H

#include <ls/types.h>

/*
 * Compile time versions of __sw_hweightN()
 */
#define __const_hweight8(w)		\
      (	(!!((w) & (1ULL << 0))) +	\
	(!!((w) & (1ULL << 1))) +	\
	(!!((w) & (1ULL << 2))) +	\
	(!!((w) & (1ULL << 3))) +	\
	(!!((w) & (1ULL << 4))) +	\
	(!!((w) & (1ULL << 5))) +	\
	(!!((w) & (1ULL << 6))) +	\
	(!!((w) & (1ULL << 7)))	)

#define __const_hweight16(w) (__const_hweight8(w)  + __const_hweight8((w)  >> 8 ))
#define __const_hweight32(w) (__const_hweight16(w) + __const_hweight16((w) >> 16))
#define __const_hweight64(w) (__const_hweight32(w) + __const_hweight32((w) >> 32))

unsigned int __sw_hweight8(unsigned int w);
unsigned int __sw_hweight16(unsigned int w);
unsigned int __sw_hweight32(unsigned int w);
unsigned long __sw_hweight64(__u64 w);

/*
 * Generic interface.
 */
#define hweight8(w)  (__builtin_constant_p(w) ? __const_hweight8(w)  : __sw_hweight8(w))
#define hweight16(w) (__builtin_constant_p(w) ? __const_hweight16(w) : __sw_hweight16(w))
#define hweight32(w) (__builtin_constant_p(w) ? __const_hweight32(w) : __sw_hweight32(w))
#define hweight64(w) (__builtin_constant_p(w) ? __const_hweight64(w) : __sw_hweight64(w))

/*
 * Interface for known constant arguments
 */
#define HWEIGHT8(w)  __const_hweight8(w)
#define HWEIGHT16(w) __const_hweight16(w)
#define HWEIGHT32(w) __const_hweight32(w)
#define HWEIGHT64(w) __const_hweight64(w)

/*
 * Type invariant interface to the compile time constant hweight functions.
 */
#define HWEIGHT(w)   HWEIGHT64((u64)w)

#endif /* __LS_BITOPS_HWEIGHT_H */
