#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_rsa.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <ssl_pem.h>
#include <ssl_rand.h>
#include <ssl_io.h>

static t_htbl		*__rsa_htable;

static const char	*__frand;
static uint32_t		__gflag;
static t_iodes		__out;
static int			__modsize;
static t_node		*__asn_pkey;
static t_der		*__der_pkey;
static t_pem		*__pem_pkey;

static int	__set_rand(const char *, const t_task *);
static int	__init_io(const char *, const t_task *);

static const t_task	T[] = {
	/*	KEY			PTR			TFLAG	GFLAG	OFLAG				VAL	*/
	{	"-rand",	__set_rand,	NONE,	NONE,	NONE,				1	},
	{	"-o",		__init_io,	NONE,	NONE,	IO_WRITE|IO_FILE,	1	},
	{	NULL,		NULL,		NONE,	NONE,	NONE,				0	}
};

static int	__set_rand(const char *opt, const t_task *task)
{
	__frand = opt;
	return (SSL_OK);
}

static int	__init_io(const char *opt, const t_task *task)
{
	return (io_init(&__out, task->oflag, opt));
}

static int	__set_modsize(const char *opt)
{
	if (!ft_str_isdigit(opt))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	__modsize = ft_atoi(opt);

	return (SSL_OK);
}

static void	__clean(void)
{
	ft_htbl_del(__rsa_htable);
	asn_tree_del(__asn_pkey);
	der_del(__der_pkey);
	pem_del(__pem_pkey);
}

static int	__write_output(void)
{
	ft_printf("%@e is %d (%#x)\n", RSA_EXPPUB, RSA_EXPPUB);

	if (io_write(&__out, __pem_pkey->content, __pem_pkey->size) < 0)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int __run_task(void)
{
	ft_printf("%@Generating RSA private key, %d bit long modulus\n", __modsize);

	if (SSL_OK != rsa_gen_key(&__asn_pkey, __modsize, __frand))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != asn_tree_der_encode(__asn_pkey, &__der_pkey))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != pem_encode(
		(t_ostring *)__der_pkey, &__pem_pkey, "RSA PRIVATE KEY", SSL_FALSE))
			return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != __write_output())
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}

static int	__get_task(const char **opt)
{
	int		(*f_task)(const char *, const t_task *);
	t_task	*task;

	while (NULL != *opt)
	{
		if (NULL == (task = ft_htbl_get(__rsa_htable, *opt)))
		{
			if (SSL_OK != __set_modsize(*opt))
				return (SSL_ERROR(INVALID_OPTION_FLAG));
		}
		else
		{
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
		}
		opt++;
	}

	return (SSL_OK);
}

int	comm_rsa_gen(const char **opt, const char *name_comm)
{
	int	ret;

	if (NULL == opt)
		return (SSL_ERROR(UNSPECIFIED_ERROR));
	if (NULL == (__rsa_htable = util_task_htable(T, sizeof(T)/sizeof(T[0]))))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	if (SSL_OK != io_init(&__out, IO_WRITE_STDOUT))
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	__frand = NULL;
	__modsize = 512;

	if (SSL_OK == (ret = __get_task(opt)))
		ret = __run_task();

	io_close(&__out);
	__clean();

	if (SSL_OK != ret)
		return (SSL_ERROR(UNSPECIFIED_ERROR));

	return (SSL_OK);
}
