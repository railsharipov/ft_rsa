#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <json.h>
#include <asn1.h>
#include <der.h>
#include <pem.h>
#include <io.h>

typedef enum __e_asn1_form {
	__ASN1_FORM_PEM,
	__ASN1_FORM_DER,
	__ASN1_FORM_JSON,
} __t_asn1_form;

int	cmd_asn1parse(const t_cmd *cmd)
{
	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	// ANS1 input format
	__t_asn1_form inform = __ASN1_FORM_PEM;
	if (ft_htbl_has(cmd->opts, "--inform")) {
		// Parse ASN1 schema from JSON file
		const char *informs = ft_htbl_get(cmd->opts, "--inform");
		if (ft_streq(informs, "PEM")) {
			inform = __ASN1_FORM_PEM;
		} else if (ft_streq(informs, "DER")) {
			inform = __ASN1_FORM_DER;
		} else if (ft_streq(informs, "JSON")) {
			inform = __ASN1_FORM_JSON;
		} else {
			SSL_LOG(ERROR, "invalid asn1 input format: %s", informs);
			return (SSL_ERR);
		}
	}
	// ANS1 output format
	__t_asn1_form outform = __ASN1_FORM_PEM;
	if (ft_htbl_has(cmd->opts, "--outform")) {
		// Parse ASN1 schema from JSON file
		const char *outforms = ft_htbl_get(cmd->opts, "--inform");
		if (ft_streq(outforms, "PEM")) {
			outform = __ASN1_FORM_PEM;
		} else if (ft_streq(outforms, "DER")) {
			outform = __ASN1_FORM_DER;
		} else if (ft_streq(outforms, "JSON")) {
			outform = __ASN1_FORM_JSON;
		} else {
			SSL_LOG(ERROR, "invalid asn1 input format: %s", outforms);
			return (SSL_ERR);
		}
	}

	// Source
	t_io_v2_stream *in = NULL;
	if (ft_htbl_has(cmd->opts, "-i")) {
		if (SSL_OK != io_v2_file_reader(&in, ft_htbl_get(cmd->opts, "-i"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}
	else {
		in = io_v2_stdin_reader;
	}

	// Sink
	t_io_v2_stream *out = NULL;
	if (ft_htbl_has(cmd->opts, "-o")) {
		if (SSL_OK != io_v2_file_writer(&out, ft_htbl_get(cmd->opts, "-o"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}
	else {
		out = io_v2_stdout_writer;
	}

	// Parse ASN1 from input
	t_node *asn1_node = NULL;
	char *pem_label = NULL;
	switch (inform) {
	case __ASN1_FORM_PEM:
		;;
		// TODO: handle encrypted PEM input
		t_pem pem_ctx = {0};
		t_ostring enc;
		ft_ostr_init_with_capacity(&enc, 128);
		// Decode PEM and get DER encoding
		if (SSL_OK != pem_decode_stream(&pem_ctx, in, &enc, NULL)) {
			SSL_LOG(ERROR, "der decode error");
			return (SSL_ERR);
		}
		pem_label = ft_strdup(pem_ctx.label);
		// Decode DER encoding
		if (SSL_OK != der_decode(&asn1_node, &enc)) {
			SSL_LOG(ERROR, "der decode error");
			return (SSL_ERR);
		}
		break;

	case __ASN1_FORM_DER:
		;;
		if (SSL_OK != der_decode_stream(&asn1_node, in)) {
			SSL_LOG(ERROR, "der decode error");
			return (SSL_ERR);
		}
		break;

	case __ASN1_FORM_JSON:
		;;
		t_node *json = NULL;
		if (SSL_OK != json_parse_stream(in, &json)) {
			SSL_LOG(ERROR, "json error");
			return (SSL_ERR);
		}
		asn1_node = asn1_node_create_from_schema(json);
		if (NULL == asn1_node) {
			SSL_LOG(ERROR, "asn1 error");
			return (SSL_ERR);
		}
		break;

	default:
		;;
		SSL_LOG(ERROR, UNEXPECTED_ERROR);
		return (SSL_ERR);
	}

	// Dump ASN1 to output
	char *asn1_s = asn1_node_dumps(asn1_node);
	if (io_v2_write_all(out, asn1_s, ft_strlen(asn1_s)) < 0) {
		SSL_LOG(ERROR, "i/o error");
		return (SSL_ERR);
	}
	if (io_v2_write_all(io_v2_stdout_writer, "\n", 1) < 0) {
		SSL_LOG(ERROR, "i/o error");
		return (SSL_ERR);
	}

	// Close all streams
	if (io_v2_close(in) < 0) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	if (io_v2_close(out) < 0) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}
