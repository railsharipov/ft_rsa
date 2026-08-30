#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
# include <libft.h>
# include <io.h>

typedef enum e_der_status {
	DER_OK = 0,
	DER_NEED_MORE,
	DER_INVALID,
	DER_IO_ERROR,
	DER_NO_MEMORY,
	DER_OUTPUT_FULL,
} t_der_status;

t_ostring	*der_init(void);
void		der_del(t_ostring *);
void		der_clear(t_ostring *);

int	der_encode(t_node *asn1_node, t_ostring *encoded);
int	der_encode_stream(t_node *asn1_node, t_iodes *out);
int der_decode(t_node **asn1_node, t_ostring *encoded);
int der_decode_stream(t_node **asn1_node, t_io_v2_stream *in);

#endif
