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

# include <ssl/alloc.h>
# include <ssl/error.h>
# include <test/check.h>
# include <libft/htable.h>

# define SSL_FLAG(F,X)		((int)(((X)&(F))==(F)))

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

typedef uint32_t	t_bool;
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

t_htbl  		*ssl_task_htable(const t_task *, int);
void			ssl_task_htable_del(const t_htbl *);

#endif
