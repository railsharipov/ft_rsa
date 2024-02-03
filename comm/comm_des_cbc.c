#include <ssl/ssl.h>
#include <util/io.h>
#include <ssl/des.h>
#include <ssl/base64.h>
#include <libft/htable.h>
#include <libft/bytes.h>

static char	*__keyhex;
static char	*__salthex;
static char	*__vecthex;

static t_des	*__des;
static t_htbl	*__des_htable;
static uint32_t	__gflag;
static t_iodes	__in;
static t_iodes	__out;
static int		(*__f_op)(void);

static const char	*__pass;

static int	__setup_task(const char **);
static int	__run_task(void);
static int	__get_vector(const char *, const t_task *);
static int	__get_pass(const char *, const t_task *);
static int	__init_io(const char *, const t_task *);
static int	__set_op(const char *, const t_task *);
static int	__get_input(char **, size_t *);
static int	__write_output(const char *, size_t);
static void	__dump_vectors(void);
static int	__enc(t_ostring *, t_ostring *);
static int	__enc_b64(t_ostring *, t_ostring *);
static int	__dec(t_ostring *, t_ostring *);
static int	__dec_b64(t_ostring *, t_ostring *);

static const t_task	T[] = {
	/*	KEY		PTR				TFLAG		GFLAG	OFLAG			VAL	*/
	{	"-k", 	__get_vector,	DES_K,		DES_K,	NONE,				1	},
	{	"-s", 	__get_vector,	DES_S,		DES_S,	NONE,				1	},
	{	"-v", 	__get_vector,	DES_V,		DES_V,	NONE,				1	},
	{	"-p", 	__get_pass,		DES_P,		DES_P,	NONE,				1	},
	{	"-i", 	__init_io,		IO_INPUT,	NONE,	IO_READ|IO_FILE,	1	},
	{	"-o", 	__init_io,		IO_OUTPUT,	NONE,	IO_WRITE|IO_FILE,	1	},
	{	"-e",	__set_op,		DES_E,		NONE,	NONE,				0	},
	{	"-d",	__set_op,		DES_D,		NONE,	NONE,				0	},
	{	"-a",	NULL,			NONE,		DES_A,	NONE,				0	},
	{	"-n",	NULL,			NONE,		DES_N,	NONE,				0	},
	{	NULL,	NULL,			NONE,		NONE,	NONE,				0	}
};

int	comm_des_cbc(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (NULL == (__des_htable = ssl_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__in, IO_READ|IO_STDIN))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__out, IO_WRITE|IO_STDOUT))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	// default des mode is encrypt
	__gflag = DES_E;

	if (SSL_OK == (ret = __setup_task(opt)))
		ret = __run_task();

	io_close_multi(&__in, &__out, NULL);
	ssl_task_htable_del(__des_htable);

	if (SSL_OK != ret)
		return (DES_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__setup_task(const char **opt)
{
	int		(*f_setup)(const char *, const t_task *);
	t_task	*task;

	// dynamically setup task
	while (NULL != *opt)
	{
		if (NULL == (task = ft_htbl_get(__des_htable, *opt)))
			return (DES_ERROR(INVALID_INPUT_ERROR));

		__gflag |= task->gflag;

		// if option flag is required
		if (task->val)
			if (NULL == * ++opt)
				return (DES_ERROR("expected option flag"));

		if (NULL != (f_setup = task->ptr))
			if (SSL_OK != f_setup(*opt, task))
				return (DES_ERROR(UNSPECIFIED_ERROR));

		opt++;
	}

	return (SSL_OK);
}

static int __run_task(void)
{
	int			(*f_op)(t_ostring *, t_ostring *);
	int			ret;
	t_ostring	input;
	t_ostring	output;

	ret = SSL_OK;
	__des = des_hexinit(__keyhex, __salthex, __vecthex);

	if (SSL_OK != __get_input(&input.content, &input.size))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_FLAG(DES_D, __gflag))
		f_op = (SSL_FLAG(DES_A, __gflag)) ? (__dec_b64) : (__dec);
	else
		f_op = (SSL_FLAG(DES_A, __gflag)) ? (__enc_b64) : (__enc);

	if ((NULL != __pass) && (SSL_OK != ssl_setpass(__pass)))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK == (ret = f_op(&input, &output)))
		ret = __write_output((char *)output.content, output.size);

	ssl_unsetpass();

	SSL_FREE(input.content);
	SSL_FREE(output.content);

	return (ret);
}

static int	__init_io(const char *opt, const t_task *task)
{
	t_iodes	*iodes;

	iodes = (SSL_FLAG(IO_INPUT, task->tflag)) ? (&__in):(&__out);

	return (io_init(iodes, task->oflag, opt));
}

static int	__get_vector(const char *opt, const t_task *task)
{
	if (!ft_str_ishex(opt))
		return (DES_ERROR(INVALID_INPUT_ERROR));

	if (DES_K == task->tflag)
		__keyhex = (char *)opt;
	else if (DES_S == task->tflag)
		__salthex = (char *)opt;
	else if (DES_V == task->tflag)
		__vecthex = (char *)opt;
	else
		return (DES_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__get_pass(const char *opt, const t_task *task)
{
	(void)task;
	__pass = opt;

	return (SSL_OK);
}

static int __set_op(const char *opt, const t_task *task)
{
	uint32_t	remove_flag;

	if (DES_D == task->tflag)
		remove_flag = DES_E;
	else
		remove_flag = DES_D;

	// encrypt and decrypt flags are mutually exclusive
	__gflag &= ~remove_flag;
	__gflag |= task->tflag;

	return (SSL_OK);
}

static int	__get_input(char **input, size_t *insize)
{
	char	buf[IO_BUFSIZE];
	ssize_t	rbytes;
	size_t	tbytes;

	*input = NULL;
	*insize = 0;

	if (SSL_FLAG(DES_A | DES_D, __gflag))
	{
		__in.delim = '\n';
	}
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
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

static int	__write_output(const char *output, size_t outsize)
{
	if (SSL_FLAG(DES_N, __gflag))
		__dump_vectors();

	if (SSL_FLAG(DES_A | DES_E, __gflag))
		__out.delim = '\n';

	if (io_write(&__out, output, outsize) < 0)
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_FLAG(DES_A | DES_E, __gflag))
		if (io_write(&__out, "\n", 1) < 0)
			return (DES_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static void	__dump_vectors(void)
{
	ft_printf("salt=");
	ft_bytes_dump_hex(__des->salt, 8, 0, 0);
	ft_printf("key=");
	ft_bytes_dump_hex(__des->key, 8, 0, 0);
	ft_printf("iv=");
	ft_bytes_dump_hex(__des->vect, 8, 0, 0);
}

static int	__enc(t_ostring *mes, t_ostring *ciph)
{
	return (des_cbc_encrypt(__des, mes, ciph));
}

static int	__enc_b64(t_ostring *mes, t_ostring *ciph)
{
	t_ostring	b64;
	int			ret;

	ret = SSL_OK;

	if (SSL_OK != des_cbc_encrypt(__des, mes, ciph))
		return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != base64_encode(
		(char *)(ciph->content), ciph->size, &b64.content, &b64.size))
			ret = (DES_ERROR(UNSPECIFIED_ERROR));

	SSL_FREE(ciph->content);
	ciph->content = b64.content;
	ciph->size = b64.size;

	return (ret);
}

static int	__dec(t_ostring *ciph, t_ostring *mes)
{
	return (des_cbc_decrypt(__des, ciph, mes));
}

static int	__dec_b64(t_ostring *b64, t_ostring *mes)
{
	t_ostring	cipher;

	if (SSL_OK != base64_decode(
		(char *)(b64->content), b64->size, &cipher.content, &cipher.size))
			return (DES_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != des_cbc_decrypt(__des, &cipher, mes))
	{
		SSL_FREE(cipher.content);
		return (DES_ERROR(UNSPECIFIED_ERROR));
	}
	SSL_FREE(cipher.content);

	return (SSL_OK);
}
