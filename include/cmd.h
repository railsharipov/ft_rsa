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
#include <libft.h>

# define NONE	0

struct s_cmd;

typedef int		(*t_func_cmd)(const struct s_cmd *cmd);

int		cmd_logger_print_file(const char *file_name);
int		cmd_read_stdin_pass(char *buf, size_t bufsize);

int     cmd_base64(const struct s_cmd *cmd);
int     cmd_md5(const struct s_cmd *cmd);
int     cmd_sha1(const struct s_cmd *cmd);
int     cmd_sha224(const struct s_cmd *cmd);
int     cmd_sha256(const struct s_cmd *cmd);
int     cmd_sha512(const struct s_cmd *cmd);
int     cmd_sha384(const struct s_cmd *cmd);
int     cmd_sha512_224(const struct s_cmd *cmd);
int     cmd_sha512_256(const struct s_cmd *cmd);
int     cmd_des_ecb(const struct s_cmd *cmd);
int     cmd_des_cbc(const struct s_cmd *cmd);

int     cmd_asn1(const struct s_cmd *cmd);

int     cmd_rsa_gen(const struct s_cmd *cmd);
int     cmd_rsa(const struct s_cmd *cmd);
int     cmd_rsa_utl(const struct s_cmd *cmd);

#endif
