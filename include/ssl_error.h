#ifndef SSL_ERROR_H
# define SSL_ERROR_H

# include <ft_printf.h>
# include <sys/errno.h>

# define SSL_ERRBUF		1024
# define SSL_INFO_FILE	"./docs/usage.txt"

extern int global_ssl_error_set;

# define SSL_ERROR(MES) ({ \
	ft_printf("%s, %s:%d ---> ", __func__, __FILE__, __LINE__); \
	if ((!global_ssl_error_set) && (errno)) \
	{ \
		perror(MES); \
		global_ssl_error_set = 1; \
	} \
	else if (NULL != MES) \
		ft_printf("%@%s\n", MES); \
	else \
		ft_printf("%@not specified\n"); \
	SSL_ERR; \
})

#endif
