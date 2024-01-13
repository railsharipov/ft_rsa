#include <ssl/ssl.h>
#include <ssl/error.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <libft/alloc.h>

void	ssl_exit(uint32_t error)
{
	char	buf[ERROR_BUF_SIZE];
	int		rbytes;
	int		fd;

	if (USAGE_ERROR == error)
	{
		if ((fd = open(SSL_INFO_FILE, O_RDONLY)) > 0)
		{
			while ((rbytes = read(fd, buf, ERROR_BUF_SIZE)) > 0)
			{
				write(STDOUT_FILENO, buf, rbytes);
			}
			close(fd);
		}
	}
	else if (MEMORY_ERROR == error)
	{
		ft_printf("%@FATAL: memory error\n");
	}
	ft_free_all();

	exit(0);
}
