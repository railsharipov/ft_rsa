/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/16 18:15:40 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/12 10:43:14 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/base64.h>
#include <libft/htable.h>

static int	__get_task(const char **opt);
static int	__run_task(void);
static int	__get_input(char **input, size_t *insize);
static int	__write_output(const char *enc, size_t encsize);
static int	__lbrk_io(const char *, const t_task *);
static int	__init_io(const char *, const t_task *);
static int	__set_op(const char *, const t_task *);

static t_htbl		*__b64_htable;
static uint32_t		__gflag;
static t_iodes		__in;
static t_iodes		__out;
static int			(*__f_b64)(const char *, size_t, char **, size_t *);

static const t_task	T[] = {
	/*	KEY	PTR	TFLAG	GFLAG	OFLAG	VAL	*/
	{	"-b",	__lbrk_io,	NONE,		NONE,		NONE,				1	},
	{	"-e",	NULL,		NONE,		NONE,		NONE,				0	},
	{	"-d",	__set_op,	B64_D,		NONE,		NONE,				0	},
	{	"-i",	__init_io,	IO_INPUT,	IO_INPUT,	IO_READ|IO_FILE,	1	},
	{	"-o",	__init_io,	IO_OUTPUT,	IO_OUTPUT,	IO_WRITE|IO_FILE,	1	},
	{	NULL,	NULL,		NONE,		NONE,		NONE,				0	}
};

int	comm_base64(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	if (NULL == (__b64_htable = ssl_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	io_init(&__in, IO_READ|IO_STDIN);
	io_init(&__out, IO_WRITE|IO_STDOUT);

	__f_b64 = base64_encode;

	if (SSL_OK == (ret = __get_task(opt)))
		ret = __run_task();

	io_close_multi(&__in, &__out, NULL);
	ssl_task_htable_del(__b64_htable);

	if (SSL_OK != ret)
		return (BASE64_ERROR(UNSPECIFIED_ERROR));
	return (SSL_OK);
}

static int	__get_task(const char **opt)
{
	int		(*f_setup)(const char *, const t_task *);
	t_task	*task;

	// dynamically setup task
	while (NULL != *opt)
	{
		if (NULL == (task = ft_htbl_get(__b64_htable, *opt)))
			return (BASE64_ERROR(INVALID_INPUT_ERROR));

		__gflag |= task->gflag;

		// if option flag is required
		if (task->val)
			if (NULL == * ++opt)
				return (BASE64_ERROR("expected option flag"));

		if (NULL != (f_setup = task->ptr))
			if (SSL_OK != f_setup(*opt, task))
				return (BASE64_ERROR(UNSPECIFIED_ERROR));

		opt++;
	}

	return (SSL_OK);
}

static int	__run_task(void)
{
	char	*input;
	char	*output;
	size_t	insize;
	size_t	outsize;

	if (SSL_OK != __get_input(&input, &insize))
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != __f_b64(input, insize, &output, &outsize))
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != __write_output(output, outsize))
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__get_input(char **input, size_t *insize)
{
	char	buf[IO_BUFSIZE];
	int		rbytes;
	size_t	tbytes;

	*input = NULL;
	*insize = 0;

	if (SSL_FLAG(B64_D, __gflag))
	{
		__in.delim = '\n';
	}
	while ((rbytes = io_read(&__in, buf, IO_BUFSIZE)) > 0)
	{
		SSL_REALLOC(*input, *insize, *insize + rbytes);
		ft_memcpy(*input + *insize, buf, rbytes);
		*insize += rbytes;
	}

	if (rbytes < 0)
	{
		SSL_FREE(*input);
		*insize = 0;
		return (BASE64_ERROR(UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

static int	__write_output(const char *enc, size_t encsize)
{
	if (base64_decode == __f_b64)
		__out.delim = 0;

	if (io_write(&__out, enc, encsize) < 0)
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	if (base64_encode == __f_b64)
		if (io_write(&__out, "\n", 1) < 0)
			return (BASE64_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__set_op(const char *opt, const t_task *task)
{
	(void)opt;

	if (SSL_FLAG(B64_D, task->tflag))
		__f_b64 = base64_decode;

	return (SSL_OK);
}

static int	__lbrk_io(const char *opt, const t_task *task)
{
	if (!ft_str_isdigit(opt))
		return (BASE64_ERROR("invalid option flag"));

	(void)task;
	__out.lwidth = ft_atoi(opt);
	__out.lwidth = MAX(0, __out.lwidth);
	__out.delim = '\n';

	return (SSL_OK);
}

static int	__init_io(const char *opt, const t_task *task)
{
	t_iodes	*iodes;

	iodes = (SSL_FLAG(IO_INPUT, task->tflag)) ? (&__in):(&__out);

	if (SSL_OK != io_init(iodes, task->oflag, opt))
		return (BASE64_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}
