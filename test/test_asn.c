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

static int	__test_asn_tree_create_subjectPublicKeyInfo(void);
static int	__test_asn_tree_query_subjectPublicKeyInfo(void);
static int	__test_asn_tree_create_privateKeyInfo(void);
static int	__test_asn_tree_query_privateKeyInfo(void);

static const char *__schema_pkcs8_subjectPublicKeyInfo_path = "resources/asn/schema-pkcs8-subjectPublicKeyInfo.json";
static const char *__schema_pkcs1_rsaPublicKey_path = "resources/asn/schema-pkcs1-rsaPublicKey.json";

static const char *__schema_pkcs8_privateKeyInfo_path = "resources/asn/schema-pkcs8-privateKeyInfo.json";
static const char *__schema_pkcs1_rsaPrivateKey_path = "resources/asn/schema-pkcs1-rsaPrivateKey.json";

static t_ostring	__schema_subjectPublicKeyInfo_json;
static t_ostring	__schema_rsaPublicKey_json;
static t_ostring	__schema_pkcs8_privateKeyInfo_json;
static t_ostring	__schema_pkcs1_rsaPrivateKey_json;

int	test_asn(void)
{
	int		ret;

	if (SSL_OK != __test_asn_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	ret = __test_asn_tree_create_subjectPublicKeyInfo()
		| __test_asn_tree_query_subjectPublicKeyInfo();
	ret |= __test_asn_tree_create_privateKeyInfo()
		| __test_asn_tree_query_privateKeyInfo();

	__test_asn_cleanup();

	return (ret);
}

static int	__test_asn_setup(void)
{
	if (SSL_OK != test_read_file(__schema_pkcs8_subjectPublicKeyInfo_path, &__schema_subjectPublicKeyInfo_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__schema_pkcs1_rsaPublicKey_path, &__schema_rsaPublicKey_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__schema_pkcs8_privateKeyInfo_path, &__schema_pkcs8_privateKeyInfo_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != test_read_file(__schema_pkcs1_rsaPrivateKey_path, &__schema_pkcs1_rsaPrivateKey_json)) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_asn_cleanup(void)
{
	return ;
}

static int	__test_asn_tree_create_subjectPublicKeyInfo(void)
{
	t_node	*asn_tree, *asn_node;
	t_iasn	*asn_item;
	t_node 	*schema_json;
	char 	*schema_json_str;
	int 	ret;

	// subjectPublicKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_subjectPublicKeyInfo_json, 0, __schema_subjectPublicKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "subjectPublicKeyInfo"));

	// subjectPublicKeyInfo -> [ algorithm, subjectPublicKey ]
	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithm"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);

	// algorithm -> [ rsaEncryption, parameters ]
	asn_node = NULL;
	ret = asn_tree_query("[0][0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaEncryption"));

	// algorithm -> parameters
	asn_node = NULL;
	ret = asn_tree_query("[0][1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "parameters"));

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "subjectPublicKey"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_BIT_STRING);
	TEST_ASSERT(asn_item->encapsulates == SSL_TRUE);

	// rsaPublicKey
	schema_json_str = ft_ostr_to_cstr(&__schema_rsaPublicKey_json, 0, __schema_rsaPublicKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaPublicKey"));

	// rsaPublicKey -> [ modulus, publicExponent ]
	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "modulus"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "publicExponent"));
	TEST_ASSERT(asn_node->next == NULL);

	asn_tree_del(asn_tree);

	TEST_PASS();
}

static int __test_asn_tree_query_subjectPublicKeyInfo(void)
{
	t_node	*asn_tree, *asn_node;
	t_iasn		*asn_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	// subjectPublicKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_subjectPublicKeyInfo_json, 0, __schema_subjectPublicKeyInfo_json.size);
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
	TEST_ASSERT(ft_streq(asn_item->description, "subjectPublicKeyInfo"));

	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithm"));

	asn_node = NULL;
	ret = asn_tree_query("[0][0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaEncryption"));

	asn_node = NULL;
	ret = asn_tree_query("[0][1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_NULL);

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_BIT_STRING);
	TEST_ASSERT(asn_item->encapsulates == SSL_TRUE);

	// rsaPublicKey
	schema_json_str = ft_ostr_to_cstr(&__schema_rsaPublicKey_json, 0, __schema_rsaPublicKey_json.size);
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
	TEST_ASSERT(ft_streq(asn_item->description, "rsaPublicKey"));

	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "modulus"));

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum = ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "publicExponent"));

	TEST_PASS();
}

static int __test_asn_tree_create_privateKeyInfo(void)
{
	t_node	*asn_tree, *asn_node;
	t_iasn	*asn_item;
	t_node 	*schema_json;
	char 	*schema_json_str;
	int 	ret;

	// privateKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs8_privateKeyInfo_json, 0, __schema_pkcs8_privateKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "privateKeyInfo"));

	// privateKeyInfo -> [ version, algorithm, privateKey ]
	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "version"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);

	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithm"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);

	// algorithm -> [ rsaEncryption, parameters ]
	asn_node = NULL;
	ret = asn_tree_query("[1][0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaEncryption"));

	// algorithm -> parameters
	asn_node = NULL;
	ret = asn_tree_query("[1][1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "parameters"));

	asn_node = NULL;
	ret = asn_tree_query("[2]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "privateKey"));
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	TEST_ASSERT(asn_item->encapsulates == SSL_TRUE);

	// rsaPrivateKey
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs1_rsaPrivateKey_json, 0, __schema_pkcs1_rsaPrivateKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn_tree = asn_tree_create(schema_json);
	TEST_ASSERT(asn_tree != NULL);
	TEST_ASSERT(asn_tree->type == JSON_TYPE_BYTES);
	asn_item = asn_tree->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaPrivateKey"));

	// rsaPrivateKey -> [ version, modulus, publicExponent, privateExponent, prime1, prime2, exponent1, exponent2, coefficient ]
	const char *params[] = { "version", "modulus", "publicExponent", "privateExponent", "prime1", "prime2", "exponent1", "exponent2", "coefficient" };
	for (int i = 0; i < 9; i++) {
		char *query;
		ft_sprintf(&query, "[%d]", i);
		asn_node = NULL;
		ret = asn_tree_query(query, asn_tree, &asn_node);
		TEST_ASSERT(ret == SSL_OK);
		TEST_ASSERT(asn_node != NULL);
		asn_item = asn_node->content;
		TEST_ASSERT(asn_item != NULL);
		TEST_ASSERT(ft_streq(asn_item->description, params[i]));
		TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	}
	asn_tree_del(asn_tree);

	TEST_PASS();
}

static int __test_asn_tree_query_privateKeyInfo(void)
{
	t_node	*asn_tree, *asn_node;
	t_iasn		*asn_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	// privateKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs8_privateKeyInfo_json, 0, __schema_pkcs8_privateKeyInfo_json.size);
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
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "privateKeyInfo"));

	// privateKeyInfo -> version
	asn_node = NULL;
	ret = asn_tree_query("[0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn_item->description, "version"));

	// privateKeyInfo -> algorithm
	asn_node = NULL;
	ret = asn_tree_query("[1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "algorithm"));

	// algorithm -> rsaEncryption
	asn_node = NULL;
	ret = asn_tree_query("[1][0]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaEncryption"));

	// algorithm -> parameters
	asn_node = NULL;
	ret = asn_tree_query("[1][1]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn_item->description, "parameters"));

	// privateKeyInfo -> privateKey
	// decode encapsulated privateKey sequence
	asn_node = NULL;
	ret = asn_tree_query("[2]", asn_tree, &asn_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(asn_node != NULL);
	TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
	asn_item = asn_node->content;
	TEST_ASSERT(asn_item != NULL);
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	TEST_ASSERT(ft_streq(asn_item->description, "privateKey"));
	TEST_ASSERT(asn_item->encapsulates == SSL_TRUE);

	// rsaPrivateKey
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs1_rsaPrivateKey_json, 0, __schema_pkcs1_rsaPrivateKey_json.size);
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
	TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn_item->description, "rsaPrivateKey"));

	// rsaPrivateKey items
	const char *params[] = { "version", "modulus", "publicExponent", "privateExponent", "prime1", "prime2", "exponent1", "exponent2", "coefficient" };
	for (int i = 0; i < 9; i++) {
		char *query;
		ft_sprintf(&query, "[%d]", i);
		asn_node = NULL;
		ret = asn_tree_query(query, asn_tree, &asn_node);
		TEST_ASSERT(ret == SSL_OK);
		TEST_ASSERT(asn_node != NULL);
		TEST_ASSERT(asn_node->type == JSON_TYPE_BYTES);
		asn_item = asn_node->content;
		TEST_ASSERT(asn_item != NULL);
		TEST_ASSERT(asn_item->tagnum == ASN_TAGNUM_INT);
		TEST_ASSERT(ft_streq(asn_item->description, params[i]));
	}

	TEST_PASS();
}