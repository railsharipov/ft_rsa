#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__get_obj_id_octets(char **, size_t *, char *);
static int	__check_sub_ids(char **, int);
static void	__get_sub_ids(int **, int *, char **, int);
static void	__encode_sub_ids(char **, size_t *, uint32_t *, int);

int	der_append_oid(t_der *der, void *content, size_t cont_nbits)
{
	char	*obj_id_string;
	char	*obj_name;
	char	*obj_octets;
	size_t	obj_size;

	(void)cont_nbits;

	if (NULL == der || NULL == content)
		return (DER_ERROR(INVALID_INPUT));

	obj_name = (char *)content;

	if (NULL == (obj_id_string = asn_oid_tree_get_oid(obj_name)))
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));

	if (SSL_OK != __get_obj_id_octets(&obj_octets, &obj_size, obj_id_string))
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));

	der_append_id_tag(
		der,
		ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL,
		ASN_TAG_OBJECT_ID);

	der_append_len(der, obj_size);
	der_append_content(der, obj_octets, obj_size);

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
	num_sub_id_strings = ft_2darray_len((void **)sub_id_strings);

	if (SSL_OK == (ret = __check_sub_ids(sub_id_strings, num_sub_id_strings)))
	{
		__get_sub_ids(&sub_ids, &num_sub_ids, sub_id_strings, num_sub_id_strings);
		__encode_sub_ids(obj_octets, obj_size, (uint32_t *)sub_ids, num_sub_ids);

		SSL_FREE(sub_ids);
	}

	ft_2darray_del((void **)sub_id_strings, -1);

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
	unsigned char	*octets;
	size_t			osize;
	size_t			sub_id_word_size;
	size_t			sub_id_word_bitsize;
	size_t			shift, bitcnt;

	sub_id_word_size = sizeof(*sub_ids);

	osize = NBITS_TO_NWORDS(CHAR_BIT * sub_id_word_size * num_sub_ids, 7);
	SSL_ALLOC(octets, MIN(1, osize));

	osize = 0;

	while (num_sub_ids-- > 0)
	{
		sub_id_word_bitsize = util_lmbit(*sub_ids, CHAR_BIT * sub_id_word_size);
		// round up to a multiple of 7
		sub_id_word_bitsize = CEIL(sub_id_word_bitsize, 7);

		bitcnt = 0;

		// get 7-bit blocks except last one
		while (bitcnt + 7 < sub_id_word_bitsize)
		{
			bitcnt += 7;
			shift = sub_id_word_bitsize - bitcnt;

			*octets++ = ((*sub_ids >> shift) & 0x7F) | 0x80;
			osize += 1;
		}

		// get last sub id 7-bit block
		*octets = *sub_ids & 0x7F;
		osize += 1;

		sub_ids++;
	}

	*obj_octets = (char *)octets;
	*obj_size = osize;
}
