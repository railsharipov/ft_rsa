#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_io.h>
#include <ssl_rand.h>
#include <ssl_rsa.h>
#include <ssl_map.h>
#include <ssl_asn.h>
#include <ssl_pem.h>
#include <ssl_der.h>

static char		*TYPE_RSA_PRIVATE_KEY = "RSA PRIVATE KEY";
static char		*TYPE_X509_PUBLIC_KEY = "PUBLIC KEY";

static t_htbl	*__rsa_htable;
static t_io		__in;
static t_io		__out;
static t_io		__inkey;

static uint32_t	__gflag;
static char		*__in_type;
static char		*__in_map;

static int		(*__f_op)(t_ostring *, t_ostring *, t_node *);

static int	__init_io(const char *, const t_task *);
static int	__set_op(const char *, const t_task *);
static int	__set_type(const char *, const t_task *);

static const t_task	T[] = {
	/*	KEY			PTR			TFLAG		GFLAG		OFLAG			VAL	*/
	{	"-in",		__init_io,	IO_INPUT,	NONE,		IO_READ_FILE,	1	},
	{	"-out",		__init_io,	IO_OUTPUT,	NONE,		IO_WRITE_FILE,	1	},
	{	"-inkey",	__init_io,	NONE,		NONE,		IO_READ_FILE,	1	},
	{	"-encrypt",	__set_op,	RSA_ENCRYPT,NONE,		NONE,			0	},
	{	"-decrypt",	__set_op,	RSA_DECRYPT,NONE,		NONE,			0	},
	{	"-hexdump",	NULL,		NONE,		RSA_HEXDUMP,NONE,			0	},
	{	"-pubin",	__set_type,	RSA_PUBIN,	NONE,		NONE,			0	},
	{	NULL,		NULL,		NONE,		NONE,		NONE,			0	}
};

static int	__init_io(const char *opt, const t_task *task)
{
	t_io	*io;

	if (SSL_FLAG(IO_INPUT, task->tflag))
	{
		io = &__in;
	}
	else if (SSL_FLAG(IO_OUTPUT, task->tflag))
	{
		io = &__out;
	}
	else
	{
		io = &__inkey;
	}
	return (io_init(io, opt, ft_strlen(opt), task->oflag));
}

static int	__set_op(const char *opt, const t_task *task)
{
	(void)opt;

	if (SSL_FLAG(RSA_ENCRYPT, task->tflag))
	{
		__f_op = rsa_encrypt;
	}
	else
	{
		__f_op = rsa_decrypt;
	}
	return (SSL_OK);
}

static int	__set_type(const char *opt, const t_task *task)
{
	if (SSL_FLAG(RSA_PUBIN, task->tflag))
	{
		__in_type = TYPE_X509_PUBLIC_KEY;
		__in_map = MAP_X509_PUBLIC_KEY;
	}
	return (SSL_OK);
}

static int	__write_output(char *output, size_t outsize)
{
	if (SSL_FLAG(RSA_HEXDUMP, __gflag))
	{
		util_hexdump(__out.fd, output, outsize);
	}
	else if (__out.func(&__out, output, outsize) < 0)
	{
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__get_input(t_io *io, char **input, size_t *insize)
{
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	size_t	tbytes;

	*input = NULL;
	*insize = 0;

	while ((rbytes = io->func(io, buf, IO_BUFSIZE)) > 0)
	{
		SSL_REALLOC(*input, *insize, (*insize) + rbytes);
		ft_memcpy(*input + *insize, buf, rbytes);
		*insize += rbytes;
	}
	if (rbytes < 0)
	{
		SSL_FREE(*input);
		*insize = 0;
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__decode_key(t_pem *key, t_node **asn_key)
{
	t_der	*der_key;
	int		ret;

	ret = SSL_OK;

	if (SSL_OK != pem_decode(key, __in_type, (t_ostring **)&der_key))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	else if (SSL_OK != asn_tree_der_decode(der_key, __in_map, asn_key))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	der_del(der_key);

	return (ret);
}

static int	__run_task(void)
{
	t_ostring	input;
	t_ostring	output;
	t_pem		key;
	t_node		*asn_key;
	int			ret;

	util_ostrinit((t_ostring *)&input);
	util_ostrinit((t_ostring *)&output);
	util_ostrinit((t_ostring *)&key);
	asn_key = NULL;
	ret = SSL_OK;

	if (NULL == __inkey.input)
	{
		return (SSL_ERROR(EXPECTED_OPTION_FLAG));
	}
	if (SSL_OK != __get_input(&__inkey, &(key.content), &(key.size)))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	else if (SSL_OK != __get_input(&__in, &(input.content), &(input.size)))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	else if (SSL_OK != __decode_key(&key, &asn_key))
	{
		ret = UNSPECIFIED_ERROR;
	}
	else if (SSL_OK != __f_op(&input, &output, asn_key))
	{
		ret = UNSPECIFIED_ERROR;
	}
	else if (SSL_OK != __write_output(output.content, output.size))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	asn_tree_del(asn_key);
	SSL_FREE(input.content);
	SSL_FREE(output.content);
	SSL_FREE(key.content);

	return (ret);
}

static int	__get_task(const char **opt)
{
	int		(*f_task)(const char *, const t_task *);
	t_task	*task;

	while (NULL != *opt)
	{
		if (NULL == (task = ft_htbl_get(__rsa_htable, *opt)))
		{
			return (SSL_ERROR(INVALID_OPTION_FLAG));
		}
		__gflag |= task->gflag;
		opt += task->val;

		if (NULL == *opt)
		{
			return (SSL_ERROR(EXPECTED_OPTION_FLAG));
		}
		else if (NULL != (f_task = task->ptr))
		{
			if (SSL_OK != f_task(*opt, task))
				return (SSL_ERROR(UNSPECIFIED_ERROR));
		}
		opt++;
	}

	return (SSL_OK);
}

int	comm_rsa_utl(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	if (NULL == (__rsa_htable = util_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__in, NULL, 0, IO_READ_STDIN))
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	if (SSL_OK != io_init(&__out, NULL, 0, IO_WRITE_STDOUT))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	__f_op = rsa_encrypt;
	__in_type = TYPE_RSA_PRIVATE_KEY;
	__in_map = MAP_RSA_PRIVATE_KEY;

	if (SSL_OK == (ret = __get_task(opt)))
		ret = __run_task();

	io_close_multi(&__in, &__out, &__inkey, NULL);

	if (SSL_OK != ret)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}
