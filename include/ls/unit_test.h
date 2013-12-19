#ifndef __LS_UNIT_TEST_H
#define __LS_UNIT_TEST_H

#ifdef NDEBUG
#define UNIT_TEST(priority) static __attribute__((unused))
#else
#define UNIT_TEST(priority) static __attribute__((constructor(priority)))
#endif

#endif /* __LS_UNIT_TEST_H */
