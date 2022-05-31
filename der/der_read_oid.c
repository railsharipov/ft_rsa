#include <ft_ssl.h>
#include <ssl_error.h>
#include <ssl_asn.h>
#include <ssl_der.h>
#include <bnum.h>

static const int	ASN_TAG			= (ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE);
static const int	ASN_PRIMITIVE	= (ASN_ENCODE_PRIMITIVE | ASN_TAG);

static void	__num_ids_to_ostring(
	char **obj_id, uint32_t *num_ids, int num_idlen)
{
	unsigned char	**sub_ids;
	uint32_t	concat_id;
	int			ix;

	SSL_ALLOC(sub_ids, (num_idlen+1) * sizeof(char *));

	// Top level id can only have value of { 0, 1, 2 }
	// Second level id must have value of { 0, 1, 2, ... 9 }
	// (... refer to X.660)
	//
	// First two ids are concatenated into one single id
	// using following formula:
	//
	// CONCAT_ID = 40 * ID_0 + ID_1

	concat_id = num_ids[0];
 	ft_sprintf((char **)sub_ids + 0, "%lu.", num_ids[0] / 40);
 	ft_sprintf((char **)sub_ids + 1, "%lu.", num_ids[0] % 40);

	ix = 1;
	while (ix < num_idlen-1) // skip last one
	{
	 	ft_sprintf((char **)sub_ids + (ix+1), "%lu.", num_ids[ix]);
		ix++;
	}
	if (ix < num_idlen) // get last one
	{
		ft_sprintf((char **)sub_ids + (ix+1), "%lu", num_ids[ix]);
	}
	*obj_id = ft_2darray_merge_cstr((char **)sub_ids, num_idlen+1);

	ix = 0;
	while (ix < num_idlen+1)
	{
		free(sub_ids[ix++]);
	}
	SSL_FREE(sub_ids);
}

static int	__sub_ids(char **obj_id, unsigned char *octets, size_t olen)
{
	uint32_t	num_ids[olen];
	int			ix;
	int			iy;
	int			bits;

	ft_bzero(num_ids, olen*sizeof(int));

	ix = 0;
	iy = 0;
	while (ix < olen)
	{
		bits = 0;
		// get sub id 7-bit blocks except last one
		while ((ix < olen) && (octets[ix] & 0x80))
		{
			num_ids[iy] <<= (CHAR_BIT-1);
			num_ids[iy] |= octets[ix] & 0x7F;
			bits += (CHAR_BIT-1);
			ix++;
		}
		if ((bits > CHAR_BIT*(sizeof(int)-1)) || (ix >= olen))
		{
			return (DER_ERROR(UNSPECIFIED_ERROR));
		}
		// get last sub id 7-bit block
		num_ids[iy] <<= (CHAR_BIT-1);
		num_ids[iy] |= octets[ix] & 0x7F;
		iy++;
		ix++;
	}
	if (iy < 2)
	{
		return (DER_ERROR(UNSPECIFIED_ERROR));
	}
	__num_ids_to_ostring(obj_id, num_ids, iy);

	return (SSL_OK);
}

int	der_read_oid(t_iasn *item, char **derenc, size_t *dersize)
{
	unsigned char	*octets;
	size_t	osize;
	size_t	olen;
	char	*obj_name;
	char	*obj_id;

	SSL_CHECK(NULL != item);
	SSL_CHECK((NULL != derenc) && (NULL != *derenc));
	SSL_CHECK(NULL != dersize);

	octets = (unsigned char *)(*derenc);
	osize = *dersize;

	if ((ASN_PRIMITIVE | ASN_TAG_OBJECT_ID) != *octets)
	{
		return (DER_ERROR(INVALID_ASN_TYPE_TAG));
	}
	octets++;
	osize--;

	if (SSL_OK != der_read_len(&octets, &osize, &olen))
	{
		return (DER_ERROR(INVALID_ASN_LEN_TAG));
	}
	if (olen > osize)
	{
		return (DER_ERROR(INVALID_ASN_LEN_TAG));
	}
	if (SSL_OK != __sub_ids(&obj_id, octets, olen))
	{
		return (DER_ERROR(INVALID_ASN_OBJECT_ID));
	}
	obj_name = asn_oid_tree_get_name(obj_id);
	SSL_FREE(obj_id);

	if (NULL == obj_name)
	{
		return (DER_ERROR(UNKNOWN_ASN_OBJECT_ID));
	}
	item->content = obj_name;
	item->size = ft_strlen(obj_name);

	*derenc = (char *)(octets) + olen;
	*dersize = osize - olen;

	return (SSL_OK);
}
