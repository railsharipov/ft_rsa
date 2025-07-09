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
	/*	FLAG				FD				*/
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
	IO_LOG(TRACE, "io_fopen: entering function with flags=0x%x, filename=%s", flags, filename ? filename : "NULL");
	
	t_io_param	*param;
	char		*input;
	int 		insize;

	if (NULL == iodes) {
		IO_LOG(TRACE, "io_fopen: NULL iodes parameter, returning -1");
		return (-1);
	}

	IO_LOG(TRACE, "io_fopen: initializing iodes structure");
	ft_bzero(iodes, sizeof(t_iodes));

	if (NULL == (param = __get_param(flags))) {
		IO_LOG(TRACE, "io_fopen: invalid flags 0x%x, returning -1", flags);
		return (-1);
	}

	IO_LOG(TRACE, "io_fopen: setting fd=%d from param", param->fd);
	iodes->fd = param->fd;

	if (FLAG(IO_FILE, flags)) {
		if (NULL == filename) {
			IO_LOG(TRACE, "io_fopen: IO_FILE flag set but filename is NULL, returning -1");
			return (-1);
		}

		IO_LOG(TRACE, "io_fopen: opening file '%s' with flags 0x%x", filename, flags);
		if (FLAG(IO_READ, flags)) {
			iodes->fd = open(filename, O_RDONLY, 0644);
			IO_LOG(TRACE, "io_fopen: opened file for reading, fd=%d", iodes->fd);
		}
		else if (FLAG(IO_WRITE, flags)) {
			iodes->fd = open(filename, O_TRUNC|O_RDWR|O_CREAT, 0644);
			IO_LOG(TRACE, "io_fopen: opened file for writing, fd=%d", iodes->fd);
		}
		else {
			IO_LOG(TRACE, "io_fopen: invalid flags combination, returning -1");
			return (-1);
		}
	}

	if (iodes->fd < 0) {
		IO_LOG(TRACE, "io_fopen: file open failed, fd=%d, returning -1", iodes->fd);
		return (-1);
	}

	IO_LOG(TRACE, "io_fopen: setting mode to IO_MODE_FILDES");
	iodes->mode = IO_MODE_FILDES;

	IO_LOG(TRACE, "io_fopen: file open successful, fd=%d, returning 0", iodes->fd);
	return (0);
}

static t_io_param	*__get_param(uint32_t flags)
{
	IO_LOG(TRACE, "__get_param: searching for flags 0x%x", flags);
	
	int	ix;

	ix = 0;
	while (ix < TSIZE) {
		if (T[ix].flag == flags) {
			IO_LOG(TRACE, "__get_param: found matching param at index %d", ix);
			return ((t_io_param *)T+ix);
		}
		ix++;
	}

	IO_LOG(TRACE, "__get_param: no matching param found for flags 0x%x", flags);
	return (NULL);
}
