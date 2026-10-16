#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <json.h>
#include <asn1.h>
#include <der.h>
#include <pem.h>
#include <base64.h>
#include <textutil.h>
#include <io.h>
#include <file.h>

#include <libft_v2.h>
#include <json_v2.h>

int	cmd_dev(const t_cmd *cmd)
{
// 	t_node_v2 *tree = NULL;
// 	if (SSL_OK != asn1_v2_oid_tree_create(ASN_V2_OID_TREE_FILE_PATH, &tree)) {
// 		SSL_LOG(ERROR, "asn1 oid tree create error");
// 		return (SSL_ERR);
// 	} else {
// 		SSL_LOG(INFO, "asn1 oid tree create success");
// 	}
//
// 	t_asn_v2_oid oid = {0};
// 	if (SSL_OK != asn1_v2_get_oid_by_name(tree, "rsaEncryption", &oid)) {
// 		SSL_LOG(ERROR, "failed to find object id for provided name");
// 		return (SSL_ERR);
// 	}
// 	char *oid_dumps = asn1_v2_oid_dumps(&oid);
// 	ft_printf("%s\n", oid_dumps);
//
// 	const char *oname = asn1_v2_get_oid_name(tree, &oid);
// 	if (NULL == oname) {
// 		SSL_LOG(ERROR, "expected object name but got null");
// 		return (SSL_ERR);
// 	}
// 	ft_printf("\"%s\"\n", oname);
//
// 	const char *jschema_filename = "resources/asn1/schema-v2-pkcs8-KeyContainers.json";
// 	const char *asn1_type_name = "EncryptedPrivateKeyInfo";
// 	const char *der_filename = "tests/files/keys/pkcs8-encryptedPrivateKeyInfo.der";
//
// 	const char *jschema_filename = "resources/asn1/schema-v2-pkcs1-RSA.json";
// 	const char *asn1_type_name = "RSAPrivateKey";
// 	const char *der_filename = "tests/files/keys/pkcs1-rsaPrivateKey.der";
//
// 	const char *jschema_filename = "resources/asn1/schema-v2-pkcs8-KeyContainers.json";
// 	const char *asn1_type_name = "PrivateKeyInfo";
// 	const char *der_filename = "tests/files/keys/pkcs8-privateKeyInfo.der";

	const char *jschema_filename = "resources/asn1/schema-v2-example.json";
	const char *asn1_type_name = "PrivateKeyInfo";

	const t_json_v2 *jschema = NULL;
	if (JSON_OK != json_v2_parse_file(jschema_filename, &jschema)) {
		SSL_LOG(ERROR, "json parse error");
		return (SSL_ERR);
	}
	if (JSON_OK != json_v2_validate(jschema)) {
		SSL_LOG(ERROR, "invalid json");
		return (SSL_ERR);
	}
	if (SSL_OK != asn1_v2_schema_validate(jschema)) {
		SSL_LOG(ERROR, "invalid asn1 schema");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema is valid");
	}
	// char *json_dumps = json_v2_pretty_dumps(jschema, 2, 240, true);
	// ft_printf("%s\n", json_dumps);

	t_asn_v2_module *asn1_module = NULL;
	if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
		SSL_LOG(ERROR, "asn1 schema parse failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema parse success");
	}
	char *amodule_dumps = asn1_v2_module_pretty_dumps(asn1_module, 1, 320, true);
	ft_printf("%s\n", amodule_dumps);

	const t_asn_v2_type *asn1_type = ft_htbl_v2_get(&asn1_module->type_refs, asn1_type_name);
	assert(NULL != asn1_type);
//
// 	t_der_v2_type *der_type = NULL;
// 	if (SSL_OK != asn1_v2_type_compile(&der_type, asn1_type)) {
// 		SSL_LOG(ERROR, "asn1 type compile failed");
// 		return (SSL_ERR);
// 	} else {
// 		SSL_LOG(INFO, "asn1 type compile success");
// 	}
// 	char *dtype_dumps = der_v2_type_pretty_dumps(der_type, 0, 320, true);
// 	ft_printf("%s\n", dtype_dumps);

// 	t_ostring encoded = {0};
// 	if (SSL_OK != file_read_all(der_filename, &encoded)) {
// 		SSL_LOG(ERROR, "file read error: %s", der_filename);
// 		return (SSL_ERR);
// 	}
//
// 	t_der_v2_value *der_value = NULL;
// 	if (SSL_OK != der_v2_decode(encoded.content, encoded.size, der_type, &der_value)) {
// 		SSL_LOG(ERROR, "der decode failed");
// 		return (SSL_ERR);
// 	} else {
// 		SSL_LOG(INFO, "der decode success");
// 	}
// 	char *dvalue_dumps = der_v2_value_pretty_dumps(der_value, 0, 320, true);
// 	ft_printf("%s\n", dvalue_dumps);

	const char *complex_der_filename = "tests/files/asn1/complex.der";
	t_ostring generic_encoded = {0};
	if (SSL_OK != file_read_all(complex_der_filename, &generic_encoded)) {
		SSL_LOG(ERROR, "file read error: %s", complex_der_filename);
		return (SSL_ERR);
	}

	t_der_v2_value *generic_der_value = NULL;
	if (SSL_OK != der_v2_generic_decode(generic_encoded.content, generic_encoded.size, &generic_der_value)) {
		SSL_LOG(ERROR, "der generic decode failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "der generic decode success");
	}
	char *generic_dvalue_dumps = der_v2_value_pretty_dumps(generic_der_value, 0, 320, true);
	ft_printf("%s\n", generic_dvalue_dumps);

	t_ostring generic_re_encoded = {0};
	if (SSL_OK != der_v2_generic_encode(&generic_re_encoded.content, &generic_re_encoded.size, generic_der_value)) {
		SSL_LOG(ERROR, "der generic encode failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "der generic encode success");
	}
	char *re_encoded_dumps = ft_bytes_dumps_hex(generic_re_encoded.content, generic_re_encoded.size);
	ft_printf("%s\n", re_encoded_dumps);

	return (SSL_OK);
}
