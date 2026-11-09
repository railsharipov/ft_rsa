#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <json.h>
#include <asn1_v2.h>
#include <der.h>
#include <pem.h>
#include <base64.h>
#include <textutil.h>
#include <io.h>
#include <file.h>

#include <libft_v2.h>
#include <json_v2.h>

static const t_json_v2 *__must_parse_json_parameters(const char *params)
{
	const t_json_v2 *jparams = NULL;
	if (JSON_OK != json_v2_parse(params, &jparams)) {
		SSL_LOG(ERROR, "bad params format");
		abort();
	}
	return (jparams);
}

static const char *__must_get_json_parameter(const t_json_v2 *jparams, const char *key)
{
	const t_json_v2 *jtype = NULL;
	if (JSON_V2_OK != json_v2_query_nonnull(key, jparams, &jtype)) {
		SSL_LOG(ERROR, "params: missing required key: `%s`", key);
		abort();
	}
	if (jtype->kind != JSON_V2_TYPE_STRING) {
		SSL_LOG(ERROR, "params: `%s`: expected string but got %s", key, json_v2_get_type_name(jtype->kind));
		abort();
	}
	return (jtype->as.cstring);
}

int	cmd_dev(const t_cmd *cmd)
{
	if (!ft_htbl_has(cmd->opts, "-m")) {
		SSL_LOG(ERROR, "dev module name is not specified");
		return (SSL_ERR);
	}
	const char *module_name = ft_htbl_get(cmd->opts, "-m");

	/* ASN1 parser. */

	if (ft_streq(module_name, "asn1-parser")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *asn1_filename = __must_get_json_parameter(jparams, "asn1_file");

		t_ostring asn1_text = {0};
		if (SSL_OK != file_read_all(asn1_filename, &asn1_text)) {
			SSL_LOG(ERROR, "%s: file read error: %s", asn1_filename);
			return (SSL_ERR);
		}

		t_asn_v2_ast *asn1_ast = NULL;
		if (SSL_OK != asn1_v2_parse((char *)asn1_text.content, asn1_text.size, &asn1_ast)) {
			SSL_LOG(ERROR, "%s: asn1 parse failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: asn1 parse success", module_name);
		}

		char *dumps = asn1_v2_ast_dumps(asn1_ast);
		ft_printf("%s\n", dumps);
	}

	/* ASN1 object id tree builder. */

	else if (ft_streq(module_name, "asn1-oid-tree")) {
		t_node_v2 *tree = NULL;
		if (SSL_OK != asn1_v2_oid_tree_create(ASN_V2_OID_TREE_FILE_PATH, &tree)) {
			SSL_LOG(ERROR, "%s: asn1 oid tree create error", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: asn1 oid tree create success", module_name);
		}

		t_asn_v2_oid oid = {0};
		if (SSL_OK != asn1_v2_get_oid_by_name(tree, "rsaEncryption", &oid)) {
			SSL_LOG(ERROR, "%s: failed to find object id for provided name", module_name);
			return (SSL_ERR);
		}
		if (ft_htbl_has(cmd->opts, "-v")) {
			char *oid_dumps = asn1_v2_oid_dumps(&oid);
			ft_printf("%s\n", oid_dumps);
		}

		const char *oname = asn1_v2_get_oid_name(tree, &oid);
		if (NULL == oname) {
			SSL_LOG(ERROR, "%s: expected object name but got null", module_name);
			return (SSL_ERR);
		}
		if (ft_htbl_has(cmd->opts, "-v")) {
			ft_printf("\"%s\"\n", oname);
		}
	}

	/* ASN1 schema parser. */

	else if (ft_streq(module_name, "asn1-schema-parser")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *jschema_filename = __must_get_json_parameter(jparams, "schema_file");

		const t_json_v2 *jschema = NULL;
		if (JSON_OK != json_v2_parse_file(jschema_filename, &jschema)) {
			SSL_LOG(ERROR, "%s: json parse error", module_name);
			return (SSL_ERR);
		}

		t_asn_v2_module *asn1_module = NULL;
		if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
			SSL_LOG(ERROR, "%s: asn1 schema parse failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: asn1 schema parse success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *amodule_dumps = asn1_v2_module_pretty_dumps(asn1_module, true);
			ft_printf("%s\n", amodule_dumps);
		}

		const char *asn1_type_name = __must_get_json_parameter(jparams, "type");
		const t_asn_v2_type *asn1_type = asn1_v2_module_get_type(asn1_module, asn1_type_name);
		if (NULL == asn1_type) {
			SSL_LOG(ERROR, "%s: no such asn1 type in the parsed schema: `%s`", module_name, asn1_type_name);
			return (SSL_ERR);
		}

		t_der_v2_type *der_type = NULL;
		if (SSL_OK != der_v2_type_compile(&der_type, asn1_type)) {
			SSL_LOG(ERROR, "%s: asn1 type compile failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: asn1 type compile success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *dtype_dumps = der_v2_type_pretty_dumps(der_type, true);
			ft_printf("%s\n", dtype_dumps);
		}
	}

	/* DER decoder type-assisted. */

	else if (ft_streq(module_name, "der-decoder")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *jschema_filename = __must_get_json_parameter(jparams, "schema_file");
		const char *der_filename = __must_get_json_parameter(jparams, "der_file");

		const t_json_v2 *jschema = NULL;
		if (JSON_OK != json_v2_parse_file(jschema_filename, &jschema)) {
			SSL_LOG(ERROR, "%s: json parse error", module_name);
			return (SSL_ERR);
		}
		t_asn_v2_module *asn1_module = NULL;
		if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
			SSL_LOG(ERROR, "%s: asn1 schema parse failed", module_name);
			return (SSL_ERR);
		}

		const char *asn1_type_name = __must_get_json_parameter(jparams, "type");
		const t_asn_v2_type *asn1_type = asn1_v2_module_get_type(asn1_module, asn1_type_name);
		if (NULL == asn1_type) {
			SSL_LOG(ERROR, "%s: no such asn1 type in the parsed schema: `%s`", module_name, asn1_type_name);
			return (SSL_ERR);
		}
		t_der_v2_type *der_type = NULL;
		if (SSL_OK != der_v2_type_compile(&der_type, asn1_type)) {
			SSL_LOG(ERROR, "%s: asn1 type compile failed", module_name);
			return (SSL_ERR);
		}

		t_ostring encoded = {0};
		if (SSL_OK != file_read_all(der_filename, &encoded)) {
			SSL_LOG(ERROR, "%s: file read error: %s", module_name, der_filename);
			return (SSL_ERR);
		}

		t_der_v2_value *der_value = NULL;
		if (SSL_OK != der_v2_decode(encoded.content, encoded.size, der_type, &der_value)) {
			SSL_LOG(ERROR, "%s: der decode failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: der decode success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *dvalue_dumps = der_v2_value_pretty_dumps(der_value, true);
			ft_printf("%s\n", dvalue_dumps);
		}
	}

	/* DER decoder generic. */

	else if (ft_streq(module_name, "der-generic-decoder")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *der_filename = __must_get_json_parameter(jparams, "der_file");

		t_ostring encoded = {0};
		if (SSL_OK != file_read_all(der_filename, &encoded)) {
			SSL_LOG(ERROR, "%s: file read error: %s", module_name, der_filename);
			return (SSL_ERR);
		}

		t_der_v2_value *der_value = NULL;
		if (SSL_OK != der_v2_generic_decode(encoded.content, encoded.size, &der_value)) {
			SSL_LOG(ERROR, "%s: der generic decode failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: der generic decode success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *dvalue_dumps = der_v2_value_pretty_dumps(der_value, true);
			ft_printf("%s\n", dvalue_dumps);
		}
	}

	/* DER encoder type-assisted. */

	else if (ft_streq(module_name, "der-encoder")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *jschema_filename = __must_get_json_parameter(jparams, "schema_file");

		const t_json_v2 *jschema = NULL;
		if (JSON_OK != json_v2_parse_file(jschema_filename, &jschema)) {
			SSL_LOG(ERROR, "%s: json parse error", module_name);
			return (SSL_ERR);
		}
		t_asn_v2_module *asn1_module = NULL;
		if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
			SSL_LOG(ERROR, "%s: asn1 schema parse failed", module_name);
			return (SSL_ERR);
		}

		const char *asn1_type_name = __must_get_json_parameter(jparams, "type");
		const char *asn1_value_name = __must_get_json_parameter(jparams, "value");

		const t_asn_v2_type *asn1_type = asn1_v2_module_get_type(asn1_module, asn1_type_name);
		const t_asn_v2_value *asn1_value = asn1_v2_module_get_value(asn1_module, asn1_value_name);

		if (NULL == asn1_type) {
			SSL_LOG(ERROR, "%s: no such asn1 type in the parsed schema: `%s`", module_name, asn1_type_name);
			return (SSL_ERR);
		}
		if (NULL == asn1_value) {
			SSL_LOG(ERROR, "%s: no such asn1 value in the parsed schema: `%s`", module_name, asn1_value_name);
			return (SSL_ERR);
		}

		t_der_v2_type *der_type = NULL;
		if (SSL_OK != der_v2_type_compile(&der_type, asn1_type)) {
			SSL_LOG(ERROR, "%s: asn1 type compile failed", module_name);
			return (SSL_ERR);
		}
		t_der_v2_value *der_value = NULL;
		if (SSL_OK != der_v2_value_compile(&der_value, asn1_value)) {
			SSL_LOG(ERROR, "%s: asn1 value compile failed", module_name);
			return (SSL_ERR);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *dtype_dumps = der_v2_type_pretty_dumps(der_type, true);
			SSL_LOG(INFO, "%s: der type:\n%s", module_name, dtype_dumps);
			char *dvalue_dumps = der_v2_value_pretty_dumps(der_value, true);
			SSL_LOG(INFO, "%s: der value:\n%s", module_name, dvalue_dumps);
		}

		t_ostring encoded = {0};
		if (SSL_OK != der_v2_encode(&encoded.content, &encoded.size, der_type, der_value)) {
			SSL_LOG(ERROR, "%s: der encode failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: der encode success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *encoded_dumps = ft_bytes_dumps_hex(encoded.content, encoded.size);
			SSL_LOG(INFO, "%s: der encoding: %s", module_name, encoded_dumps);
		}
	}

	/* DER encoder generic. */

	else if (ft_streq(module_name, "der-generic-encoder")) {
		if (!ft_htbl_has(cmd->opts, "--params")) {
			SSL_LOG(ERROR, "%s: no parameter spec provided", module_name);
			return (SSL_ERR);
		}
		const t_json_v2 *jparams = __must_parse_json_parameters(ft_htbl_get(cmd->opts, "--params"));
		const char *jschema_filename = __must_get_json_parameter(jparams, "schema_file");

		const t_json_v2 *jschema = NULL;
		if (JSON_OK != json_v2_parse_file(jschema_filename, &jschema)) {
			SSL_LOG(ERROR, "%s: json parse error", module_name);
			return (SSL_ERR);
		}
		t_asn_v2_module *asn1_module = NULL;
		if (SSL_OK != asn1_v2_schema_parse(&asn1_module, jschema)) {
			SSL_LOG(ERROR, "%s: asn1 schema parse failed", module_name);
			return (SSL_ERR);
		}

		const char *asn1_value_name = __must_get_json_parameter(jparams, "value");
		const t_asn_v2_value *asn1_value = asn1_v2_module_get_value(asn1_module, asn1_value_name);

		if (NULL == asn1_value) {
			SSL_LOG(ERROR, "%s: no such asn1 value in the parsed schema: `%s`", module_name, asn1_value_name);
			return (SSL_ERR);
		}
		t_der_v2_value *der_value = NULL;
		if (SSL_OK != der_v2_value_compile(&der_value, asn1_value)) {
			SSL_LOG(ERROR, "%s: asn1 value compile failed", module_name);
			return (SSL_ERR);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *dvalue_dumps = der_v2_value_pretty_dumps(der_value, true);
			ft_printf("%s\n", dvalue_dumps);
		}

		t_ostring encoded = {0};
		if (SSL_OK != der_v2_generic_encode(&encoded.content, &encoded.size, der_value)) {
			SSL_LOG(ERROR, "%s: der generic encode failed", module_name);
			return (SSL_ERR);
		} else {
			SSL_LOG(INFO, "%s: der generic encode success", module_name);
		}

		if (ft_htbl_has(cmd->opts, "-v")) {
			char *encoded_dumps = ft_bytes_dumps_hex(encoded.content, encoded.size);
			ft_printf("%s\n", encoded_dumps);
		}
	}

	else {
		SSL_LOG(ERROR, "unknown module: %s", module_name);
		return (SSL_ERR);
	}

	return (SSL_OK);
}
