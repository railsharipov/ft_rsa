#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <ssl.h>

void	cli_print_usage(void)
{
	char	buf[256];
	char	*message;
	ssize_t	rbytes;
	int		fd;

	if ((fd = open("./docs/usage.txt", O_RDONLY)) > 0) {
		while ((rbytes = read(fd, buf, sizeof(buf) - 1)) > 0) {
			buf[rbytes] = '\0';
			CLI_LOG(INFO, "%s", buf);
		}
		close(fd);
	}
}
