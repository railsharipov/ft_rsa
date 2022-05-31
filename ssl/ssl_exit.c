#include <ft_ssl.h>
#include <ssl_error.h>

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
		else
		{
			ft_printf("@invalid user input, refer to readme at "
				"https://github.com/nugliar/ft_rsa");
		}
	}
	else if (MEMORY_ERROR == error)
	{
		ft_printf("%@FATAL: memory error\n");
	}
	ft_free_all();

	exit(0);
}
