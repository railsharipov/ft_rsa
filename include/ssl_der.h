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

typedef struct	s_der
{
	char	*content;
	size_t	size;
}				t_der;

struct s_iasn;

t_der	*der_init(void);
void	der_del(t_der *);
void	der_append_content(t_der *der, void *content, size_t size);
int		der_append_id_tag(t_der *der, int tag_flags, uint32_t tag_num);

void	der_append_content(t_der *der, void *content, size_t nbits);
int		der_append_sequence(t_der *der, void *content, size_t nbits);
int		der_append_bool(t_der *der, void *content, size_t nbits);
int		der_append_int(t_der *der, void *content, size_t nbits);
int		der_append_ostring(t_der *der, void *content, size_t nbits);
int		der_append_bitstring(t_der *der, void *content, size_t nbits);
int		der_append_oid(t_der *der, void *content, size_t nbits);
int		der_append_null(t_der *der, void *content, size_t nbits);
int		der_append_len(char *, size_t size);
int		der_read_sequence(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_bool(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_int(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_ostring(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_bitstring(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_oid(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_null(struct s_iasn *item, char **derenc, size_t *dersize);
int		der_read_len(unsigned char **, size_t *, size_t *);

#endif
