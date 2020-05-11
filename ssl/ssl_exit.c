#include <ft_ssl.h>

void	ssl_exit(uint32_t error)
{
	if (ERR_INVALID == error)
	{
		ft_printf("\n%s%s\n\n%s%s\n%-14s%s\n%-14s%s\n%-14s%s\n"\
				"%-14s%s\n%-14s%s\n\n",
			"Usage: ", "./ft_ssl | command | flags <opt>",
			"Digest commands: md5, ",
			"sha1, sha224, sha256, sha384, sha512, sha512/224, sha512/256",
			"	<file>", "read from <file>",
			"	-p", "read from IO_STDIN", "	-r", "reverse output format",
			"	-q", "quiet mode", "	-s <string>", "input <string>");
		ft_printf("%s%s\n%-14s%s\n%-14s%s\n%-14s%s\n"\
				"%-14s%s\n%-14s%s\n%-14s%s\n%-14s%s\n%-14s%s\n\n",
			"Cipher commands: ", "base64, des-ecb, des-cbc",
			"	-a", "encode/decode the input/output to base64",
			"	-i <file>", "read from <file>",
			"	-o <file>", "write output to <file>",
			"	-e", "encrypt mode", "	-d", "decrypt mode",
			"	-k <key>", "hex key (des)", "	-s <salt>", "hex salt (des)",
			"	-p <pass>", "password (des)");
	}
	else if (ERR_MEMORY == error)
		ft_printf("%@FATAL: Memory error\n");
	else if (ERR_NULL == error)
		ft_printf("%@FATAL: Unexpected null pointer\n");

	ft_free_all();
	exit(0);
}
