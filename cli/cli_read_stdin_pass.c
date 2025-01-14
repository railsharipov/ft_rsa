#include <pwd.h>
#include <unistd.h>
#include <ssl.h>
#include <libft/string.h>

int	cli_read_stdin_pass(char *buf, size_t bufsize)
{
	char	*input;
	int		inlen;
	int		ret;

	ret = SSL_OK;
	input = getpass("enter password:");
	inlen = ft_strlen(input);

	if (inlen > bufsize) {
		ret = CLI_LOG(ERROR, INVALID_INPUT_ERROR);
	} else {
		ft_memzcpy(buf, input, bufsize, inlen);
		input = getpass("confirm password:");
		if (ft_strcmp(buf, input)) {
			ret = CLI_LOG(ERROR, "passwords don't match");
		}
	}
	ft_bzero(input, _PASSWORD_LEN);

	return (ret);
}
