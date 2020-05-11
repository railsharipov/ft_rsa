/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   digest.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/16 10:11:59 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/11 09:35:43 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_ssl.h>
#include <ssl_hash.h>

# undef FUNC_HASH
# define FUNC_HASH(S)	hash_ ## S ## _init ,\
						hash_ ## S ## _update ,\
						hash_ ## S ## _final

static const struct {
	char	*name;
	t_hash	*(*func_init)(void);
	void	(*func_update)(t_hash *, const char *, size_t);
	void	(*func_final)(t_hash *, const char *, size_t);
} FUNC[] = {
	{	"md5",			FUNC_HASH(md5)			},
	{	"sha1",			FUNC_HASH(sha1)			},
	{	"sha256",		FUNC_HASH(sha256)		},
	{	"sha224",		FUNC_HASH(sha224)		},
	{	"sha512",		FUNC_HASH(sha512)		},
	{	"sha384",		FUNC_HASH(sha384)		},
	{	"sha512/256",	FUNC_HASH(sha512_256)	},
	{	"sha512/224",	FUNC_HASH(sha512_224)	},
};

static char		*algo;
static uint32_t	gflag;
static t_hash	*hash;
static t_io		in;

static const t_task	T[] = {
	/*	KEY		PTR		TFLAG		GFLAG		OFLAG			VAL	*/
	{	"-p",	NULL,	HASH_P,		NONE,		IO_READ|IO_STDIN,	0	},
	{	"-s",	NULL,	HASH_S,		NONE,		IO_INPUT|IO_STRING,	1	},
	{	"-q",	NULL,	NONE,		HASH_Q,		NONE,				0	},
	{	"-r",	NULL,	NONE,		HASH_R,		NONE,				0	},
};

static const t_task FILE_TASK =
	{	NULL,	NULL,	IO_FILE,		NONE,		IO_READ|IO_FILE,		0	};

static t_htbl	*hash_htable;
static t_hash	*(*func_hash_init)(void);
static void		(*func_hash_update)(t_hash *, const char *, size_t);
static void		(*func_hash_final)(t_hash *, const char *, size_t);

static int	__init_hash_func_by_name(const char *name)
{
	int	size;
	int	ix;

	size = sizeof(FUNC)/sizeof(FUNC[0]);
	ix = 0;

	while (ix < size)
	{
		if (!ft_strcmp(FUNC[ix].name, name))
		{
			func_hash_init = FUNC[ix].func_init;
			func_hash_update = FUNC[ix].func_update;
			func_hash_final = FUNC[ix].func_final;
			break ;
		}
		ix++;
	}

	if (ix >= size)
		return (SSL_ERR);
	return (SSL_OK);
}

static void	__out_hash(const char *sarg, uint32_t tflag, uint32_t gflag)
{
	char	*hexhash;
	char	*sformat;

	sformat = NULL;
	SSL_ALLOC(hexhash, (2*hash->size)+1);
	ft_binhex(hexhash, hash->hash, hash->size);

	if (!SSL_FLAG(HASH_Q, gflag) && !SSL_FLAG(HASH_P, tflag))
	{
		if (SSL_FLAG(HASH_S, tflag))
			ft_sprintf(&sformat, "\"%s\"", sarg);
		else if (SSL_FLAG(IO_FILE, tflag))
			ft_sprintf(&sformat, "%s", sarg);
		if (!SSL_FLAG(HASH_R, gflag))
			ft_printf("%q (%s) = %s\n", algo, sformat, hexhash);
		else
			ft_printf("%s %s\n", hexhash, sformat);
	}
	else
	{
		ft_printf("%s\n", hexhash);
	}
	SSL_FREE(sformat);
	SSL_FREE(hexhash);
}

static int	__run_task(uint32_t tflag, uint32_t gflag)
{
	char	*buf;
	int		bufsize;
	int		rbytes;

	bufsize = FLOOR(RD_BUF, 128);
	SSL_ALLOC(buf, bufsize);

	hash = func_hash_init();

	while ((rbytes = in.func(&in, buf, bufsize)) == bufsize)
	{
		if (SSL_FLAG(HASH_P, tflag))
			write(1, buf, bufsize);
		func_hash_update(hash, buf, bufsize);
	}

	if (rbytes < 0)
		return (SSL_ERROR(NULL));

	if (SSL_FLAG(HASH_P, tflag))
		write(1, buf, rbytes);

	func_hash_final(hash, buf, rbytes);
	__out_hash(in.input, tflag, gflag);

	return (SSL_OK);
}

static int	__next_task(char **opt)
{
	t_task		*task;

	if (NULL == *opt)
		return (SSL_OK);

	if (NULL == (task = ft_htbl_get(hash_htable, *opt)))
		task = &FILE_TASK;

	gflag |= task->gflag;
	opt += task->val;

	if (NONE != task->tflag)
	{
		if (NULL == *opt)
			return (SSL_ERROR("missing option argument"));
		if (SSL_OK != io_init(&in, *opt, ft_strlen(*opt), task->oflag))
			return (SSL_ERROR("hash i/o error"));
		if (SSL_OK != __run_task(task->tflag, gflag))
			return (SSL_ERROR("hash error"));
	}
	io_close(&in);

	return (__next_task(opt + 1));
}

static int	__default_task(char **opt)
{
	if (SSL_OK != io_init(&in, NULL, 0, (IO_READ | IO_STDIN)))
		return (SSL_ERROR("hash i/o error"));
	if (SSL_OK != __run_task(NONE, HASH_Q))
		return (SSL_ERROR("hash error"));

	return (SSL_OK);
}

int	comm_hash(char **opt, const char *name_comm)
{
	int	ret;

	algo = name_comm;
	SSL_CHECK(NULL != opt);

	if (SSL_OK != __init_hash_func_by_name(name_comm))
		return (SSL_ERROR("hash command error"));

	if (NULL == (hash_htable = util_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (SSL_ERROR("hash command error"));

	if (NULL == *opt)
		ret = __default_task(opt);
	else
		ret = __next_task(opt);

	ft_htbl_del(hash_htable);

	return (ret);
}

# undef FUNC_HASH
