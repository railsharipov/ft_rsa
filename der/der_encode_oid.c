#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__get_obj_id_octets(char **, size_t *, char *);
static int	__check_sub_ids(char **, int);
static void	__get_sub_ids(int **, int *, char **, int);
static void	__encode_sub_ids(char **, size_t *, uint32_t *, int);

int	der_encode_oid(t_ostring *osbuf, void *content, size_t size)
{
	char	obj_name[size+1];
	char	*obj_id_string;
	char	*obj_octets;
	size_t	obj_size;

	if (NULL == osbuf || NULL == content)
		return (DER_ERROR(INVALID_INPUT));

	ft_memcpy(obj_name, content, size);
	obj_name[size] = 0;

	if (NULL == (obj_id_string = asn_oid_tree_get_oid(obj_name)))
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));

	if (SSL_OK != __get_obj_id_octets(&obj_octets, &obj_size, obj_id_string))
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));

	osbuf->content = ft_memdup(obj_octets, obj_size);
	osbuf->size = obj_size;

	return (SSL_OK);
}

static int	__get_obj_id_octets(
	char **obj_octets, size_t *obj_size, char *obj_id_string)
{
	char	**sub_id_strings;
	int		num_sub_id_strings;
	int		*sub_ids;
	int		num_sub_ids;
	int		ret;

	sub_id_strings = ft_strsplit(obj_id_string, '.');
	num_sub_id_strings = ft_2darray_len_null_terminated((void **)sub_id_strings);

	if (SSL_OK == (ret = __check_sub_ids(sub_id_strings, num_sub_id_strings)))
	{
		__get_sub_ids(&sub_ids, &num_sub_ids, sub_id_strings, num_sub_id_strings);
		__encode_sub_ids(obj_octets, obj_size, (uint32_t *)sub_ids, num_sub_ids);

		SSL_FREE(sub_ids);
	}

	ft_2darray_del_null_terminated((void **)sub_id_strings);

	return (ret);
}

static int	__check_sub_ids(char **sub_id_strings, int num_sub_id_strings)
{
	if (num_sub_id_strings < 2 || NULL == sub_id_strings)
	{
		return (DER_ERROR(UNSPECIFIED_ERROR));
	}

	while (num_sub_id_strings-- > 0)
	{
		if (!ft_str_isdigit(sub_id_strings[num_sub_id_strings]))
			return (DER_ERROR(UNSPECIFIED_ERROR));
	}

	return (SSL_OK);
}

static void	__get_sub_ids(
	int **sub_ids, int *num_sub_ids, char **sub_id_strings, int num_sub_id_strings)
{
	int	*ids;
	int	num_ids;
	int	ix;

	ids = *sub_ids;
	num_ids = num_sub_id_strings - 1;

	SSL_ALLOC(ids, sizeof(*ids) * num_ids);

	ids[0] = 40 * ft_atoi(sub_id_strings[0]) + ft_atoi(sub_id_strings[1]);

	ix = 0;
	while (NULL != sub_id_strings[ix+2])
	{
		ids[ix+1] = ft_atoi(sub_id_strings[ix+2]);
		ix++;
	}

	*sub_ids = ids;
	*num_sub_ids = num_ids;
}

static void	__encode_sub_ids(
	char **obj_octets, size_t *obj_size, uint32_t *sub_ids, int num_sub_ids)
{
	unsigned char	*id_octets;
	size_t			id_octets_size;
	unsigned char	*sub_id_enc;
	int				sub_id_enc_size;
	int				sub_id_enc_nbits;
	uint32_t		sub_id;
	ssize_t			idx;

	id_octets_size = NBITS_TO_NWORDS(8 * sizeof(*sub_ids) * num_sub_ids, 7);
	id_octets_size = MAX(1, id_octets_size);
	SSL_ALLOC(id_octets, id_octets_size);

	id_octets_size = 0;
	idx = 0;

	while (num_sub_ids-- > 0)
	{
		sub_id = *sub_ids++;
		sub_id_enc_nbits = util_lmbit(sub_id, 8*sizeof(sub_id));
		sub_id_enc_size = NBITS_TO_NWORDS(sub_id_enc_nbits, 7);

		SSL_ALLOC(sub_id_enc, sub_id_enc_size);

		idx = sub_id_enc_size;

		sub_id_enc[--idx] = 0x7F & sub_id;
		sub_id >>= 7;

		while (sub_id != 0)
		{
			sub_id_enc[--idx] = (0x7F & sub_id) | 0x80;
			sub_id >>= 7;
		}

		ft_memcpy(id_octets + id_octets_size, sub_id_enc, sub_id_enc_size);
		id_octets_size += sub_id_enc_size;
		SSL_FREE(sub_id_enc);
	}

	*obj_octets = (char *)id_octets;
	*obj_size = id_octets_size;
}
