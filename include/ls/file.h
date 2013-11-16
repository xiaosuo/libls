#ifndef __LS_FILE_H
#define __LS_FILE_H

int file_each_line(const char *filename,
		int (*cb)(char *line, unsigned int len, void *user),
		void *user);

#endif /* __LS_FILE_H */
