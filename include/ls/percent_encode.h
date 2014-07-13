#ifndef __LS_PERCENT_ENCODE_H
#define __LS_PERCENT_ENCODE_H

#include <sys/types.h>

char *percent_encode(const void *raw, size_t size);
ssize_t percent_decode_in_place(void *enc, size_t len);

#endif /* __LS_PERCENT_ENCODE_H */
