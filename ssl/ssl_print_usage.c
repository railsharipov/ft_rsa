#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>

void	ssl_print_usage(void)
{
	char	buf[256];
	int		rbytes;
	int		fd;

	if ((fd = open("./docs/usage.txt", O_RDONLY)) > 0)
	{
		while ((rbytes = read(fd, buf, sizeof(buf))) > 0) {
			write(STDOUT_FILENO, buf, rbytes);
		}
		close(fd);
	}
}
