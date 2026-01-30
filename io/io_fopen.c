#include <unistd.h>
#include <sys/fcntl.h>
#include <io.h>
#include <libft/std.h>

typedef struct	s_io_param
{
	uint32_t	flag;
	int			fd;
}				t_io_param;

static const t_io_param	T[] = {
	/*	flags				fd				*/
	{	IO_READ|IO_STDIN,	STDIN_FILENO	},
	{	IO_READ|IO_STDOUT,	STDOUT_FILENO	},
	{	IO_READ|IO_FILE,	-1				},
	{	IO_WRITE|IO_STDIN,	STDIN_FILENO	},
	{	IO_WRITE|IO_STDOUT,	STDOUT_FILENO	},
	{	IO_WRITE|IO_FILE,	-1				},
};

static const size_t	TSIZE = sizeof(T)/sizeof(t_io_param);

static t_io_param	*__get_param(uint32_t);

int	io_fopen(t_iodes *iodes, uint32_t flags, const char *filename)
{
	SSL_LOG(TRACE, "io fopen with flags=0x%x, filename=%s", flags, filename ? filename : "");
	
	t_io_param	*param;

	if (NULL == iodes) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (-1);
	}

	ft_bzero(iodes, sizeof(t_iodes));

	if (NULL == (param = __get_param(flags))) {
		SSL_LOG(ERROR, "invalid flags %#x", flags);
		return (-1);
	}

	iodes->fd = param->fd;

	if (SSL_FLAG(IO_FILE, flags)) {
		if (NULL == filename) {
			SSL_LOG(ERROR, "filename is not specified");
			return (-1);
		}

		SSL_LOG(TRACE, "opening file '%s' with flags 0x%x", filename, flags);
		if (SSL_FLAG(IO_READ, flags)) {
			iodes->fd = open(filename, O_RDONLY, 0644);
			SSL_LOG(TRACE, "opened file for reading, fd=%d", iodes->fd);
		}
		else if (SSL_FLAG(IO_WRITE, flags)) {
			iodes->fd = open(filename, O_TRUNC|O_RDWR|O_CREAT, 0644);
			SSL_LOG(TRACE, "opened file for writing, fd=%d", iodes->fd);
		}
		else {
			SSL_LOG(ERROR, "invalid flags combination");
			return (-1);
		}
	}

	if (iodes->fd < 0) {
		SSL_LOG(ERROR, "file open failed, fd=%d", iodes->fd);
		return (-1);
	}

	iodes->mode = IO_MODE_FILDES;

	return (0);
}
static t_io_param	*__get_param(uint32_t flags)
{
	int	ix;

	ix = 0;
	while (ix < TSIZE) {
		if (T[ix].flag == flags) {
			return ((t_io_param *)T+ix);
		}
		ix++;
	}

	return (NULL);
}

