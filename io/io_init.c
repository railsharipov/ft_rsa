/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_func.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/13 09:49:33 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:36:39 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>

struct s_io_param {
	uint32_t		flag;
	FUNC_SYSTEM_IO	f1;
	int				arg;
	FUNC_IO			f2;
};

static const struct s_io_param	T[] = {
	/*	FLAG					F1		ARGS					F2	 	*/
	{	IO_READ | IO_STDIN,		NULL,	STDIN_FILENO,			io_fread	},
	{	IO_READ | IO_STDOUT,	NULL,	STDOUT_FILENO,			io_fread	},
	{	IO_READ | IO_FILE,		open,	O_RDONLY,				io_fread	},
	{	IO_WRITE | IO_STDIN,	NULL,	STDIN_FILENO,			io_fwrite	},
	{	IO_WRITE | IO_STDOUT,	NULL,	STDOUT_FILENO,			io_fwrite	},
	{	IO_WRITE | IO_FILE,		open,	O_TRUNC|O_RDWR|O_CREAT,	io_fwrite	},
	{	IO_INPUT | IO_STRING,	NULL,	NONE,					io_sread	},
	{	IO_INPUT,				NULL,	STDIN_FILENO,			io_fread	},
	{	IO_OUTPUT,				NULL,	STDOUT_FILENO,			io_fwrite	},
};

static const size_t	T_SIZE = sizeof(T) / sizeof(struct s_io_param);

static t_htbl	*__init_io_htable(void);
static int		__init_io_fd(int *fd, char *input, FUNC_SYSTEM_IO f, int arg);

int	io_init(t_io *const io, const char *input, int insize, uint32_t flags)
{
	static t_htbl		*io_htable;
	struct s_io_param	*param;
	void				*key;

	if (NULL == io)
		return (IO_ERROR(INVALID_INPUT));

	ft_bzero(io, sizeof(t_io));
	io->input = (char *)input;
	io->insize = insize;

	if (NULL == io_htable)
		io_htable = __init_io_htable();

	key = &flags;

	if (NULL == (param = ft_htbl_get(io_htable, key, sizeof(uint32_t))))
		return (IO_ERROR(INVALID_IO_PARAMETERS));

	if (SSL_OK != __init_io_fd(&io->fd, io->input, param->f1, param->arg))
		return (IO_ERROR(UNSPECIFIED_ERROR));

	io->func = param->f2;

	return (SSL_OK);
}

static t_htbl	*__init_io_htable(void)
{
	t_htbl	*io_htable;
	int		ix;
	void	*key;
	void	*content;

	io_htable = ft_htbl_init(T_SIZE);

	ix = 0;
	while (ix < T_SIZE)
	{
		key = (void *)&(T[ix].flag);
		content = (void *)(T + ix);
		ft_htbl_add(io_htable, content, key, sizeof(uint32_t));
		ix++;
	}
	return (io_htable);
}

static int	__init_io_fd(int *fd, char *input, FUNC_SYSTEM_IO f, int arg)
{
	if (NULL == f)
	{
		*fd = arg;
		return (SSL_OK);
	}

	if (NULL == input)
		return (IO_ERROR(INVALID_IO_PARAMETERS));

	if ((*fd = f(input, arg, 0644)) < 0)
		return (IO_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}
