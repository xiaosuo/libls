#ifndef __LS_UNIT_TEST_H
#define __LS_UNIT_TEST_H

#ifdef NDEBUG
#define UNIT_TEST static __attribute__((unused))
#define UNIT_TEST1(priority) static __attribute__((unused))
#else
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR >= 3)
#define UNIT_TEST static __attribute__((constructor))
#define UNIT_TEST1(priority) static __attribute__((constructor(priority + 101)))
#else
#define UNIT_TEST static __attribute__((constructor))
#endif
#endif

#endif /* __LS_UNIT_TEST_H */
