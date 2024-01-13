#include <util/json.h>

void	json_del(t_node *node)
{
	if (NULL == node) {
		return ;
	}
	(ft_node_del)(node);
}