#include <unistd.h>
#include <sys/fcntl.h>
#include <util/io.h>
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

int	io_open(t_iodes *iodes, uint32_t flags, const char *filename)
{
	t_io_param	*param;
	char		*input;
	int 		insize;

	if (NULL == iodes)
		return (-1);

	ft_bzero(iodes, sizeof(t_iodes));

	if (NULL == (param = __get_param(flags)))
		return (-1);

	iodes->fd = param->fd;

	if (FLAG(IO_FILE, flags))
	{
		if (NULL == filename)
			return (-1);

		if (FLAG(IO_READ, flags))
			iodes->fd = open(filename, O_RDONLY, 0644);
		else if (FLAG(IO_WRITE, flags))
			iodes->fd = open(filename, O_TRUNC|O_RDWR|O_CREAT, 0644);
		else
			return (-1);
	}

	if (iodes->fd < 0)
		return (-1);

	iodes->mode = IO_MODE_FILDES;

	return (0);
}

static t_io_param	*__get_param(uint32_t flags)
{
	int	ix;

	ix = 0;
	while (ix < TSIZE)
	{
		if (T[ix].flag == flags)
			return ((t_io_param *)T+ix);
		ix++;
	}

	return (NULL);
}
