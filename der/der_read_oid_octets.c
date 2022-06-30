#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__get_obj_id_string(char **, unsigned char *, size_t);
static int	__get_sub_ids(uint32_t *, int *, unsigned char *, size_t);
static void	__sub_ids_to_obj_id_string(char **, uint32_t *, int);

int	der_read_oid_octets(t_iasn *item, unsigned char *derenc, size_t dersize)
{
	char	*obj_name;
	char	*obj_id;
	int		ret;

	if (NULL == item || NULL == derenc)
		return (DER_ERROR(INVALID_INPUT));

	if (dersize == 0)
		return (DER_ERROR(INVALID_DER_ENCODING));

	if (SSL_OK != __get_obj_id_string(&obj_id, derenc, dersize))
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));

	if (NULL == (obj_name = asn_oid_tree_get_name(obj_id)))
	{
		ret = DER_ERROR(UNKNOWN_ASN_OBJECT_ID);
	}
	else
	{
		item->content = obj_name;
		item->bitsize = NBYTES_TO_NBITS(ft_strlen(obj_name));

		ret = SSL_OK;
	}

	SSL_FREE(obj_id);

	return (ret);
}

static int	__get_obj_id_string(
	char **obj_id, unsigned char *derenc, size_t dersize)
{
	uint32_t	sub_ids[dersize/sizeof(uint32_t)+1];
	int			num_sub_ids;

	ft_bzero(sub_ids, sizeof(sub_ids));

	if (SSL_OK != __get_sub_ids(sub_ids, &num_sub_ids, derenc, dersize))
		return (DER_ERROR(INVALID_DER_ENCODING));

	__sub_ids_to_obj_id_string(obj_id, sub_ids, num_sub_ids);

	return (SSL_OK);
}

static int	__get_sub_ids(
	uint32_t *sub_ids, int *num_sub_ids, unsigned char *derenc, size_t dersize)
{
	size_t		sub_id_word_size;
	int			bitcnt;
	int			ix;

	sub_id_word_size = sizeof(*sub_ids);

	ix = 0;
	while (dersize != 0)
	{
		bitcnt = 0;
		// get sub id 7-bit blocks except last one
		while ((dersize-- != 0) && (*derenc & 0x80))
		{
			sub_ids[ix] <<= 7;
			sub_ids[ix] |= *derenc++ & 0x7F;
			bitcnt += 7;
		}

		if (bitcnt > CHAR_BIT * (sub_id_word_size-1))
			return (DER_ERROR(UNSPECIFIED_ERROR));

		// get last sub id 7-bit block
		sub_ids[ix] <<= 7;
		sub_ids[ix] |= *derenc & 0x7F;
		ix++;
	}

	if (ix < 2)
		return (DER_ERROR(UNSPECIFIED_ERROR));

	*num_sub_ids = ix;

	return (SSL_OK);
}

static void	__sub_ids_to_obj_id_string(
	char **obj_id, uint32_t *sub_ids, int num_sub_ids)
{
	char		**sub_id_strings;
	int			num_sub_id_strings;
	char		**sub_id_sptr;
	uint32_t	concat_id;
	int			ix;

	/*
	/	Top level id can only have value of { 0, 1, 2 }
	/	Second level id must have value of { 0, 1, 2, ... 9 }
	/	(... refer to X.660)
	/
	/	First two ids are concatenated into one single id
	/	using following formula:
	/
	/	CONCAT_ID = 40 * ID_0 + ID_1
	*/

	num_sub_id_strings = num_sub_ids + 1;
	SSL_ALLOC(sub_id_strings, num_sub_id_strings * sizeof(char *));
	sub_id_sptr = sub_id_strings;

	ix = 0;

	// get first two ids
	concat_id = sub_ids[ix];
 	ft_sprintf(sub_id_sptr++, "%lu.", sub_ids[ix] / 40);
 	ft_sprintf(sub_id_sptr++, "%lu.", sub_ids[ix] % 40);

	ix += 1;

	// get the rest of ids, except the last one
	while (ix < num_sub_ids-1)
	{
	 	ft_sprintf(sub_id_sptr++, "%lu.", sub_ids[ix]);
		ix++;
	}

	// get the last id
	ft_sprintf(sub_id_sptr + (ix+1), "%lu", sub_ids[ix]);

	*obj_id = ft_2darray_merge_cstr(sub_id_strings, num_sub_id_strings);

	ft_2darray_del((void **)sub_id_strings, num_sub_id_strings);
}
