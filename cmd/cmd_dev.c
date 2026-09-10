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

int	cmd_dev(const t_cmd *cmd)
{
	t_node *json = NULL;
	if (JSON_OK != json_parse_file("resources/asn1/schema-v2-example.json", &json)) {
		SSL_LOG(ERROR, "json parse error");
		return (SSL_ERR);
	}
	// ft_printf("%s\n", json_dumps(json));

	if (SSL_OK != asn1_v2_schema_validate(json)) {
		SSL_LOG(ERROR, "invalid asn1 schema");
		return (SSL_ERR);
	} else {
		SSL_LOG(INFO, "asn1 schema is valid");
	}

	return (SSL_OK);
}
