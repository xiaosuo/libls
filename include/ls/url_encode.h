#ifndef __LS_URL_ENCODE_H
#define __LS_URL_ENCODE_H

#include <sys/types.h>

char *url_encode(const void *raw, size_t size);
ssize_t url_decode_in_place(void *enc, size_t len);

#endif /* __LS_URL_ENCODE_H */
