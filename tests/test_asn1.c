#include <libft.h>
#include <common.h>
#include <logger.h>
#include "test.h"
#include <json.h>
#include <io.h>
#include <bnum.h>
#include <asn1.h>
#include <file.h>
#include "test.h"

static int	__test_asn1_setup(void);
static void	__test_asn1_cleanup(void);

static int	__test_asn1_tree_create_subjectPublicKeyInfo(void);
static int	__test_asn1_tree_query_subjectPublicKeyInfo(void);
static int	__test_asn1_tree_create_privateKeyInfo(void);
static int	__test_asn1_tree_query_privateKeyInfo(void);

static const char *__schema_pkcs8_subjectPublicKeyInfo_path = "resources/asn1/schema-pkcs8-subjectPublicKeyInfo.json";
static const char *__schema_pkcs1_rsaPublicKey_path = "resources/asn1/schema-pkcs1-rsaPublicKey.json";

static const char *__schema_pkcs8_privateKeyInfo_path = "resources/asn1/schema-pkcs8-privateKeyInfo.json";
static const char *__schema_pkcs1_rsaPrivateKey_path = "resources/asn1/schema-pkcs1-rsaPrivateKey.json";

static t_ostring	__schema_subjectPublicKeyInfo_json;
static t_ostring	__schema_rsaPublicKey_json;
static t_ostring	__schema_pkcs8_privateKeyInfo_json;
static t_ostring	__schema_pkcs1_rsaPrivateKey_json;

int	test_asn(void)
{
	int		ret;

	if (SSL_OK != __test_asn1_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	ret = __test_asn1_tree_create_subjectPublicKeyInfo()
		| __test_asn1_tree_query_subjectPublicKeyInfo();
	ret |= __test_asn1_tree_create_privateKeyInfo()
		| __test_asn1_tree_query_privateKeyInfo();

	__test_asn1_cleanup();

	return (ret);
}

static int	__test_asn1_setup(void)
{
	if (SSL_OK != file_read_all(__schema_pkcs8_subjectPublicKeyInfo_path, &__schema_subjectPublicKeyInfo_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__schema_pkcs1_rsaPublicKey_path, &__schema_rsaPublicKey_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__schema_pkcs8_privateKeyInfo_path, &__schema_pkcs8_privateKeyInfo_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__schema_pkcs1_rsaPrivateKey_path, &__schema_pkcs1_rsaPrivateKey_json)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static void	__test_asn1_cleanup(void)
{
	return ;
}

static int	__test_asn1_tree_create_subjectPublicKeyInfo(void)
{
	t_node	*asn1_node, *result_asn1_node;
	t_iasn	*asn1_item;
	t_node 	*schema_json;
	char 	*schema_json_str;
	int 	ret;

	// subjectPublicKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_subjectPublicKeyInfo_json, 0, __schema_subjectPublicKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);
	TEST_ASSERT(asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "subjectPublicKeyInfo"));

	// subjectPublicKeyInfo -> [ algorithm, subjectPublicKey ]
	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "algorithm"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_SEQUENCE);

	// algorithm -> [ rsaEncryption, parameters ]
	result_asn1_node = NULL;
	ret = asn1_query("[0][0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaEncryption"));

	// algorithm -> parameters
	result_asn1_node = NULL;
	ret = asn1_query("[0][1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "parameters"));

	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "subjectPublicKey"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_BIT_STRING);

	// rsaPublicKey
	schema_json_str = ft_ostr_to_cstr(&__schema_rsaPublicKey_json, 0, __schema_rsaPublicKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);
	TEST_ASSERT(asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaPublicKey"));

	// rsaPublicKey -> [ modulus, publicExponent ]
	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "modulus"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);

	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn1_item->description, "publicExponent"));
	TEST_ASSERT(result_asn1_node->next == NULL);

	asn1_tree_del(asn1_node);

	TEST_PASS();
}

static int __test_asn1_tree_query_subjectPublicKeyInfo(void)
{
	t_node	*asn1_node, *result_asn1_node;
	t_iasn		*asn1_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	// subjectPublicKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_subjectPublicKeyInfo_json, 0, __schema_subjectPublicKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);

	result_asn1_node = NULL;
	ret = asn1_query(".", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "subjectPublicKeyInfo"));

	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "algorithm"));

	result_asn1_node = NULL;
	ret = asn1_query("[0][0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaEncryption"));

	result_asn1_node = NULL;
	ret = asn1_query("[0][1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_NULL);

	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_BIT_STRING);

	// rsaPublicKey
	schema_json_str = ft_ostr_to_cstr(&__schema_rsaPublicKey_json, 0, __schema_rsaPublicKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);

	result_asn1_node = NULL;
	ret = asn1_query(".", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaPublicKey"));

	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn1_item->description, "modulus"));

	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum = ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn1_item->description, "publicExponent"));

	TEST_PASS();
}

static int __test_asn1_tree_create_privateKeyInfo(void)
{
	t_node	*asn1_node, *result_asn1_node;
	t_iasn	*asn1_item;
	t_node 	*schema_json;
	char 	*schema_json_str;
	int 	ret;

	// privateKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs8_privateKeyInfo_json, 0, __schema_pkcs8_privateKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);
	TEST_ASSERT(asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "privateKeyInfo"));

	// privateKeyInfo -> [ version, algorithm, privateKey ]
	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "version"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);

	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "algorithm"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_SEQUENCE);

	// algorithm -> [ rsaEncryption, parameters ]
	result_asn1_node = NULL;
	ret = asn1_query("[1][0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaEncryption"));

	// algorithm -> parameters
	result_asn1_node = NULL;
	ret = asn1_query("[1][1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "parameters"));

	result_asn1_node = NULL;
	ret = asn1_query("[2]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "privateKey"));
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_OCTET_STRING);

	// rsaPrivateKey
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs1_rsaPrivateKey_json, 0, __schema_pkcs1_rsaPrivateKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);
	TEST_ASSERT(asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaPrivateKey"));

	// rsaPrivateKey -> [ version, modulus, publicExponent, privateExponent, prime1, prime2, exponent1, exponent2, coefficient ]
	const char *params[] = { "version", "modulus", "publicExponent", "privateExponent", "prime1", "prime2", "exponent1", "exponent2", "coefficient" };
	for (int i = 0; i < 9; i++) {
		char *query;
		ft_sprintf(&query, "[%d]", i);
		result_asn1_node = NULL;
		ret = asn1_query(query, asn1_node, &result_asn1_node);
		TEST_ASSERT(ret == SSL_OK);
		TEST_ASSERT(result_asn1_node != NULL);
		asn1_item = result_asn1_node->content;
		TEST_ASSERT(asn1_item != NULL);
		TEST_ASSERT(ft_streq(asn1_item->description, params[i]));
		TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);
	}
	asn1_tree_del(asn1_node);

	TEST_PASS();
}

static int __test_asn1_tree_query_privateKeyInfo(void)
{
	t_node	*asn1_node, *result_asn1_node;
	t_iasn		*asn1_item;
	t_node 		*schema_json;
	char 		*schema_json_str;
	int 		ret;

	// privateKeyInfo
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs8_privateKeyInfo_json, 0, __schema_pkcs8_privateKeyInfo_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);

	result_asn1_node = NULL;
	ret = asn1_query(".", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "privateKeyInfo"));

	// privateKeyInfo -> version
	result_asn1_node = NULL;
	ret = asn1_query("[0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);
	TEST_ASSERT(ft_streq(asn1_item->description, "version"));

	// privateKeyInfo -> algorithm
	result_asn1_node = NULL;
	ret = asn1_query("[1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "algorithm"));

	// algorithm -> rsaEncryption
	result_asn1_node = NULL;
	ret = asn1_query("[1][0]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_OBJECT_ID);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaEncryption"));

	// algorithm -> parameters
	result_asn1_node = NULL;
	ret = asn1_query("[1][1]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_NULL);
	TEST_ASSERT(ft_streq(asn1_item->description, "parameters"));

	// privateKeyInfo -> privateKey
	// decode encapsulated privateKey sequence
	result_asn1_node = NULL;
	ret = asn1_query("[2]", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_OCTET_STRING);
	TEST_ASSERT(ft_streq(asn1_item->description, "privateKey"));

	// rsaPrivateKey
	schema_json_str = ft_ostr_to_cstr(&__schema_pkcs1_rsaPrivateKey_json, 0, __schema_pkcs1_rsaPrivateKey_json.size);
	ret = json_parse(schema_json_str, &schema_json);
	TEST_ASSERT(ret == SSL_OK);

	asn1_node = asn1_node_create_from_schema(schema_json);
	TEST_ASSERT(asn1_node != NULL);

	result_asn1_node = NULL;
	ret = asn1_query(".", asn1_node, &result_asn1_node);
	TEST_ASSERT(ret == SSL_OK);
	TEST_ASSERT(result_asn1_node != NULL);
	TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
	asn1_item = result_asn1_node->content;
	TEST_ASSERT(asn1_item != NULL);
	TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_SEQUENCE);
	TEST_ASSERT(ft_streq(asn1_item->description, "rsaPrivateKey"));

	// rsaPrivateKey items
	const char *params[] = { "version", "modulus", "publicExponent", "privateExponent", "prime1", "prime2", "exponent1", "exponent2", "coefficient" };
	for (int i = 0; i < 9; i++) {
		char *query;
		ft_sprintf(&query, "[%d]", i);
		result_asn1_node = NULL;
		ret = asn1_query(query, asn1_node, &result_asn1_node);
		TEST_ASSERT(ret == SSL_OK);
		TEST_ASSERT(result_asn1_node != NULL);
		TEST_ASSERT(result_asn1_node->type == JSON_TYPE_BYTES);
		asn1_item = result_asn1_node->content;
		TEST_ASSERT(asn1_item != NULL);
		TEST_ASSERT(asn1_item->tagnum == ASN_TAGNUM_INT);
		TEST_ASSERT(ft_streq(asn1_item->description, params[i]));
	}

	TEST_PASS();
}
