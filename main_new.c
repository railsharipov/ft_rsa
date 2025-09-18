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

#include <common.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <cmd.h>
#include <argp.h>
#include <libft/string.h>

int	main(int ac, const char **av)
{
	t_ap_parser	parser;
	t_ap_cmd	*cmd;

	if (ac < 2) {
		__print_usage();
		exit(1);
	}
	argp_parser_init(&parser);

	// base64 command
	cmd = argp_new_cmd("base64", "base64", cmd_base64);
	argp_add_opt(cmd, (t_ap_arg){"-b", "set output line width to <num>", AP_ARG_TYPE_NUMBER});
	argp_add_opt(cmd, (t_ap_arg){"-i", "read message from <file>", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-o", "output to <file>", AP_ARG_TYPE_STRING});
	argp_add_cmd(&parser, cmd);

	// digest commands
	const char *digest_commands[] = { "md5", "sha1", "sha224", "sha256", "sha384", "sha512", "sha512/224", "sha512/256"};
	for (int i = 0; i < sizeof(digest_commands) / sizeof(digest_commands[0]); i++) {
		cmd = argp_new_cmd(digest_commands[i], digest_commands[i], cmd_hash);
		argp_add_opt(cmd, (t_ap_arg){"-r", "reverse output format", AP_ARG_TYPE_FLAG});
		argp_add_opt(cmd, (t_ap_arg){"-q", "quiet mode", AP_ARG_TYPE_FLAG});
		argp_add_opt(cmd, (t_ap_arg){"-s", "read from string", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-f", "read from file", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-p", "read from stdin", AP_ARG_TYPE_FLAG});
		argp_add_cmd(&parser, cmd);
	}

	// des commands
	const char *des_commands[] = { "des-ecb", "des-cbc"};
	for (int i = 0; i < sizeof(des_commands) / sizeof(des_commands[0]); i++) {
		cmd = argp_new_cmd(des_commands[i], des_commands[i], cmd_des_ecb);
		argp_add_opt(cmd, (t_ap_arg){"-a", "base64-encode output / base64-decode input (depending on des mode)", AP_ARG_TYPE_FLAG});
		argp_add_opt(cmd, (t_ap_arg){"-i", "read from file", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-o", "write to file", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-e", "des encryption mode", AP_ARG_TYPE_FLAG});
		argp_add_opt(cmd, (t_ap_arg){"-d", "des decryption mode", AP_ARG_TYPE_FLAG});
		argp_add_opt(cmd, (t_ap_arg){"-k", "hex key", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-s", "hex salt", AP_ARG_TYPE_STRING});
		argp_add_opt(cmd, (t_ap_arg){"-p", "password", AP_ARG_TYPE_STRING});
		argp_add_cmd(&parser, cmd);
	}

	// genrsa command
	cmd = argp_new_cmd("genrsa", "generate RSA private keys", cmd_rsa_gen);
	argp_add_opt(cmd, (t_ap_arg){"-rand", "set rand source as list of <files>, separated by colon", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-o", "output generated key to a <file>", AP_ARG_TYPE_STRING});
	argp_add_cmd(&parser, cmd);

	// rsa command
	cmd = argp_new_cmd("rsa", "perform operation on RSA keys", cmd_rsa);
	argp_add_opt(cmd, (t_ap_arg){"-in", "read input key from file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-out", "write key to file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-inform", "set format of the input key, default is PEM-encoded", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-outform", "set format of the output key, default is PEM-encoded", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-pubin", "input key is a X.509 public key", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-pubout", "output key as X.509 public key", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-check", "check RSA private key", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-passin", "set password for input key", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-passout", "set password for output key", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-des", "encrypt output key using DES-CBC (key outform shall be PEM)", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-text", "print input key components", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-noout", "do not output key", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-modulus", "print input key modulus", AP_ARG_TYPE_FLAG});
	argp_add_cmd(&parser, cmd);

	// rsautl command
	cmd = argp_new_cmd("rsautl", "perform RSA crypt", cmd_rsa_utl);
	argp_add_opt(cmd, (t_ap_arg){"-in", "read message form file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-out", "output to a file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-inkey", "read input key form file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-encrypt", "RSA encryption mode", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-decrypt", "RSA decryption mode", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-pubin", "input key is a X.509 public key", AP_ARG_TYPE_FLAG});
	argp_add_opt(cmd, (t_ap_arg){"-hexdump", "hexdump output", AP_ARG_TYPE_FLAG});
	argp_add_cmd(&parser, cmd);

	// test command
	cmd = argp_new_cmd("test", "test", cmd_test);
	argp_add_opt(cmd, (t_ap_arg){"-i", "read from file", AP_ARG_TYPE_STRING});
	argp_add_opt(cmd, (t_ap_arg){"-o", "write to file", AP_ARG_TYPE_STRING});
	argp_add_cmd(&parser, cmd);

	if (SSL_OK != argp_parse(&parser, av+1)) {
		ARGP_LOG(ERROR, "failed to parse arguments");
		exit(1);
	}

	// execute command
	cmd = argp_get_cmd(&parser, av[1]);
	if (NULL == cmd) {
		ARGP_LOG(ERROR, "unknown command: %s", av[1]);
		exit(1);
	}
	if (NULL != cmd->func) {
		((FUNC_CMD)cmd->func)(cmd);
	}

	return (0);
}