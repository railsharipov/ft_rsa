#include <ft_ssl.h>
#include <ssl_asn.h>
#include <ssl_der.h>

static int	__check_sub_ids(char **sub_ids, int nids)
{
	if ((nids < 2) || (NULL == sub_ids))
	{
		return (SSL_ERROR("invalid object id bytes"));
	}
	while (nids-- > 0)
	{
		if (!ft_str_isdigit(sub_ids[nids]))
		return (SSL_ERROR("invalid sub identifier"));
	}
	return (SSL_OK);
}

static void	__encode_sub_ids(
	char **octets, int *olen, uint32_t *num_ids, int nids)
{
	uint8_t	*p;
	int		ix;
	int		bitcnt;

	*olen = 0;

	SSL_ALLOC(*octets, sizeof(int)*nids + (nids/8+1));
	p = (uint8_t *)*octets;

	ix = 0;
	while (ix < nids)
	{
		bitcnt = CEIL(util_lmbit(num_ids[ix], CHAR_BIT*sizeof(int)), CHAR_BIT-1);
		bitcnt = MAX(CHAR_BIT-1, bitcnt);

		while (bitcnt > 0)
		{
			*p = (num_ids[ix] >> (bitcnt-(CHAR_BIT-1))) & 0x7F;
			bitcnt -= (CHAR_BIT-1);

			if (bitcnt > 0)
			{
				*p |= 0x80;
			}
			p++;
			*olen += 1;
		}
		ix++;
	}
}

static int	__sub_ids(char **octets, int *olen, char *content, int size)
{
	char	**sub_ids;
	int		*num_ids;
	int		nids;
	int		ix;

	sub_ids = ft_strsplit(content, '.');
	nids = ft_2darray_len(sub_ids);

	if (SSL_OK != __check_sub_ids(sub_ids, nids))
	{
		ft_2darray_del(sub_ids, -1);
		return (SSL_ERR);
	}
	SSL_ALLOC(num_ids, sizeof(int)*(nids-1));
	num_ids[0] = 40*ft_atoi(sub_ids[0]) + ft_atoi(sub_ids[1]);

	ix = 0;
	while (NULL != sub_ids[ix+2])
	{
		num_ids[ix+1] = ft_atoi(sub_ids[ix+2]);
		ix++;
	}
	__encode_sub_ids(octets, olen, (uint8_t *)num_ids, nids-1);

	ft_2darray_del(sub_ids, -1);
	SSL_FREE(num_ids);

	return (SSL_OK);
}

int	der_append_oid(t_der *der, void *content, int cont_nbytes)
{
	int		id_nbytes, len_nbytes, enc_nbytes;
	char	*octets;
	int		olen;
	char	*obj_id;
	char	*precontent;

	(void)cont_nbytes;

	if (NULL == der)
	{
		return (SSL_ERROR("invalid input"));
	}
	id_nbytes = 1; // since simple tag expected

	if (NULL == (obj_id = asn_oid_tree_get_oid(content)))
	{
		return (SSL_ERROR("invalid asn object id"));
	}
	if (SSL_OK != __sub_ids(&octets, &olen, obj_id, ft_strlen(obj_id)))
	{
		return (SSL_ERROR("bad asn object id"));
	}
	SSL_ALLOC(precontent, (1+sizeof(olen))+id_nbytes);

	*precontent = ASN_ENCODE_PRIMITIVE | ASN_TAG_UNIVERSAL | ASN_TAG_SIMPLE;
	*precontent |= ASN_TAG_OBJECT_ID;

	len_nbytes = der_append_len(precontent + id_nbytes, olen);
	enc_nbytes = id_nbytes + len_nbytes + olen;

	SSL_REALLOC(der->content, der->size, der->size + enc_nbytes);
	ft_memcpy(der->content + der->size, precontent, id_nbytes + len_nbytes);
	der->size += id_nbytes + len_nbytes;
	ft_memcpy(der->content + der->size, octets, olen);
	der->size += olen;

	SSL_FREE(precontent);

	return (SSL_OK);
}
