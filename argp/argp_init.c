#include <common.h>
#include <argp.h>
#include <libft/htable.h>

int	argp_parser_init(t_ap_parser *parser)
{
	if (parser == NULL) {
		ARGP_LOG(ERROR, INVALID_INPUT_ERROR);
		return SSL_ERR;
	}
	ft_bzero(parser, sizeof(t_ap_parser));

	parser->htbl = ft_htbl_create(LIBFT_HT_SIZE);

	return SSL_OK;
}