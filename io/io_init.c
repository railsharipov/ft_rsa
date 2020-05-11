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
#include <ssl_io.h>

struct s_io_param {
	uint32_t	flag;
	int			(*f1)(const char *, int, ...);
	int			arg;
	FUNC_IO		f2;
};

static const struct s_io_param	T[] = {
	/*	FLAG				F1		ARGS					F2		 	*/
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

static t_htbl	*__init_io_htable()
{
	t_htbl	*io_htable;
	int		size;
	int		ix;

	size = sizeof(T)/sizeof(T[0]);
	io_htable = ft_htbl_init(size);

	ix = 0;
	while (ix < size)
	{
		ft_htbl_raw_add(io_htable, &(T[ix].flag), sizeof(uint32_t), T+ix);
		ix++;
	}
	return (io_htable);
}

int	io_init(t_io *const io, char *input, int insize, uint32_t flags)
{
	static t_htbl		*io_htable;
	struct s_io_param	*param;
	int					ix;
	int 				ret;

	ft_bzero(io, sizeof(t_io));
	io->input = input;
	io->insize = insize;

	ret = SSL_OK;

	if (NULL == io_htable)
		io_htable = __init_io_htable();

	param = ft_htbl_raw_get(io_htable, &flags, sizeof(uint32_t));

	if (NULL == param)
		ret = SSL_ERROR("invalid i/o flag specified");
	else if (NULL != param->f1)
	{
		if (NULL == io->input)
			ret = SSL_ERROR("no i/o flag option specified");
		else if ((io->fd = param->f1(io->input, param->arg, 0644)) < 0)
			ret = SSL_ERROR(NULL);
	}
	else
	{
		io->fd = param->arg;
	}

	if (SSL_OK == ret)
		io->func = param->f2;
	// else
	// 	io_print_stats(io, COLOR_RED "IO ERROR" COLOR_RESET);

	return (ret);
}
