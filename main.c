#include <common.h>
#include <base64.h>
#include <des.h>
#include <hash.h>
#include <rsa.h>
#include <cmd.h>
#include <args.h>
#include <libft/string.h>

int	main(int ac, const char **av)
{
	t_arg_cmd	*cmd_arg, *sub_cmd_arg;
	t_cmd		*cmd;
	t_func_cmd	f_cmd;

	// default command
	cmd_arg = args_new_cmd(av[0], "SSL-ish command line tool", NULL);
	args_add_global_opt(cmd_arg, args_new_opt("-h", "show help", AP_OPT_TYPE_FLAG));
	args_add_global_opt(cmd_arg, args_new_opt("-debug", "enable debug logging", AP_OPT_TYPE_FLAG));
	args_add_global_opt(cmd_arg, args_new_opt("-trace", "enable trace logging", AP_OPT_TYPE_FLAG));

	// base64 command
	sub_cmd_arg = args_new_cmd("base64", "Base64 encoding", cmd_base64);
	args_add_opt(sub_cmd_arg, args_new_opt("-b", "set output line width", AP_OPT_TYPE_NUMBER));
	args_add_opt(sub_cmd_arg, args_new_opt("-i", "read message from file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-o", "output to file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-d", "base64 decode", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-e", "base64 encode", AP_OPT_TYPE_FLAG));
	args_add_cmd(cmd_arg, sub_cmd_arg);

	// digest commands
	const char *digest_commands[] = { "md5", "sha1", "sha224", "sha256", "sha384", "sha512", "sha512/224", "sha512/256"};
	for (int i = 0; i < sizeof(digest_commands) / sizeof(digest_commands[0]); i++) {
		sub_cmd_arg = args_new_cmd(digest_commands[i], ft_strjoin(ft_strup(digest_commands[i]), " message digest"), cmd_hash);
		args_add_opt(sub_cmd_arg, args_new_opt("-r", "reverse output format", AP_OPT_TYPE_FLAG));
		args_add_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode", AP_OPT_TYPE_FLAG));
		args_add_opt(sub_cmd_arg, args_new_opt("-s", "read from string", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-f", "read from file", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-p", "read from stdin", AP_OPT_TYPE_FLAG));
		args_add_cmd(cmd_arg, sub_cmd_arg);
	}

	// des commands
	const char *des_commands[] = { "des-ecb", "des-cbc"};
	for (int i = 0; i < sizeof(des_commands) / sizeof(des_commands[0]); i++) {
		sub_cmd_arg = args_new_cmd(des_commands[i], ft_strjoin(ft_strup(des_commands[i]), " crypt"), cmd_des_ecb);
		args_add_opt(sub_cmd_arg, args_new_opt("-a", "base64-encoded input/output", AP_OPT_TYPE_FLAG));
		args_add_opt(sub_cmd_arg, args_new_opt("-i", "read from file", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-o", "write to file", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-e", "des encryption mode", AP_OPT_TYPE_FLAG));
		args_add_opt(sub_cmd_arg, args_new_opt("-d", "des decryption mode", AP_OPT_TYPE_FLAG));
		args_add_opt(sub_cmd_arg, args_new_opt("-k", "hex key", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-s", "hex salt", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-p", "password", AP_OPT_TYPE_STRING));
		args_add_opt(sub_cmd_arg, args_new_opt("-n", "dump vectors", AP_OPT_TYPE_FLAG));
		if (ft_strcmp(des_commands[i], "des-cbc") == 0) {
			args_add_opt(sub_cmd_arg, args_new_opt("-v", "hex vector", AP_OPT_TYPE_STRING));
		}
		args_add_cmd(cmd_arg, sub_cmd_arg);
	}

	// genrsa command
	sub_cmd_arg = args_new_cmd("genrsa", "Generate RSA private keys", cmd_rsa_gen);
	args_add_opt(sub_cmd_arg, args_new_opt("-rand", "set rand source as list of <files>, separated by colon", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-o", "output generated key to a <file>", AP_OPT_TYPE_STRING));
	args_add_cmd(cmd_arg, sub_cmd_arg);

	// rsa command
	sub_cmd_arg = args_new_cmd("rsa", "RSA key procedures", cmd_rsa);
	args_add_opt(sub_cmd_arg, args_new_opt("-in", "read input key from file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-out", "write key to file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-inform", "set format of the input key, default is PEM-encoded", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-outform", "set format of the output key, default is PEM-encoded", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-pubin", "input key is a X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-pubout", "output key as X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-check", "check RSA private key", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-passin", "set password for input key", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-passout", "set password for output key", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-des", "encrypt output key using DES-CBC (key outform shall be PEM)", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-text", "print input key components", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-noout", "do not output key", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-modulus", "print input key modulus", AP_OPT_TYPE_FLAG));
	args_add_cmd(cmd_arg, sub_cmd_arg);

	// rsautl command
	sub_cmd_arg = args_new_cmd("rsautl", "RSA crypt", cmd_rsa_utl);
	args_add_opt(sub_cmd_arg, args_new_opt("-in", "read message form file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-out", "output to a file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-inkey", "read input key form file", AP_OPT_TYPE_STRING));
	args_add_opt(sub_cmd_arg, args_new_opt("-encrypt", "RSA encryption mode", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-decrypt", "RSA decryption mode", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-pubin", "input key is a X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_opt(sub_cmd_arg, args_new_opt("-hexdump", "hexdump output", AP_OPT_TYPE_FLAG));
	args_add_cmd(cmd_arg, sub_cmd_arg);

	// test command
	sub_cmd_arg = args_new_cmd("test", "Run tests", cmd_test);
	args_add_opt(sub_cmd_arg, args_new_opt("-v", "verbosity level", AP_OPT_TYPE_STRING));
	args_add_cmd(cmd_arg, sub_cmd_arg);
	
	cmd = args_parse(cmd_arg, av, ac);
	if (cmd == NULL) {
		CMD_LOG(ERROR, INVALID_INPUT_ERROR);
		CMD_LOG(INFO, "for help, run:\n\t%s -h\n\t%s <sub-command> -h", av[0], av[0]);
		exit(1);
	}
	if (ft_htbl_get(cmd->opts, "-h")) {
		args_dump_help(cmd->arg_ref);
		return (SSL_OK);
	}
	f_cmd = (t_func_cmd)cmd->func;

	if (f_cmd == NULL) {
		ARGP_LOG(ERROR, "noop command");
		return (SSL_OK);
	}
	return (f_cmd(cmd));
}