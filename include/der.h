#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
# include <libft/string.h>
# include <libft/logger.h>

# define DER_LOG(LEVEL, MES, ...)	der_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

struct s_node;
struct s_iodes;

t_ostring	*der_init(void);
void	der_del(t_ostring *);
void	der_clear(t_ostring *);

int	der_encode(struct s_node *tree, struct s_iodes *iodes);
int der_decode(struct s_node **tree, struct s_iodes *iodes);

int	der_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

#endif
