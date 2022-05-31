#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

void exit_with_error(char *str)
{
	if (errno)
		perror(NULL);
	else if (str)
		printf("%s\n", str);

	exit(0);
}

int	main(int ac, char **av)
{
	int 	file_fd;
	int		rand_fd;
	int		bufsize = 0;
	char	*buf = NULL;

	if (ac != 3)
		exit_with_error("usage: genrb <filename> <filesize>");

	bufsize = strtol(av[2], NULL, 10);
	buf = malloc(bufsize);

	if (NULL == buf)
		exit_with_error(NULL);

	if ((file_fd = open(av[1], O_RDWR | O_CREAT, 0644)) < 0)
		exit_with_error(NULL);

	if ((rand_fd = open("/dev/random", O_RDONLY)) < 0)
		exit_with_error(NULL);

	if (read(rand_fd, buf, bufsize) != bufsize)
		exit_with_error(NULL);

	if (write(file_fd, buf, bufsize) != bufsize)
		exit_with_error(NULL);

	return (0);
}
