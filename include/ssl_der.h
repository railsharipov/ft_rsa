#ifndef SSL_DER_H
# define SSL_DER_H

# define DER_ERROR(ERROR)	SSL_ERROR(ADD_ERROR_CTX(ERROR, DER_ERROR_CTX))

enum	e_der_error
{
	INVALID_ASN_OBJECT_ID = 1,
	INVALID_ASN_TYPE_TAG,
	INVALID_ASN_LEN_TAG,
	INVALID_DER_ENCODING,
	UNKNOWN_ASN_OBJECT_ID,
};

typedef struct		s_der
{
	unsigned char	*content;
	size_t			size;
}					t_der;

struct s_iodes;

t_der	*der_init(void);
void	der_del(t_der *);
void	der_clean(t_der *);

ssize_t	der_read_octets(char *content, size_t size, struct s_iodes *);
ssize_t	der_read_octets_indef(char **content, size_t *size, struct s_iodes *);
ssize_t	der_read_tag(uint8_t *tag, uint32_t *tagnum, struct s_iodes *);
ssize_t	der_read_len(size_t *len, uint8_t *form, struct s_iodes *);

ssize_t	der_write_octets(char *content, size_t size, struct s_iodes *);
ssize_t	der_write_tag(uint8_t tag_flags, uint32_t tag_num, struct s_iodes *);
ssize_t	der_write_len(size_t len, struct s_iodes *);

int		der_encode_sequence(t_ostring *osbuf, void *content, size_t size);
int		der_encode_bool(t_ostring *osbuf, void *content, size_t size);
int		der_encode_int(t_ostring *osbuf, void *content, size_t size);
int		der_encode_ostring(t_ostring *osbuf, void *content, size_t size);
int		der_encode_bitstring(t_ostring *osbuf, void *content, size_t size);
int		der_encode_oid(t_ostring *osbuf, void *content, size_t size);
int		der_encode_null(t_ostring *osbuf, void *content, size_t size);

int		der_decode_sequence(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_bool(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_int(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_ostring(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_bitstring(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_oid(t_ostring *osbuf, uint8_t *enc, size_t size);
int		der_decode_null(t_ostring *osbuf, uint8_t *enc, size_t size);

#endif
