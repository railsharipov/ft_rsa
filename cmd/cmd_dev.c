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
	const t_json_v2 *jschema = NULL;
	if (JSON_OK != json_v2_parse_file("resources/asn1/schema-v2-pkcs8-KeyContainers.json", &jschema)) {
		SSL_LOG(ERROR, "json parse error");
		return (SSL_ERR);
	}
	if (JSON_OK != json_v2_validate(jschema)) {
		SSL_LOG(ERROR, "invalid json");
		return (SSL_ERR);
	}

	// char *json_dumps = json_v2_pretty_dumps(jschema, 2, 240, true);
	// ft_printf("%s\n", json_dumps);

	if (SSL_OK != asn1_v2_schema_validate(jschema)) {
		SSL_LOG(ERROR, "invalid asn1 schema");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema is valid");
	}

	t_asn_v2_module *asn1_module = NULL;
	if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
		SSL_LOG(ERROR, "asn1 schema parse failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema parse success");
	}

	// char *atype_dumps = asn1_v2_module_pretty_dumps(asn1_module, 1, 320, true);
	// ft_printf("%s\n", atype_dumps);

	t_asn_v2_type *asn1_type = ft_htbl_v2_get(&asn1_module->types, "PrivateKeyInfo");
	assert(NULL != asn1_type);

	t_der_v2_type *der_type = NULL;
	if (SSL_OK != asn1_v2_type_compile(&der_type, asn1_type)) {
		SSL_LOG(ERROR, "asn1 type compile failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 type compile success");
	}

	char *dtype_dumps = der_v2_type_pretty_dumps(der_type, 0, 320, true);
	ft_printf("%s\n", dtype_dumps);

	return (SSL_OK);
}
