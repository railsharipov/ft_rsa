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

enum	e_status
{
	SSL_FALSE		= 0UL,
	SSL_TRUE		= 1UL,
	SSL_OK			= 1UL << 1,
	SSL_ERR			= 1UL << 2,
	SSL_ERR_USAGE	= 1UL << 3,
	SSL_ERR_MEMORY	= 1UL << 4
};

typedef uint32_t	t_bool;
typedef __uint128_t	t_uint128;

typedef int		(*FUNC_COM)(char **, const char *);

typedef	struct	s_ostring
{
	char		*content;
	size_t		size;
}				t_ostring;

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

int				comm_base64(char **, const char *);
int				comm_hash(char **, const char *);
int				comm_des_ecb(char **, const char *);
int				comm_des_cbc(char **, const char *);
int				comm_rsa_gen(char **, const char *);
int				comm_rsa(char **, const char *);
int				comm_rsa_utl(char **, const char *);

void			util_ostrinit(t_ostring *);
t_ostring		*util_ostrnew(void *, size_t);
t_ostring		*util_ostrdup(t_ostring *);
void			util_ostrdel(t_ostring *);
uint32_t		util_bswap32(uint32_t);
uint64_t		util_bswap64(uint64_t);
t_uint128		util_bswap128(t_uint128);
void			util_xor(char *, char *, char *, size_t);
int 			util_lmbit(uint64_t, int);
int 			util_rmbit(uint64_t, int);
void			util_lshift_bytes(unsigned char *, int, int);
void			util_rshift_bytes(unsigned char *, int, int);
void			util_puthex(void *, int, int, int);
void			util_hexdump(int fd, void *ptr, size_t size);
int				util_null(int, ...);
t_htbl  		*util_task_htable(const t_task *, int);
void			util_print_intbits(uint64_t, int, int);
void			util_print_strbits(void *, size_t);

#endif
