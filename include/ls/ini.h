#ifndef __LS_INI_H
#define __LS_INI_H

int ini_parse(const char *filename, int (*callback)(int line_number,
	      const char *section, const char *name, const char *value,
	      void *user), void *user);

#endif /* __LS_INI_H */
