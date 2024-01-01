/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ssl.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 10:47:08 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/05 08:45:31 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SSL_H
# define FT_SSL_H

# include <ssl_alloc.h>
# include <ssl_check.h>
# include <ssl_error.h>
# include <ft_printf.h>
# include <libft.h>

# define SSL_FLAG(F,X)		((int)(((X)&(F))==(F)))
# define IS_NONZERO_32(X)	((((X)|(0-(X)))>>31)&1)
# define IS_NONZERO_64(X)	((((X)|(0-(X)))>>63)&1)

# define NONE	0

enum	e_boolean
{
	SSL_FALSE	= 0,
	SSL_TRUE	= 1,
};

enum	e_status
{
	SSL_FAIL	= -1,
	SSL_OK		= 0
};

typedef int		(*FUNC_COM)(const char **, const char *);

typedef	struct	s_ostring
{
	char		*content;
	size_t		size;
}				t_ostring;


typedef uint32_t	t_bool;
typedef t_ostring	t_bitstring;
typedef __uint128_t	uint128_t;

typedef struct	s_task
{
	char		*key;
	void		*ptr;
	uint32_t	tflag;
	uint32_t	gflag;
	uint32_t	oflag;
	uint32_t	val;
}				t_task;

void			ssl_exit(uint32_t);
char			*ssl_getpass(void);
int				ssl_setpass(const char *);
void			ssl_unsetpass(void);

int				comm_base64(const char **, const char *);
int				comm_hash(const char **, const char *);
int				comm_des_ecb(const char **, const char *);
int				comm_des_cbc(const char **, const char *);
int				comm_rsa_gen(const char **, const char *);
int				comm_rsa(const char **, const char *);
int				comm_rsa_utl(const char **, const char *);
int				comm_test(const char **, const char *);

t_ostring		*util_ostr_init(void);
t_ostring		*util_ostr_new(size_t);
t_ostring		*util_ostr_dup(t_ostring *);
void			util_ostr_del(t_ostring *);
void			util_ostr_clean(t_ostring *);
void			util_ostr_append(t_ostring *ostring, void *content, size_t size);
void			util_ostr_prepend(t_ostring *ostring, void *content, size_t size);
char			*util_ostr_to_str(t_ostring *ostring, size_t idx, size_t len);
void			util_ostr_set_content(t_ostring *ostring, void *content, size_t size);
void			*util_ostr_get_content(t_ostring *ostring);
size_t			util_ostr_get_size(t_ostring *ostring);

uint32_t		util_bswap32(uint32_t);
uint64_t		util_bswap64(uint64_t);
uint128_t		util_bswap128(uint128_t);
void			util_xor(char *, char *, char *, size_t);
int 			util_lmbit(uint64_t, int);
int 			util_rmbit(uint64_t, int);
uint64_t		util_bytes_to_uint64(char *bytes, int nbytes);
void			util_lshift_bytes(unsigned char *, int, int);
void			util_rshift_bytes(unsigned char *, int, int);
void			util_puthex(void *, int, int, int);
void			util_hexdump(int fd, void *ptr, size_t size);
t_htbl  		*util_task_htable(const t_task *, int);
void			util_task_htable_del(const t_htbl *);
void			util_print_intbits(uint64_t, int, int);
void			util_print_strbits(void *, size_t);

#endif
