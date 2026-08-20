#include <common.h>
#include <logger.h>
#include <asn.h>
#include <json.h>
#include <libft.h>
#include <bnum.h>

#define __ASNQ_BAD_QUERY_ERROR "bad query"

static int 	__f_asn_node_selector(t_node *node, t_node *query, t_node **ret_asn_node);

int asn_tree_query(const char *s, t_node *asn_tree, t_node **ret_asn_node)
{
	if (NULL == s) {
		SSL_LOG(ERROR, __ASNQ_BAD_QUERY_ERROR);
		return (SSL_ERR);
	}

	if (NULL == asn_tree || NULL == ret_asn_node) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	SSL_LOG(TRACE, "running query `%s` on asn tree: %s", s, asn_tree_dump(asn_tree));

	*ret_asn_node = NULL;

	if (SSL_OK != json_query_with_f_selector(s, (t_node *)asn_tree, (t_node **)ret_asn_node, __f_asn_node_selector)) {
		SSL_LOG(ERROR, __ASNQ_BAD_QUERY_ERROR);
		return (SSL_ERR);
	}

	SSL_LOG(TRACE, "query result: %s", asn_tree_dump(*ret_asn_node));

	return (SSL_OK);
}

static int 	__f_asn_node_selector(t_node *node, t_node *query, t_node **ret_node)
{
	if (query->type == JSON_Q_OBJECT_KEY) {
		SSL_LOG(ERROR, "unexpected asn tree query type: %s", json_get_query_type_name(query->type));
		return (JSON_BAD_QUERY);
	}
	else if (query->type == JSON_Q_SELF) {
		*ret_node = node;
		return (JSON_MATCH);
	}
	else if (query->type == JSON_Q_ARRAY_INDEX) {
		t_node	*arr_item;
		t_iasn		*asn_item;
		int			target_idx, idx;

		SSL_LOG(TRACE, "indexing asn node array at: `%s`", query->content);

		asn_item = node->content;
		arr_item = asn_item->content;
		target_idx = ft_atoi(query->content);

		SSL_LOG(TRACE, "array has %d items", ft_lst_size(arr_item));

		idx = 0;
		while (arr_item != NULL) {
			if (idx == target_idx) {
				SSL_LOG(TRACE, "found asn node at index `%d`", target_idx);
				*ret_node = arr_item;
				return (JSON_MATCH);
			}
			arr_item = arr_item->next;
			idx++;
		}
		SSL_LOG(TRACE, "no match found");

		return (JSON_NO_MATCH);
	}
	else {
		return (JSON_BAD_QUERY);
	}
}
