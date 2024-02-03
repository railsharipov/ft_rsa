#include <ssl/ssl.h>
#include <ssl/asn.h>
#include <ssl/der.h>
#include <libft/2darray.h>

static int	__get_obj_id_string(char **, unsigned char *, size_t);
static int	__get_sub_ids(uint32_t *, int *, unsigned char *, size_t);
static void	__sub_ids_to_obj_id_string(char **, uint32_t *, int);

int	der_decode_oid(t_ostring *osbuf, unsigned char *enc, size_t size)
{
	char	*obj_name;
	char	*obj_id;
	int		ret;

	if (NULL == osbuf || NULL == enc)
		return (DER_ERROR(INVALID_INPUT_ERROR));

	if (size == 0)
		return (DER_ERROR("invalid der encoding"));

	if (SSL_OK != __get_obj_id_string(&obj_id, enc, size))
		return (DER_ERROR("invalid asn object id"));

	if (NULL == (obj_name = asn_oid_tree_get_name(obj_id)))
	{
		ret = DER_ERROR("unknown asn object id");
	}
	else
	{
		osbuf->content = obj_name;
		osbuf->size = ft_strlen(obj_name);

		ret = SSL_OK;
	}

	SSL_FREE(obj_id);

	return (ret);
}

static int	__get_obj_id_string(
	char **obj_id, unsigned char *nec, size_t size)
{
	uint32_t	sub_ids[size+1];
	int			num_sub_ids;

	ft_bzero(sub_ids, sizeof(sub_ids));

	if (SSL_OK != __get_sub_ids(sub_ids, &num_sub_ids, nec, size))
		return (DER_ERROR("invalid der encoding"));

	__sub_ids_to_obj_id_string(obj_id, sub_ids, num_sub_ids);

	return (SSL_OK);
}

static int	__get_sub_ids(
	uint32_t *sub_ids, int *num_sub_ids, unsigned char *nec, size_t size)
{
	int	ix;

	ix = 0;
	while (size != 0)
	{
		// get 7-bit blocks, except the last one
		while (size != 0 && *nec & 0x80)
		{
			sub_ids[ix] <<= 7;
			sub_ids[ix] |= *nec++ & 0x7F;
			size--;
		}

		if ((*nec & 0x80) != 0)
			return (DER_ERROR("invalid der encoding"));

		// get the last block
		sub_ids[ix] <<= 7;
		sub_ids[ix] |= *nec++ & 0x7F;
		size--;

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
	ft_sprintf(sub_id_sptr, "%lu", sub_ids[ix]);

	*obj_id = ft_2darray_strjoin(sub_id_strings, num_sub_id_strings, "");

	ft_2darray_del((void **)sub_id_strings, num_sub_id_strings);
}
