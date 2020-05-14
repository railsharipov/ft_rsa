#include <ft_ssl.h>
#include <ssl_error.h>

void	ssl_exit(uint32_t error)
{
	char	buf[SSL_ERRBUF];
	int		rbytes;
	int		fd;

	if (SSL_ERR_USAGE == error)
	{
		if ((fd = open(SSL_INFO_FILE, O_RDONLY)) > 0)
		{
			while ((rbytes = read(fd, buf, SSL_ERRBUF)) > 0)
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
	else if (SSL_ERR_MEMORY == error)
	{
		ft_printf("%@FATAL: memory error\n");
	}
	ft_free_all();

	exit(0);
}
