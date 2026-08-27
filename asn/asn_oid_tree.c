#include <common.h>
#include <logger.h>
#include <asn1.h>
#include <map.h>
#include <libft.h>

static t_node	*__oid_tree;

static void	__oid_tree_init(void);
static void	__oid_tree_del(void);
static int	__f_init(t_node *node, const void *farg);
static int	__f_find_name(t_node *node, const void *farg);
static int	__f_find_oid(t_node *node, const void *farg);

char	*asn_oid_tree_get_name(const char *oid)
{
	t_node	*node;
	char	*name;

	name = NULL;

	if (NULL == oid) {
		return (NULL);
	}
	__oid_tree_init();

	if (ft_ntree_bfs(&node, __oid_tree, oid, __f_find_name) != 1) {
		SSL_LOG(ERROR, "failed to find name for oid: %s", oid);
		return (NULL);
	}
	if (NULL == node) {
		SSL_LOG(ERROR, "failed to find name for oid: %s", oid);
		return (NULL);
	}
	name = ft_strdup(node->content);
	__oid_tree_del();

	return (name);
}

char	*asn_oid_tree_get_oid(const char *name)
{
	t_node	*node;
	char	*oid;

	oid = NULL;

	if (NULL == name) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	__oid_tree_init();

	if (ft_ntree_bfs(&node, __oid_tree, name, __f_find_oid) != 1) {
		SSL_LOG(ERROR, "failed to find oid for name: %s", name);
		return (NULL);
	}
	if (NULL == node) {
		SSL_LOG(ERROR, "failed to find oid for name: %s", name);
		return (NULL);
	}
	oid = ft_strdup(node->key);
	__oid_tree_del();

	return (oid);
}

static void	__oid_tree_del(void)
{
	ft_ntree_del(__oid_tree, NULL);
	__oid_tree = NULL;
}

static int	__f_init(t_node *node, const void *farg)
{
	char	**keys;

	(void)farg;

	if (NULL == node) {
		return (0);
	}
	keys = ft_strsplit(node->key, ':');
	SSL_FREE(node->key);

	if (NULL == keys) {
		return (-1);
	}
	node->key = keys[0];
	node->content = keys[1];
	SSL_FREE(keys);

	return (0);
}

static void	__oid_tree_init(void)
{
	__oid_tree = ft_ntree_construct(MAP_ASN_OID_TREE);
	ft_ntree_bfs(NULL, __oid_tree, NULL, __f_init);
}

static int	__f_find_oid(t_node *node, const void *farg)
{
	if (NULL == node) {
		return (0);
	}
	SSL_LOG(TRACE, "checking oid=%s with farg=%s", node->content, farg);
	if (ft_streq(node->content, farg)) {
		SSL_LOG(TRACE, "found oid=%s with farg=%s", node->content, farg);
		return (1);
	}
	return (0);
}

static int	__f_find_name(t_node *node, const void *farg)
{
	if (NULL == node) {
		return (0);
	}
	SSL_LOG(TRACE, "checking name=%s with farg=%s", node->key, farg);
	if (ft_streq(node->key, farg)) {
		SSL_LOG(TRACE, "found name=%s with farg=%s", node->key, farg);
		return (1);
	}
	return (0);
}
