#include <ft_ssl.h>
#include <ssl_json.h>

void	json_del(t_node *node)
{
	if (NULL == node) {
		return ;
	}
	(ft_node_del)(node);
}