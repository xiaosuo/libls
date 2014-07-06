#ifndef __LS_CSV_H
#define __LS_CSV_H

int csv_parse(const char *filename, int (*callback)(int line_number,
			const char * const * field, unsigned int n, void *user),
	       	void *user);

#endif /* __LS_CSV_H */
