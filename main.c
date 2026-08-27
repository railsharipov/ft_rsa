#include <common.h>
#include <logger.h>
#include <base64.h>
#include <des.h>
#include <digest.h>
#include <rsa.h>
#include <cmd.h>
#include <args.h>
#include <libft.h>

static int __app_log_writer(const char *mes) {
	return (ft_printf("%@%s\n", mes));
}

int	main(int ac, const char **av)
{
	t_arg_cmd	*cmd_arg, *sub_cmd_arg;
	t_cmd		cmd;
	t_func_cmd	f_cmd;

	t_logger *__app_logger = logger_get_logger();
	*__app_logger = (t_logger){
		.log_writer = __app_log_writer,
		.log_level_thres = LIBFT_LOG_LEVEL_INFO,
		.debug_info_thres = LIBFT_LOG_LEVEL_TRACE,
		.is_ansi_colored = 1,
	};

	// default command
	cmd_arg = args_new_cmd(av[0], "SSL-ish command line tool", NULL);
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--debug", "set debug log level", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--trace", "set trace log level", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--warn", "set warning log level", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--info", "set info log level", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("--error", "set error log level", AP_OPT_TYPE_FLAG));
	args_add_global_cmd_opt(cmd_arg, args_new_opt("-q", "quiet mode, errors only", AP_OPT_TYPE_FLAG));

	// base64 command
	sub_cmd_arg = args_new_cmd("base64", "Base64 encoding", cmd_base64);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-b", "set output line width", AP_OPT_TYPE_NUMBER));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-i", "read message from file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-o", "output to file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-d", "base64 decode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-e", "base64 encode", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// digest commands
	sub_cmd_arg = args_new_cmd("md5", "MD5 message digest", cmd_md5);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha1", "SHA1 message digest", cmd_sha1);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha256", "SHA256 message digest", cmd_sha256);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha224", "SHA224 message digest", cmd_sha224);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha512", "SHA512 message digest", cmd_sha512);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha384", "SHA384 message digest", cmd_sha384);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha512-224", "SHA512/224 message digest", cmd_sha512_224);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("sha512-256", "SHA512/256 message digest", cmd_sha512_256);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-r", "reverse the format of the output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-q", "quiet mode, only the checksum is printed out", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "print checksum of the given string", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "echo stdin to stdout and append the checksum to stdout", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// des commands
	sub_cmd_arg = args_new_cmd("des-ecb", "DES-ECB crypt", cmd_des_ecb);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-a", "base64-encoded input/output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-i", "read from file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-o", "write to file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-e", "des encryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-d", "des decryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-k", "hex key", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "hex salt", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "password", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-n", "dump vectors", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	sub_cmd_arg = args_new_cmd("des-cbc", "DES-CBC crypt", cmd_des_cbc);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-a", "base64-encoded input/output", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-i", "read from file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-o", "write to file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-e", "des encryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-d", "des decryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-k", "hex key", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-s", "hex salt", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-v", "hex vector", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-p", "password", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-n", "dump vectors", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// POC

	sub_cmd_arg = args_new_cmd("asn1", "ASN.1 procedures", cmd_asn1);

	// NOT FULLY IMPLEMENTED YET

	// genrsa command
	sub_cmd_arg = args_new_cmd("genrsa", "Generate RSA private keys", cmd_rsa_gen);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-rand", "set rand source as list of <files>, separated by colon", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-o", "output generated key to a <file>", AP_OPT_TYPE_STRING));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// rsa command
	sub_cmd_arg = args_new_cmd("rsa", "RSA key procedures", cmd_rsa);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-in", "read input key from file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-out", "write key to file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-inform", "set format of the input key, default is PEM-encoded", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-outform", "set format of the output key, default is PEM-encoded", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-pubin", "input key is a X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-pubout", "output key as X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-check", "check RSA private key", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-passin", "set password for input key", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-passout", "set password for output key", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-des", "encrypt output key using DES-CBC (key outform shall be PEM)", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-text", "print input key components", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-noout", "do not output key", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-modulus", "print input key modulus", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// rsautl command
	sub_cmd_arg = args_new_cmd("rsautl", "RSA crypt", cmd_rsa_utl);
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-in", "read message form file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-out", "output to a file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-inkey", "read input key form file", AP_OPT_TYPE_STRING));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-encrypt", "RSA encryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-decrypt", "RSA decryption mode", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-pubin", "input key is a X.509 public key", AP_OPT_TYPE_FLAG));
	args_add_cmd_opt(sub_cmd_arg, args_new_opt("-hexdump", "hexdump output", AP_OPT_TYPE_FLAG));
	args_add_sub_cmd(cmd_arg, sub_cmd_arg);

	// parse arguments
	if (SSL_OK != args_parse(&cmd, cmd_arg, av, ac)) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		if (cmd.arg_ref != NULL) {
			args_dump_help(cmd.arg_ref);
		} else {
			args_dump_help(cmd_arg);
		}
		return (1);
	}

	// handle global options
	if (ft_htbl_has(cmd.opts, "--warn")) {
		__app_logger->log_level_thres = LIBFT_LOG_LEVEL_WARN;
	}
	if (ft_htbl_has(cmd.opts, "--info")) {
		__app_logger->log_level_thres = LIBFT_LOG_LEVEL_INFO;
	}
	if (ft_htbl_has(cmd.opts, "--debug")) {
		__app_logger->log_level_thres = LIBFT_LOG_LEVEL_DEBUG;
	}
	if (ft_htbl_has(cmd.opts, "--trace")) {
		__app_logger->log_level_thres = LIBFT_LOG_LEVEL_TRACE;
	}
	if (ft_htbl_has(cmd.opts, "-q") || ft_htbl_has(cmd.opts, "--error")) {
		__app_logger->log_level_thres = LIBFT_LOG_LEVEL_ERROR;
	}

	// handle help option
	if (ft_htbl_has(cmd.opts, "-h")) {
		args_dump_help(cmd.arg_ref);
		return (0);
	}

	// call command function
	f_cmd = (t_func_cmd)cmd.func;
	if (f_cmd != NULL && SSL_OK != f_cmd(&cmd)) {
		return (1);
	}
	return (0);
}
