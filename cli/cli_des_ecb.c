#include <common.h>
// #include <io.h>
// #include <des.h>
// #include <base64.h>
// #include <cli.h>
// #include <rand.h>
// #include <libft/htable.h>
// #include <libft/bytes.h>

// static unsigned char	__key[8];
// static unsigned char	__salt[8];

// static char	*__keyhex;
// static char	*__salthex;

// static t_htbl	*__des_htable;
// static uint32_t	__gflag;
// static t_iodes	__in;
// static t_iodes	__out;

// static const char	*__pass;

// static int	__setup_task(const char **);
// static int	__run_task(void);
// static int	__get_vector(const char *, const t_task *);
// static int	__get_pass(const char *, const t_task *);
// static int	__init_io(const char *, const t_task *);
// static int	__set_op(const char *, const t_task *);
// static int	__get_input(char **, size_t *);
// static int	__write_output(const char *, size_t);
// static void	__dump_vectors(void);
// static int	__enc(t_ostring *, t_ostring *);
// static int	__enc_b64(t_ostring *, t_ostring *);
// static int	__dec(t_ostring *, t_ostring *);
// static int	__dec_b64(t_ostring *, t_ostring *);

// static const t_task	T[] = {
// 	/*	KEY		PTR				TFLAG		GFLAG	OFLAG				VAL	*/
// 	{	"-k", 	__get_vector,	DES_K,		DES_K,	NONE,				1	},
// 	{	"-s", 	__get_vector,	DES_S,		DES_S,	NONE,				1	},
// 	{	"-p", 	__get_pass,		DES_P,		DES_P,	NONE,				1	},
// 	{	"-i", 	__init_io,		IO_INPUT,	NONE,	IO_READ|IO_FILE,	1	},
// 	{	"-o", 	__init_io,		IO_OUTPUT,	NONE,	IO_WRITE|IO_FILE,	1	},
// 	{	"-e",	__set_op,		DES_E,		NONE,	NONE,				0	},
// 	{	"-d",	__set_op,		DES_D,		NONE,	NONE,				0	},
// 	{	"-a",	NULL,			NONE,		DES_A,	NONE,				0	},
// 	{	"-n",	NULL,			NONE,		DES_N,	NONE,				0	},
// 	{	NULL,	NULL,			NONE,		NONE,	NONE,				0	}
// };

int	cli_des_ecb(const char **opt, const char *name_comm)
{
	// TODO: refactor.
	return SSL_OK;
}


// int	cli_des_ecb(const char **opt, const char *name_comm)
// {
// 	int	ret;

// 	if (NULL == opt) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (NULL == (__des_htable = cli_task_htable(T, sizeof(T)/sizeof(T[0])))) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_OK != io_fopen(&__in, IO_READ|IO_STDIN, NULL)) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_OK != io_fopen(&__out, IO_WRITE|IO_STDOUT, NULL)) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	__gflag = DES_E;

// 	ret = __setup_task(opt);

// 	if (SSL_OK == ret) {
// 		ret = __run_task();
// 	}
// 	io_fclose_multi(&__in, &__out, NULL);
// 	cli_task_htable_del(__des_htable);

// 	if (SSL_OK != ret) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	return (SSL_OK);
// }

// static int	__setup_task(const char **opt)
// {
// 	int		(*f_setup)(const char *, const t_task *);
// 	t_task	*task;

// 	// dynamically setup task
// 	while (NULL != *opt) {
// 		if (NULL == (task = ft_htbl_get(__des_htable, *opt))) {
// 			SSL_LOG(ERROR, INVALID_INPUT_ERROR);
// 			return (SSL_ERR);
// 		}
// 		__gflag |= task->gflag;

// 		// if option flag is required
// 		if (task->val) {
// 			if (NULL == * ++opt) {
// 				SSL_LOG(ERROR, "expected option flag");
// 				return (SSL_ERR);
// 			}
// 		}
// 		if (NULL != (f_setup = task->ptr)) {
// 			if (SSL_OK != f_setup(*opt, task)) {
// 				SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 				return (SSL_ERR);
// 			}
// 		}
// 		opt++;
// 	}

// 	return (SSL_OK);
// }

// static int __run_task(void)
// {
// 	int			(*f_op)(t_ostring *, t_ostring *);
// 	int			ret;
// 	t_ostring	input;
// 	t_ostring	output;

// 	ret = SSL_OK;

// 	if (__pass && !__keyhex) {
// 		if (!__salthex) {
// 			rand_useed((uint64_t *)__salt, 8);
// 		}
// 	} else {
// 		ft_hex_to_bytes(__key, __keyhex, 16);
// 		ft_hex_to_bytes(__salt, __salthex, 16);
// 	}

// 	if (SSL_OK != __get_input((char **)&input.content, &input.size)) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}

// 	if (SSL_FLAG(DES_D, __gflag) && !__keyhex) {
// 		if (input.size < 16 || ft_strncmp((char *)input.content, "Salted__", 8) != 0) {
// 			SSL_LOG(ERROR, "invalid salted format");
// 			return SSL_ERR;
// 		}
// 		ft_memcpy(__salt, input.content + 8, 8);
// 		input.content = (unsigned char *)input.content + 16;
// 		input.size -= 16;
// 	}
	
// 	if (__pass) {
// 		if (SSL_OK != rand_openssl_kdf(__key, __salt, NULL, __pass)) {
// 			SSL_LOG(ERROR, "kdf failed");
// 			return (SSL_ERR);
// 		}
// 	}

// 	if (SSL_FLAG(DES_D, __gflag)) {
// 		f_op = (SSL_FLAG(DES_A, __gflag)) ? (__dec_b64) : (__dec);
// 	} else {
// 		f_op = (SSL_FLAG(DES_A, __gflag)) ? (__enc_b64) : (__enc);
// 	}
// 	ret = f_op(&input, &output);

// 	if (SSL_OK == ret) {
// 		ret = __write_output((char *)output.content, output.size);
// 	}

// 	SSL_FREE(input.content);
// 	SSL_FREE(output.content);

// 	return (ret);
// }

// static int	__get_input(char **input, size_t *insize)
// {
// 	char	buf[IO_BUFSIZE];
// 	ssize_t	rbytes;

// 	*input = NULL;
// 	*insize = 0;

// 	// if input is in base64 format set input stream delimeter to '\n'
// 	if (SSL_FLAG(DES_A | DES_D, __gflag)) {
// 		__in.delim = '\n';
// 	}
// 	while ((rbytes = io_read(&__in, buf, IO_BUFSIZE)) > 0) {
// 		SSL_REALLOC(*input, *insize, (*insize) + rbytes);
// 		ft_memcpy(*input + *insize, buf, rbytes);
// 		*insize += rbytes;
// 	}
// 	if (rbytes < 0) {
// 		SSL_FREE(*input);
// 		*insize = 0;
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	return (SSL_OK);
// }

// static int	__write_output(const char *output, size_t outsize)
// {
// 	if (SSL_FLAG(DES_N, __gflag)) {
// 		__dump_vectors();
// 	}
// 	if (SSL_FLAG(DES_A | DES_E, __gflag)) {
// 		__out.delim = '\n';
// 	}
// 	if (io_write(&__out, output, outsize) < 0) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_FLAG(DES_A | DES_E, __gflag)) {
// 		if (io_write(&__out, "\n", 1) < 0) {
// 			SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 			return (SSL_ERR);
// 		}
// 	}
// 	return (SSL_OK);
// }

// static void	__dump_vectors(void)
// {
// 	char	hex[128];

// 	ft_bytes_dumpb_hex(__salt, 8, 0, 0, hex, sizeof(hex));
// 	ft_printf("salt=%s\n", hex);

// 	ft_bytes_dumpb_hex(__key, 8, 0, 0, hex, sizeof(hex));
// 	ft_printf("key=%s\n", hex);
// }

// static int	__init_io(const char *opt, const t_task *task)
// {
// 	t_iodes	*iodes;

// 	iodes = (SSL_FLAG(IO_INPUT, task->tflag)) ? (&__in):(&__out);
// 	return (io_fopen(iodes, task->oflag, NULL));
// }

// static int	__get_vector(const char *opt, const t_task *task)
// {
// 	if (!ft_str_ishex(opt)) {
// 		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
// 		return (SSL_ERR);
// 	}

// 	if (DES_K == task->tflag) {
// 		__keyhex = (char *)opt;
// 	} else if (DES_S == task->tflag) {
// 		__salthex = (char *)opt;
// 	} else {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	return (SSL_OK);
// }

// static int	__get_pass(const char *opt, const t_task *task)
// {
// 	(void)task;
// 	__pass = opt;

// 	return (SSL_OK);
// }

// static int __set_op(const char *opt, const t_task *task)
// {
// 	uint32_t	remove_flag;

// 	if (DES_D == task->tflag) {
// 		remove_flag = DES_E;
// 	} else {
// 		remove_flag = DES_D;
// 	}

// 	// encrypt and decrypt flags are mutually exclusive
// 	__gflag &= ~remove_flag;
// 	__gflag |= task->tflag;

// 	return (SSL_OK);
// }

// static int	__enc(t_ostring *mes, t_ostring *ciph)
// {
// 	t_ostring temp;

// 	if (__pass && !__keyhex) {
// 		temp.size = mes->size + 16;
// 		SSL_ALLOC(temp.content, temp.size);
// 		ft_memcpy(temp.content, "Salted__", 8);
// 		ft_memcpy(temp.content + 8, __salt, 8);
// 		ft_memcpy(temp.content + 16, mes->content, mes->size);
// 		return (des_ecb_encrypt(__key, &temp, ciph));
// 	}
// 	return (des_ecb_encrypt(__key, mes, ciph));
// }

// static int	__enc_b64(t_ostring *mes, t_ostring *ciph)
// {
// 	t_ostring	b64;
// 	int			ret;

// 	ret = SSL_OK;

// 	if (SSL_OK != __enc(mes, ciph)) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_OK != base64_encode(ciph->content, ciph->size, &b64.content, &b64.size)) {
// 		ret = (SSL_LOG(ERROR, UNSPECIFIED_ERROR));
// 	}

// 	SSL_FREE(ciph->content);
// 	ciph->content = b64.content;
// 	ciph->size = b64.size;

// 	return (ret);
// }

// static int	__dec(t_ostring *ciph, t_ostring *mes)
// {
// 	return (des_ecb_decrypt(__key, ciph, mes));
// }

// static int	__dec_b64(t_ostring *b64, t_ostring *mes)
// {
// 	t_ostring	cipher;

// 	if (SSL_OK != base64_decode(b64->content, b64->size, &cipher.content, &cipher.size)) {
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	if (SSL_OK != des_ecb_decrypt(__key, &cipher, mes)) {
// 		SSL_FREE(cipher.content);
// 		SSL_LOG(ERROR, UNSPECIFIED_ERROR);
// 		return (SSL_ERR);
// 	}
// 	SSL_FREE(cipher.content);

// 	return (SSL_OK);
// }
