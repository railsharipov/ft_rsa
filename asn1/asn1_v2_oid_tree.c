#include <common.h>
#include <logger.h>
#include <asn1_v2.h>
#include <json.h>
#include <json_v2.h>
#include <libft.h>
#include <bnum.h>

t_asn_v2_oid_arc *asn1_v2_oid_arc_create(void)
{
	t_asn_v2_oid_arc *arc = NULL;
	SSL_ALLOC(arc, sizeof(t_asn_v2_oid_arc));
	*arc = (t_asn_v2_oid_arc){0};
	return (arc);
}

void asn1_v2_oid_arc_clear(t_asn_v2_oid_arc *arc)
{
	switch (arc->kind) {
	case ASN_V2_OID_ARC_KIND_NUMERIC: {
		break;
	}
	case ASN_V2_OID_ARC_KIND_NAMED: {
		if (NULL != arc->as.name) SSL_FREE(arc->as.name);
		arc->as.name = NULL;
		break;
	}
	case ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC: {
		if (NULL != arc->as.name) SSL_FREE(arc->as.name);
		arc->as.name = NULL;
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 oid arc type: %d", arc->kind);
	}}
}

void asn1_v2_oid_arc_delete(t_asn_v2_oid_arc *arc)
{
	asn1_v2_oid_arc_clear(arc);
	SSL_FREE(arc);
}

void asn1_v2_oid_arc_copy(const t_asn_v2_oid_arc *src, t_asn_v2_oid_arc *dst)
{
	dst->kind = src->kind;
	switch (src->kind) {
	case ASN_V2_OID_ARC_KIND_NUMERIC: {
		dst->as.number = src->as.number;
		break;
	}
	case ASN_V2_OID_ARC_KIND_NAMED: {
		assert(NULL != src->as.name);
		dst->as.name = ft_strdup(src->as.name);
		break;
	}
	case ASN_V2_OID_ARC_KIND_NAMED_AND_NUMERIC: {
		assert(NULL != src->as.name);
		dst->as.number = src->as.number;
		dst->as.name = ft_strdup(src->as.name);
		break;
	}
	default: {
		SSL_LOG(ERROR, "unknown asn1 oid arc type: %d", src->kind);
	}}
}

typedef struct s_asn1_v2_oid_node {
	t_node_v2 base;
	char *name;
	size_t number;
	t_asn_v2_oid oid;
} t_asn_v2_oid_node;

static t_asn_v2_oid_node *__asn1_v2_oid_node_create(void)
{
	t_asn_v2_oid_node *oid_node = NULL;
	SSL_ALLOC(oid_node, sizeof(t_asn_v2_oid_node));
	*oid_node = (t_asn_v2_oid_node){0};
	return (oid_node);
}

static void __asn1_v2_oid_node_delete(t_asn_v2_oid_node *oid_node)
{
	if (NULL != oid_node->name) SSL_FREE(oid_node->name);
	SSL_FREE(oid_node);
}

static inline void __asn1_v2_oid_node_delete_adapter(t_node_v2 *node) { __asn1_v2_oid_node_delete(container_of(node, t_asn_v2_oid_node, base)); }

static t_asn_v2_oid_node *__asn1_v2_oid_tree_parse(const t_json_v2 *joid_tree, const t_asn_v2_oid_node *parent, size_t depth)
{
	if (joid_tree->kind != JSON_V2_TYPE_OBJECT) return (NULL);
	if (depth > ASN_V2_OID_MAX_NARCS) return (NULL);

	// Each tree node represents an arc of an object id. A path from root node to a specific node represents some object id.
	// For example: object id = { root arc -> arc1 -> arc2 -> arc3 -> arc4 }.

	t_asn_v2_oid_node *oid_node = __asn1_v2_oid_node_create();

	if (NULL != parent) {
		const t_json_v2 *jnumber = NULL;
		int ret = json_v2_query_nonnull("number", joid_tree, &jnumber);
		if (JSON_V2_OK != ret || jnumber->kind != JSON_V2_TYPE_NUMBER) goto label_error;
		if (bnum_cmp_dig(&jnumber->as.number, ASN_V2_OID_MAX_ARC_NUMBER) > 0) goto label_error;
		oid_node->number = bnum_to_dig_u(&jnumber->as.number);

		const t_json_v2 *jname = NULL;
		ret = json_v2_query_nonnull("name", joid_tree, &jname);
		if (JSON_V2_OK != ret || jname->kind != JSON_V2_TYPE_STRING) goto label_error;
		if (NULL == jname->as.cstring) goto label_error;
		oid_node->name = ft_strdup(jname->as.cstring);

		for (size_t i = 0; i < parent->oid.narcs; i++) {
			oid_node->oid.arc_nums[i] = parent->oid.arc_nums[i];
			oid_node->oid.narcs++;
		}
		oid_node->oid.arc_nums[oid_node->oid.narcs++] = oid_node->number;
		// SSL_LOG(WARN, "%s: %s", oid_node->name, asn1_v2_oid_dumps(&oid_node->oid));
	}

	const t_json_v2 *jarcs = NULL;
	if (JSON_V2_OK == json_v2_query_nonnull("arcs", joid_tree, &jarcs)) {
		if (jarcs->kind != JSON_V2_TYPE_ARRAY) goto label_error;
		t_list child_oid_nodes = {0};
		t_list_next next = {0};
		void *content = NULL;
		while (ft_list_next_content(&jarcs->as.list, &next, &content)) {
			const t_json_v2 *jarc = content;
			t_asn_v2_oid_node *child_oid_node = __asn1_v2_oid_tree_parse(jarc, oid_node, depth+1);
			if (NULL == child_oid_node) {
				ft_list_clear(&child_oid_nodes, __asn1_v2_oid_node_delete_adapter);
				goto label_error;
			}
			ft_list_append(&child_oid_nodes, &child_oid_node->base);
		}
		oid_node->base.nodes = child_oid_nodes.first;
	}
	return (oid_node);

label_error:
	SSL_LOG(ERROR, "failed to parse asn1 object id tree");
	__asn1_v2_oid_node_delete(oid_node);
	return (NULL);
}

int asn1_v2_oid_tree_create(const char *filename, t_node_v2 **tree)
{
	if (NULL == filename || NULL == tree) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	const t_json_v2 *joid_tree = NULL;
	if (JSON_OK != json_v2_parse_file(filename, &joid_tree)) {
		SSL_LOG(ERROR, "oid tree json file read error");
		return (SSL_ERR);
	}
	t_asn_v2_oid_node *oid_tree = __asn1_v2_oid_tree_parse(joid_tree, NULL, 0);
	if (NULL == oid_tree) {
		SSL_LOG(ERROR, "oid tree init error");
		return (SSL_ERR);
	}

	*tree = &oid_tree->base;
	return (SSL_OK);
}

void asn1_v2_oid_tree_delete(t_node_v2 *tree)
{
	if (NULL == tree) return;
	ft_ntree_v2_del(tree, __asn1_v2_oid_node_delete_adapter);
}

static bool __asn1_v2_oid_node_find_by_oid(t_node_v2 *node, const void *vctx)
{
	const t_asn_v2_oid *oid = vctx;
	t_asn_v2_oid_node *oid_node = container_of(node, t_asn_v2_oid_node, base);
	if (oid->narcs != oid_node->oid.narcs) return false;
	for (size_t i = 0; i < oid->narcs; i++) {
		if (oid->arc_nums[i] != oid_node->oid.arc_nums[i]) return false;
	}
	return true;
}

const char *asn1_v2_get_oid_name(t_node_v2 *tree, const t_asn_v2_oid *oid)
{
	if (NULL == tree) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (NULL);
	}
	t_node_v2 *node = ft_ntree_v2_dfs(tree, __asn1_v2_oid_node_find_by_oid, oid);
	if (NULL != node) {
		const t_asn_v2_oid_node *oid_node = container_of(node, const t_asn_v2_oid_node, base);
		return (oid_node->name);
	}
	return (NULL);
}

static bool __asn1_v2_oid_node_find_by_name(t_node_v2 *node, const void *vctx)
{
	const char *name = vctx;
	t_asn_v2_oid_node *oid_node = container_of(node, t_asn_v2_oid_node, base);
	if (ft_streq(oid_node->name, name)) return true;
	return false;
}

int asn1_v2_get_oid_by_name(t_node_v2 *tree, const char *name, t_asn_v2_oid *oid)
{
	if (NULL == tree || NULL == name || NULL == oid) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	t_node_v2 *node = ft_ntree_v2_dfs(tree, __asn1_v2_oid_node_find_by_name, name);
	if (NULL != node) {
		const t_asn_v2_oid_node *oid_node = container_of(node, const t_asn_v2_oid_node, base);
		for (size_t i = 0; i < oid_node->oid.narcs; i++) {
			oid->arc_nums[i] = oid_node->oid.arc_nums[i];
			oid->narcs++;
		}
		return (SSL_OK);
	}
	return (SSL_ERR);
}

int asn1_v2_get_arc_number_by_name(t_node_v2 *tree, const char *name, size_t *number)
{
	if (NULL == tree || NULL == name || NULL == number) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}
	t_node_v2 *node = ft_ntree_v2_dfs(tree, __asn1_v2_oid_node_find_by_name, name);
	if (NULL != node) {
		const t_asn_v2_oid_node *oid_node = container_of(node, const t_asn_v2_oid_node, base);
		*number = oid_node->number;
		return (SSL_OK);
	}
	return (SSL_ERR);
}

char *asn1_v2_oid_dumps(const t_asn_v2_oid *oid)
{
	t_ostring ostring = {0};
	ft_ostr_append_cstr(&ostring, "[");
	size_t commas = 0;
	for (size_t i = 0; i < oid->narcs; i++) {
		if (commas++) ft_ostr_append_cstr(&ostring, ",");
		char *nums = ft_itoa_zu(oid->arc_nums[i]);
		ft_ostr_append_cstr(&ostring, nums);
		SSL_FREE(nums);
	}
	ft_ostr_append_cstr(&ostring, "]");
	char *dumps = ft_ostr_to_cstr(&ostring, 0, ostring.size);
	ft_ostr_clear(&ostring);
	return (dumps);
}
