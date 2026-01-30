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

#ifndef FT_CLI_H
# define FT_CLI_H

# include <common.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define NONE	0

typedef int		(*t_func_comm)(const char **, const char *);

typedef struct	s_task
{
	char		*key;
	void		*ptr;
	uint32_t	tflag;
	uint32_t	gflag;
	uint32_t	oflag;
	uint32_t	val;
}				t_task;

int		cli_logger_print_file(const char *file_name);
int		cli_read_stdin_pass(char *buf, size_t bufsize);
t_htbl  *cli_task_htable(const t_task *task, int size);
void    cli_task_htable_del(const t_htbl *htbl);

int     cli_base64(const char **opt, const char *name_comm);
int     cli_hash(const char **opt, const char *name_comm);
int     cli_des_ecb(const char **opt, const char *name_comm);
int     cli_des_cbc(const char **opt, const char *name_comm);
int     cli_rsa_gen(const char **opt, const char *name_comm);
int     cli_rsa(const char **opt, const char *name_comm);
int     cli_rsa_utl(const char **opt, const char *name);
int     cli_test(const char **opt, const char *name);

#endif
