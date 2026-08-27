#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
#include <libft.h>

typedef enum e_der_status {
	DER_OK = 0,
	DER_NEED_MORE,
	DER_INVALID,
	DER_IO_ERROR,
	DER_NO_MEMORY,
	DER_OUTPUT_FULL,
} t_der_status;

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
