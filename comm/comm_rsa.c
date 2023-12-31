#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rand.h>
#include <ssl_map.h>
#include <ssl_asn.h>
#include <ssl_pem.h>
#include <ssl_der.h>
#include <ssl_rsa.h>
#include <ssl_des.h>

static char			*TYPE_RSA_PRIVATE_KEY = "RSA PRIVATE KEY";
static char			*TYPE_X509_PUBLIC_KEY = "PUBLIC KEY";

static t_htbl		*__rsa_htable;
static t_iodes		__in;
static t_iodes		__out;
static t_pem		__inkey;

static uint32_t		__gflag;
static uint32_t		__inform;
static uint32_t		__outform;
static char			*__in_type;
static char			*__out_type;
static char			*__in_map;
static char			*__out_map;
static char			*__passin;
static char			*__passout;

static int	__setup_task(const char **);
static int	__run_task(void);
static int	__get_input(char **, size_t *);
static int	__write_output(char *, size_t );
static void	__key_info(t_node *);
static int	__key_type(t_node **);
static int	__decode_key(t_node **);
static int	__encode_key(t_node *, t_ostring **);
static int	__init_io(const char *, const t_task *);
static int	__set_type(const char *, const t_task *);
static int	__set_form(const char *, const t_task *);
static int	__get_pass(const char *, const t_task *);

static const t_task	  T[] = {
	/*	KEY			PTR			TFLAG		GFLAG		OFLAG			VAL	*/
	{	"-in",		__init_io,	IO_INPUT,	NONE,		IO_READ_FILE,	1	},
	{	"-out",		__init_io,	IO_OUTPUT,	NONE,		IO_WRITE_FILE,	1	},
	{	"-inform",	__set_form,	RSA_INFORM,	NONE,		NONE,			1	},
	{	"-outform",	__set_form,	RSA_OUTFORM,NONE,		NONE,			1	},
	{	"-passin",	__get_pass,	RSA_PASSIN,	NONE,		NONE,			1	},
	{	"-passout",	__get_pass,	RSA_PASSOUT,NONE,		NONE,			1	},
	{	"-pubin",	__set_type,	RSA_PUBIN,	RSA_PUBIN,	NONE,			0	},
	{	"-pubout",	__set_type,	RSA_PUBOUT,	RSA_PUBOUT,	NONE,			0	},
	{	"-check",	NULL,		NONE,		RSA_CHECK,	NONE,			0	},
	{	"-des",		NULL,		NONE,		RSA_ENCRYPT,NONE,			0	},
	{	"-text",	NULL,		NONE,		RSA_TEXT,	NONE,			0	},
	{	"-noout",	NULL,		NONE,		RSA_NOOUT,	NONE,			0	},
	{	"-modulus",	NULL,		NONE,		RSA_MODULUS,NONE,			0	},
	{	NULL,		NULL,		NONE,		NONE,		NONE,			0	}
};

int	comm_rsa(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (NULL == (__rsa_htable = util_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__in, IO_READ_STDIN))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__out, IO_WRITE_STDOUT))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	__inform = __outform = RSA_PEM;
	__in_type = __out_type = TYPE_RSA_PRIVATE_KEY;
	__in_map = __out_map = MAP_RSA_PRIVATE_KEY;

	if (SSL_OK == (ret = __setup_task(opt)))
		ret = __run_task();

	SSL_FREE(__inkey.content);
	io_close_multi(&__in, &__out, NULL);
	util_task_htable_del(__rsa_htable);

	if (SSL_OK != ret)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

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
			return (SSL_ERROR(INVALID_INPUT));

		__gflag |= task->gflag;

		// if option flag is required
		if (task->val)
			if (NULL == * ++opt)
				return (SSL_ERROR(EXPECTED_OPTION_FLAG));

		if (NULL != (f_setup = task->ptr))
			if (SSL_OK != f_setup(*opt, task))
				return (SSL_ERROR(UNSPECIFIED_ERROR));

		opt++;
	}

	return (SSL_OK);
}

static int	__run_task(void)
{
	t_ostring	*outkey;
	t_node		*asn_key;
	int			ret;

	ret = SSL_OK;
	outkey = NULL;

	if (SSL_OK != __get_input(&(__inkey.content), &(__inkey.size)))
	{
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_OK != __decode_key(&asn_key))
	{
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (SSL_FLAG(RSA_CHECK, __gflag))
	{
		if (SSL_OK != rsa_check(asn_key))
		{
			asn_tree_del(asn_key);
			return (SSL_ERROR(UNSPECIFIED_ERROR));
		}
		ft_putstr("RSA key ok\n");
	}
	__key_info(asn_key);

	if (SSL_OK != __key_type(&asn_key))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	else if (SSL_OK != __encode_key(asn_key, &outkey))
	{
		ret = SSL_ERROR(UNSPECIFIED_ERROR);
	}
	asn_tree_del(asn_key);

	if (SSL_OK == ret)
	{
		ret = __write_output(outkey->content, outkey->size);
	}
	util_ostr_del(outkey);

	return (ret);
}

static int	__get_input(char **input, size_t *insize)
{
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	size_t	tbytes;

	*input = NULL;
	*insize = 0;

	while ((rbytes = io_read(&__in, buf, IO_BUFSIZE)) > 0)
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

static int	__write_output(char *output, size_t outsize)
{
	if (!SSL_FLAG(RSA_NOOUT, __gflag))
	{
		ft_putstr("writing RSA key\n");

		if (io_write(&__out, output, outsize) < 0)
			return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	return (SSL_OK);
}

static int	__init_io(const char *opt, const t_task *task)
{
	t_iodes	*iodes;

	iodes = (SSL_FLAG(IO_INPUT, task->tflag)) ? (&__in):(&__out);

	return (io_init(iodes, task->oflag, opt));
}

static int	__set_type(const char *opt, const t_task *task)
{
	if (SSL_FLAG(RSA_PUBIN, task->tflag))
	{
		__in_type = TYPE_X509_PUBLIC_KEY;
		__in_map = MAP_X509_PUBLIC_KEY;
	}
	else
	{
		__out_type = TYPE_X509_PUBLIC_KEY;
		__out_map = MAP_X509_PUBLIC_KEY;
	}
	return (SSL_OK);
}

static int	__set_form(const char *opt, const t_task *task)
{
	uint32_t	*form;

	form = (SSL_FLAG(RSA_INFORM, task->tflag)) ? (&__inform) : (&__outform);

	if (!ft_strcmp(opt, "PEM"))
		*form = RSA_PEM;
	else if (!ft_strcmp(opt, "DER"))
		*form = RSA_DER;
	else
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__get_pass(const char *opt, const t_task *task)
{
	if (SSL_FLAG(RSA_PASSIN, task->tflag))
		__passin = (char *)opt;
	else
		__passout = (char *)opt;

	return (SSL_OK);
}

static void	__key_info(t_node *asn_key)
{
	t_htbl	*key_items;
	t_iasn	*asn_item;
	char	*snum;

	if (SSL_FLAG(RSA_TEXT, __gflag))
	{
		asn_print(asn_key);
	}
	if (SSL_FLAG(RSA_MODULUS, __gflag))
	{
		asn_item = asn_tree_get(asn_key, "modulus");

		if (NULL != asn_item)
		{
			ft_putstr("Modulus=");
			bnum_print(NULL, asn_item->content);
		}
	}
}

static int	__key_type(t_node **asn_key)
{
	t_node	*asn_pubkey;

	if (SSL_FLAG(RSA_PUBIN, __gflag))
	{
		__out_type = TYPE_X509_PUBLIC_KEY;
		__out_map = MAP_X509_PUBLIC_KEY;
	}
	else if (SSL_FLAG(RSA_PUBOUT, __gflag))
	{
		asn_pubkey = asn_tree(MAP_X509_PUBLIC_KEY);

		if (SSL_OK != asn_transform(*asn_key, asn_pubkey))
		{
			return (SSL_ERROR(UNSPECIFIED_ERROR));
		}
		asn_tree_del(*asn_key);
		*asn_key = asn_pubkey;

		__out_type = TYPE_X509_PUBLIC_KEY;
		__out_map = MAP_X509_PUBLIC_KEY;
	}
	return (SSL_OK);
}

static int	__decode_key(t_node **asn_key)
{
	t_der	*der_key;
	int		ret;

	ret = SSL_OK;

	if (RSA_PEM == __inform)
	{
		ssl_setpass(__passin);
		ret = pem_decode(&__inkey, __in_type, (t_ostring **)&der_key);
		ssl_unsetpass();
	}
	else
	{
		der_key = (t_der *)util_ostr_dup((t_ostring *)&__inkey);
	}
	if (SSL_OK == ret)
	{
		ret = asn_tree_der_decode(der_key, __in_map, asn_key);
	}
	der_del(der_key);

	return (ret);
}

static int	__encode_key(t_node *asn_key, t_ostring **outkey)
{
	t_der	*der_key;
	int		ret;

	ret = SSL_OK;

	if (SSL_OK != asn_tree_der_encode(asn_key, &der_key))
	{
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	}
	if (RSA_PEM == __outform)
	{
		ssl_setpass(__passout);
		ret = pem_encode(
			(t_ostring *)der_key, (t_pem **)outkey,
			__out_type, SSL_FLAG(RSA_ENCRYPT, __gflag));
		ssl_unsetpass();
	}
	else
	{
		SSL_ALLOC(*outkey, sizeof(t_ostring));
		*outkey = util_ostr_dup((t_ostring *)der_key);
	}
	der_del(der_key);

	return (ret);
}
