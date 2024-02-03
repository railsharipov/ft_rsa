/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/06 11:09:10 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 10:41:35 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ssl/ssl.h>
#include <ssl/base64.h>
#include <ssl/des.h>
#include <ssl/hash.h>
#include <ssl/rsa.h>
#include <ssl/comm.h>
#include <libft/string.h>

static const struct {
	char			*name_comm;
	FUNC_COM		func_comm;
} COMMAND[] = {
	{	"md5",			comm_hash		},
	{	"sha1",			comm_hash		},
	{	"sha224",		comm_hash		},
	{	"sha256",		comm_hash		},
	{	"sha384",		comm_hash		},
	{	"sha512",		comm_hash		},
	{	"sha512/224",	comm_hash		},
	{	"sha512/256",	comm_hash		},
	{	"base64", 		comm_base64		},
	{	"des-ecb", 		comm_des_ecb	},
	{	"des-cbc",		comm_des_cbc	},
	{	"genrsa",		comm_rsa_gen	},
	{	"rsa",			comm_rsa		},
	{	"rsautl",		comm_rsa_utl	},
	{	"test",			comm_test		},
	{	NULL,			NULL			}
};

static void	__get_command(
	FUNC_COM *func_comm, char **name_comm, const char *sarg)
{
	int	ix;

	*func_comm = NULL;
	*name_comm = NULL;

	ix = 0;
	while (NULL != COMMAND[ix].name_comm)
	{
		if (!ft_strcmp(COMMAND[ix].name_comm, sarg))
		{
			*func_comm = COMMAND[ix].func_comm;
			*name_comm = COMMAND[ix].name_comm;
			break ;
		}
		ix++;
	}
}

static int	__f_stderr_logger(const char *mes)
{
	ft_printf("%@%s\n", mes);
	return (SSL_ERR);
}

int		main(int ac, const char **av)
{
	FUNC_COM	func_comm;
	char		*name_comm;

	ssl_error_set_logger(__f_stderr_logger);
	ssl_error_set_level(SSL_ERROR_LEVEL_DEBUG);

	if (ac < 2) {
		ssl_print_usage();
		exit(1);
	}
	__get_command(&func_comm, &name_comm, av[1]);

	if (NULL == name_comm) {
		ssl_print_usage();
		exit(1);
	}
	func_comm(av+2, name_comm);

	return (0);
}
