#include <ssl/ssl.h>
#include <util/io.h>
#include <ssl/rand.h>
#include <ssl/rsa.h>
#include <ssl/map.h>
#include <ssl/asn.h>
#include <ssl/pem.h>
#include <ssl/der.h>
#include <libft/node.h>
#include <libft/htable.h>
#include <libft/bytes.h>

static char		*TYPE_RSA_PRIVATE_KEY = "PRIVATE KEY";
static char		*TYPE_X509_PUBLIC_KEY = "PUBLIC KEY";

static t_htbl	*__rsa_htable;
static t_iodes	__in;
static t_iodes	__out;
static t_iodes	__inkey;

static uint32_t	__gflag;
static char		*__in_type;
static char		*__in_map;

static int		(*__f_op)(t_ostring *, t_ostring *, t_node *);

static int	__setup_task(const char **);
static int	__run_task(void);
static int	__get_input(t_iodes *, char **, size_t *);
static int	__write_output(char *, size_t);
static int	__decode_key(t_pem *, t_node **);
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

int	comm_rsa_utl(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	if (NULL == (__rsa_htable = ssl_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__in, IO_READ_STDIN))
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__out, IO_WRITE_STDOUT))
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	__f_op = rsa_encrypt;
	__in_type = TYPE_RSA_PRIVATE_KEY;
	__in_map = MAP_RSA_PRIVATE_KEY;

	if (SSL_OK == (ret = __setup_task(opt)))
		ret = __run_task();

	io_close_multi(&__in, &__out, &__inkey, NULL);
	ssl_task_htable_del(__rsa_htable);

	if (SSL_OK != ret)
		return (RSA_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__setup_task(const char **opt)
{
	int		(*f_setup)(const char *, const t_task *);
	t_task	*task;

	// dynamically setup task
	while (NULL != *opt)
	{
		if (NULL == (task = ft_htbl_get(__rsa_htable, *opt)))
			return (RSA_ERROR(INVALID_INPUT_ERROR));

		__gflag |= task->gflag;

		// if option flag is required
		if (task->val)
			if (NULL == * ++opt)
				return (RSA_ERROR("expected option flag"));

		if (NULL != (f_setup = task->ptr))
			if (SSL_OK != f_setup(*opt, task))
				return (RSA_ERROR(UNSPECIFIED_ERROR));

		opt++;
	}

	if (!io_is_initialized(&__inkey))
		return (RSA_ERROR(INVALID_INPUT_ERROR));

	return (SSL_OK);
}

static int	__run_task(void)
{
	t_ostring	input;
	t_ostring	output;
	t_pem		key;
	t_node		*asn_key;
	int			ret;

	ft_bzero(&input, sizeof(t_ostring));
	ft_bzero(&output, sizeof(t_ostring));
	ft_bzero(&key, sizeof(t_pem));
	asn_key = NULL;
	ret = SSL_OK;

	if (SSL_OK != __get_input(&__inkey, &(key.content), &(key.size)))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	else if (SSL_OK != __get_input(&__in, &(input.content), &(input.size)))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	else if (SSL_OK != __decode_key(&key, &asn_key))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	else if (SSL_OK != __f_op(&input, &output, asn_key))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	else if (SSL_OK != __write_output(output.content, output.size))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	asn_tree_del(asn_key);
	SSL_FREE(input.content);
	SSL_FREE(output.content);
	SSL_FREE(key.content);

	return (ret);
}

static int	__get_input(t_iodes *iodes, char **input, size_t *insize)
{
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	size_t	tbytes;

	*input = NULL;
	*insize = 0;

	while ((rbytes = io_read(iodes, buf, IO_BUFSIZE)) > 0)
	{
		SSL_REALLOC(*input, *insize, (*insize) + rbytes);
		ft_memcpy(*input + *insize, buf, rbytes);
		*insize += rbytes;
	}
	if (rbytes < 0)
	{
		SSL_FREE(*input);
		*insize = 0;
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__write_output(char *output, size_t outsize)
{
	if (SSL_FLAG(RSA_HEXDUMP, __gflag))
	{
		ft_bytes_write_hex(__out.fd, output, outsize);
	}
	else if (io_write(&__out, output, outsize) < 0)
	{
		return (RSA_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__decode_key(t_pem *key, t_node **asn_key)
{
	t_der	*der_key;
	int		ret;

	ret = SSL_OK;

	if (SSL_OK != pem_decode(key, __in_type, (t_ostring **)&der_key))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	else if (SSL_OK != asn_tree_der_decode(der_key, __in_map, asn_key))
		ret = RSA_ERROR(UNSPECIFIED_ERROR);

	der_del(der_key);

	return (ret);
}

static int	__init_io(const char *opt, const t_task *task)
{
	t_iodes	*iodes;

	if (SSL_FLAG(IO_INPUT, task->tflag))
		iodes = &__in;
	else if (SSL_FLAG(IO_OUTPUT, task->tflag))
		iodes = &__out;
	else
		iodes = &__inkey;

	return (io_init(iodes, task->oflag, opt));
}

static int	__set_op(const char *opt, const t_task *task)
{
	(void)opt;

	if (SSL_FLAG(RSA_ENCRYPT, task->tflag))
		__f_op = rsa_encrypt;
	else
		__f_op = rsa_decrypt;

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
