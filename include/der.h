#ifndef SSL_DER_H
# define SSL_DER_H

# include <stdint.h>
# include <common.h>
# include <libft.h>
# include <io.h>

# define DER_BYTE_TAG_SIMPLE		0x0
# define DER_BYTE_TAG_COMPLEX		0x1F

# define DER_BYTE_TAG_UNIVERSAL		0x0
# define DER_BYTE_TAG_APPLICATION	0x40
# define DER_BYTE_TAG_CONTEXT		0x80
# define DER_BYTE_TAG_PRIVATE		0xC0

# define DER_BYTE_TYPE_PRIMITIVE	0x0
# define DER_BYTE_TYPE_CONSTRUCT	0x20
# define DER_BYTE_TYPE_BOOLEAN		0x1
# define DER_BYTE_TYPE_INT			0x2
# define DER_BYTE_TYPE_BIT_STRING	0x3
# define DER_BYTE_TYPE_OCTET_STRING	0x4
# define DER_BYTE_TYPE_NULL			0x5
# define DER_BYTE_TYPE_OBJECT_ID	0x6
# define DER_BYTE_TYPE_OBJECT_DESCR	0x7
# define DER_BYTE_TYPE_SEQUENCE		0x10

# define DER_BYTE_TAG_SUBSEQ		0x80
# define DER_BYTE_TAG_SUBSEQ_LAST	0x0

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
int	der_encode_stream(t_node *asn1_node, t_io_v2_stream *out);
int der_decode(t_node **asn1_node, t_ostring *encoded);
int der_decode_stream(t_node **asn1_node, t_io_v2_stream *in);

#endif
