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

#ifndef FT_CMD_H
# define FT_CMD_H

# include <common.h>
# include <alloc.h>
# include <libft/htable.h>
# include <libft/logger.h>

# define NONE	0

struct s_cmd;

typedef int		(*t_func_cmd)(const struct s_cmd *cmd);

int		cmd_logger_print_file(const char *file_name);
int		cmd_read_stdin_pass(char *buf, size_t bufsize);

int     cmd_default(const struct s_cmd *cmd);
int     cmd_base64(const struct s_cmd *cmd);
int     cmd_hash(const struct s_cmd *cmd);
int     cmd_des_ecb(const struct s_cmd *cmd);
int     cmd_des_cbc(const struct s_cmd *cmd);
int     cmd_rsa_gen(const struct s_cmd *cmd);
int     cmd_rsa(const struct s_cmd *cmd);
int     cmd_rsa_utl(const struct s_cmd *cmd);

#endif
