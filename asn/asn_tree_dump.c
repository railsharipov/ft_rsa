#include <common.h>
#include <asn.h>
#include <json.h>
#include <libft/node.h>
#include <libft/ntree.h>
#include <libft/2darray.h>
#include <libft/string.h>
#include <libft/list.h>
#include <bnum.h>

static int 	__f_asn_node_dumper(t_node *node, t_ostring *ostring);

char	*asn_tree_dump(t_node *asn_tree)
{
	return (json_dumps_with_f_dumper(asn_tree, __f_asn_node_dumper));
}


char	*asn_tree_dumps(t_node *asn_tree)
{
	if (NULL == asn_tree) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}

	return (json_dumps_with_f_dumper(asn_tree, __f_asn_node_dumper));
}

size_t	asn_tree_dumpb(t_node *asn_tree, char *buf, size_t size)
{
	return (json_dumpb_with_f_dumper(asn_tree, buf, size, __f_asn_node_dumper));
}

static int	__f_asn_node_dumper(t_node *node, t_ostring *ostring)
{
	t_iasn	*asn_item;
	char	*s;

	switch (node->type)
	{
		case JSON_TYPE_BYTES:
			asn_item = (t_iasn *)node->content;
			if (NULL == asn_item) {
				ft_sprintf(&s, "node:<content:<ptr=%p,size=%zu>,iasn:<not-set>>", node->content, node->size);
			} else {
				ft_sprintf(&s, "node:<content:<ptr=%p,size=%zu>,iasn:<desc=%s,tagnum=%#x,tag=%#x,encap=%#x,data=<ptr=%p,size=%zu>>", node->content, node->size, asn_item->description, asn_item->tagnum, asn_item->tag, asn_item->encapsulates, asn_item->content, asn_item->size);
			}
			break;
		default:
			s = json_dumps(node);
	}
	ft_ostr_append(ostring, s, ft_strlen(s));
	SSL_FREE(s);

	return (SSL_OK);
}
