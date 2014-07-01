#ifndef __BASE64_H
#define __BASE64_H

char *base64_encode(const char *data, unsigned int len, unsigned int *res_len);
char *base64_decode(const char *data, unsigned int len, unsigned int *res_len);

#endif /* __BASE64_H */
