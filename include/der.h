#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>

# define DER_LOGGER_NAME	"der"
# define DER_LOG(LEVEL, MES, ...)	ft_logger_log(__func__, __FILE__, __LINE__, NULL, DER_LOGGER_NAME, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

struct s_node;
struct s_iodes;

t_ostring	*der_init(void);
void	der_del(t_ostring *);
void	der_clear(t_ostring *);

int	der_encode(struct s_node *tree, t_ostring *encoded);
int	der_encode_stream(struct s_node *tree, struct s_iodes *out);
int der_decode(struct s_node **tree, t_ostring *encoded);
int der_decode_stream(struct s_node **tree, struct s_iodes *in);

#endif
