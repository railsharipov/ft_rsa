#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <json.h>
#include <libft.h>
#include <bnum.h>

static int 	__f_asn1_node_dumper(t_node *node, t_ostring *ostring);

char	*asn1_tree_dump(t_node *asn1_tree)
{
	return (json_dumps_with_f_dumper(asn1_tree, __f_asn1_node_dumper));
}


char	*asn1_tree_dumps(t_node *asn1_tree)
{
	if (NULL == asn1_tree) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}

	return (json_dumps_with_f_dumper(asn1_tree, __f_asn1_node_dumper));
}

size_t	asn1_tree_dumpb(t_node *asn1_tree, char *buf, size_t size)
{
	return (json_dumpb_with_f_dumper(asn1_tree, buf, size, __f_asn1_node_dumper));
}

static int	__f_asn1_node_dumper(t_node *node, t_ostring *ostring)
{
	t_iasn	*asn1_item;
	char	*s;

	switch (node->type)
	{
		case JSON_TYPE_BYTES:
			asn1_item = (t_iasn *)node->content;
			if (NULL == asn1_item) {
				ft_sprintf(&s, "node:<content:<ptr=%p,size=%zu>,iasn:<not-set>>", node->content, node->size);
			} else {
				ft_sprintf(&s, "node:<content:<ptr=%p,size=%zu>,iasn:<desc=%s,tagnum=%#x,tag=%#x,encap=%#x,data=<ptr=%p,size=%zu>>", node->content, node->size, asn1_item->description, asn1_item->tagnum, asn1_item->tag, asn1_item->encapsulates, asn1_item->content, asn1_item->size);
			}
			break;
		default:
			s = json_dumps(node);
	}
	ft_ostr_append(ostring, s, ft_strlen(s));
	SSL_FREE(s);

	return (SSL_OK);
}
