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
#include <args.h>
#include <libft/string.h>

static void __setup_args(t_args *args);
static int __print_help(const t_args *args);

int	main(int ac, const char **av)
{
	t_args		args;
	t_args_cmd	*cmd;
	FUNC_CMD	f_cmd;

	__setup_args(&args);

	if (ac < 2) {
		__print_help(&args);
		exit(1);
	}
	if (SSL_OK != args_parse(&args, av+1)) {
		ARGP_LOG(ERROR, "error parsing arguments");
		exit(1);
	}
	cmd = args_get_cmd(&args, av[1]);
	if (NULL == cmd) {
		ARGP_LOG(ERROR, "unknown command: %s\nrun `%s --help` to get help", av[1], av[0]);
		exit(1);
	}
	f_cmd = (FUNC_CMD)cmd->func;
	if (NULL != f_cmd) {
		return (f_cmd(cmd));
	}
	return (SSL_OK);
}

static void __setup_args(t_args *args)
{
	t_args_cmd *cmd;

	args_init(args);

	// base64 command
	cmd = args_new_cmd("base64", "base64", cmd_base64);
	args_cmd_add_opt(cmd, args_new_opt("-b", "set output line width", AP_ARG_TYPE_NUMBER));
	args_cmd_add_opt(cmd, args_new_opt("-i", "read message from file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-o", "output to file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-d", "base64 decode", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-e", "base64 encode", AP_ARG_TYPE_FLAG));
	args_add_cmd(args, cmd);

	// digest commands
	const char *digest_commands[] = { "md5", "sha1", "sha224", "sha256", "sha384", "sha512", "sha512/224", "sha512/256"};
	for (int i = 0; i < sizeof(digest_commands) / sizeof(digest_commands[0]); i++) {
		cmd = args_new_cmd(digest_commands[i], digest_commands[i], cmd_hash);
		args_cmd_add_opt(cmd, args_new_opt("-r", "reverse output format", AP_ARG_TYPE_FLAG));
		args_cmd_add_opt(cmd, args_new_opt("-q", "quiet mode", AP_ARG_TYPE_FLAG));
		args_cmd_add_opt(cmd, args_new_opt("-s", "read from string", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-f", "read from file", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-p", "read from stdin", AP_ARG_TYPE_FLAG));
		args_add_cmd(args, cmd);
	}

	// des commands
	const char *des_commands[] = { "des-ecb", "des-cbc"};
	for (int i = 0; i < sizeof(des_commands) / sizeof(des_commands[0]); i++) {
		cmd = args_new_cmd(des_commands[i], des_commands[i], cmd_des_ecb);
		args_cmd_add_opt(cmd, args_new_opt("-a", "base64-encoded input/output", AP_ARG_TYPE_FLAG));
		args_cmd_add_opt(cmd, args_new_opt("-i", "read from file", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-o", "write to file", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-e", "des encryption mode", AP_ARG_TYPE_FLAG));
		args_cmd_add_opt(cmd, args_new_opt("-d", "des decryption mode", AP_ARG_TYPE_FLAG));
		args_cmd_add_opt(cmd, args_new_opt("-k", "hex key", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-s", "hex salt", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-p", "password", AP_ARG_TYPE_STRING));
		args_cmd_add_opt(cmd, args_new_opt("-n", "dump vectors", AP_ARG_TYPE_FLAG));
		if (ft_strcmp(des_commands[i], "des-cbc") == 0) {
			args_cmd_add_opt(cmd, args_new_opt("-v", "hex vector", AP_ARG_TYPE_STRING));
		}
		args_add_cmd(args, cmd);
	}

	// genrsa command
	cmd = args_new_cmd("genrsa", "generate RSA private keys", cmd_rsa_gen);
	args_cmd_add_opt(cmd, args_new_opt("-rand", "set rand source as list of <files>, separated by colon", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-o", "output generated key to a <file>", AP_ARG_TYPE_STRING));
	args_add_cmd(args, cmd);

	// rsa command
	cmd = args_new_cmd("rsa", "perform operation on RSA keys", cmd_rsa);
	args_cmd_add_opt(cmd, args_new_opt("-in", "read input key from file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-out", "write key to file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-inform", "set format of the input key, default is PEM-encoded", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-outform", "set format of the output key, default is PEM-encoded", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-pubin", "input key is a X.509 public key", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-pubout", "output key as X.509 public key", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-check", "check RSA private key", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-passin", "set password for input key", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-passout", "set password for output key", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-des", "encrypt output key using DES-CBC (key outform shall be PEM)", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-text", "print input key components", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-noout", "do not output key", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-modulus", "print input key modulus", AP_ARG_TYPE_FLAG));
	args_add_cmd(args, cmd);

	// rsautl command
	cmd = args_new_cmd("rsautl", "perform RSA crypt", cmd_rsa_utl);
	args_cmd_add_opt(cmd, args_new_opt("-in", "read message form file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-out", "output to a file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-inkey", "read input key form file", AP_ARG_TYPE_STRING));
	args_cmd_add_opt(cmd, args_new_opt("-encrypt", "RSA encryption mode", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-decrypt", "RSA decryption mode", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-pubin", "input key is a X.509 public key", AP_ARG_TYPE_FLAG));
	args_cmd_add_opt(cmd, args_new_opt("-hexdump", "hexdump output", AP_ARG_TYPE_FLAG));
	args_add_cmd(args, cmd);

	// test command
	cmd = args_new_cmd("test", "test", cmd_test);
	args_cmd_add_opt(cmd, args_new_opt("-v", "verbosity level", AP_ARG_TYPE_STRING));
	args_add_cmd(args, cmd);

	// help command
	cmd = args_new_cmd("help", "show help", __print_help);
	args_add_cmd(args, cmd);
}

static int __print_help(const t_args *args)
{
	args_dump_help(args);
	return (SSL_OK);
}