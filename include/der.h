#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
#include <libft.h>

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
