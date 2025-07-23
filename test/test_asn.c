#include <libft/list.h>
#include <libft/tuple.h>
#include <common.h>
#include <test.h>
#include <json.h>
#include <io.h>
#include <bnum.h>
#include <asn.h>
#include <test.h>

static int	__test_asn_setup(void);
static void	__test_asn_cleanup(void);

static int	__test_asn_tree_create(void);
static int	__test_asn_tree_query(void);

static const char *__scheme_x509_public_key_json_file_path = "test/testfiles/asn/scheme-x509-public-key.json";

static t_ostring	__scheme_x509_public_key_json;

int	test_asn(void)
{
	int		ret;

	if (SSL_OK != __test_asn_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	ret = __test_asn_tree_create()
		| __test_asn_tree_query();

	__test_asn_cleanup();

	return (ret);
}

static int	__test_asn_setup(void)
{
	if (SSL_OK != test_get_file_content(__scheme_x509_public_key_json_file_path, &__scheme_x509_public_key_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_asn_cleanup(void)
{
	return ;
}

static int	__test_asn_tree_create(void)
{
	t_node	*asn_tree, *asn_node_list, *asn_node;
	t_iasn		*asn_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	schema_json_str = ft_ostr_to_cstr(&__scheme_x509_public_key_json, 0, __scheme_x509_public_key_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	

	// publicKey -> [ algorithmId, bitstring ]
	asn_item = asn_tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "publicKey"));

	asn_node = (t_node *)asn_item->content;
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithmId"));

	asn_node = asn_node->next;
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_BIT_STRING);
	TEST_ASSERT(ft_streq(asn_item->description, "bitstring"));
	TEST_ASSERT(asn_node->next == NULL);

	// publicKey -> [ algorithmId, bitstring -> [ modulus, publicExponent ] ]
	asn_node = asn_item->content;
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, ""));

	asn_node = asn_item->content;
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "modulus"));

	asn_node = asn_node->next;
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "publicExponent"));
	TEST_ASSERT(asn_node->next == NULL);

	asn_tree_del(asn_tree);

	TEST_PASS();
}

static int __test_asn_tree_query(void)
{
	t_node	*asn_tree, *asn_node;
	t_iasn		*asn_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	schema_json_str = ft_ostr_to_cstr(&__scheme_x509_public_key_json, 0, __scheme_x509_public_key_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);

	asn_node = NULL;
	ret = asn_tree_query(".", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "publicKey"));

	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithmId"));

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "bitstring"));

	asn_node = NULL;
	ret = asn_tree_query("[2]", asn_tree, &asn_node);
	TEST_ASSERT(ret != SSL_OK);
	TEST_ASSERT(asn_node == NULL);

	asn_node = NULL;
	ret = asn_tree_query("[0][0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaEncryption"));

	asn_node = NULL;
	ret = asn_tree_query("[0][1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, ""));

	TEST_PASS();
}