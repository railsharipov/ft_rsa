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
	t_json_v2 *jschema = NULL;
	if (JSON_OK != json_v2_parse_file("resources/asn1/schema-v2-example.json", &jschema)) {
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

	t_asn_v2_module *asn1_module = NULL;
	if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
		SSL_LOG(ERROR, "asn1 schema parse failed");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema parse success");
	}

	char *dumps = asn1_v2_module_dumps(asn1_module);

	ft_printf("%s\n", dumps);

	return (SSL_OK);
}
