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

typedef enum __e_asn1_form {
	__ASN1_FORM_PEM,
	__ASN1_FORM_B64,
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
		} else if (ft_streq(informs, "B64")) {
			inform = __ASN1_FORM_B64;
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
		const char *outforms = ft_htbl_get(cmd->opts, "--outform");
		if (ft_streq(outforms, "PEM")) {
			outform = __ASN1_FORM_PEM;
		} else if (ft_streq(outforms, "B64")) {
			outform = __ASN1_FORM_B64;
		} else if (ft_streq(outforms, "DER")) {
			outform = __ASN1_FORM_DER;
		} else if (ft_streq(outforms, "JSON")) {
			outform = __ASN1_FORM_JSON;
		} else {
			SSL_LOG(ERROR, "invalid asn1 output format: %s", outforms);
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

	// Parse ASN1
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
		if (SSL_OK != pem_decode_from_stream(&pem_ctx, in, &enc, NULL)) {
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

	case __ASN1_FORM_B64:
		;;
		// Remove whitespace from input before feeding into decoder
		t_textutil_ctx *textutil_ctx = NULL;
		t_io_v2_stream *textutil_filter = NULL;
		SSL_ALLOC(textutil_ctx, sizeof(t_textutil_ctx));
		*textutil_ctx = (t_textutil_ctx){0};
		if (SSL_OK != io_v2_filter_reader(&textutil_filter, in, textutil_del_eolws_update, textutil_del_eolws_final, textutil_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = textutil_filter;
		// Feed processed input into base64 decoder
		t_b64_ctx b64_ctx = {0};
		t_io_v2_stream *b64_filter = NULL;
		if (SSL_OK != io_v2_filter_reader(&b64_filter, in, base64_decode_transform_update, base64_decode_transform_final, &b64_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = b64_filter;
		// Decode DER encoding
		if (SSL_OK != der_decode_stream(&asn1_node, in)) {
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
		SSL_LOG(ERROR, "invalid input format");
		return (SSL_ERR);
	}

	// Dump ASN1
	switch (outform) {
	case __ASN1_FORM_PEM:
		;;
		// Encode ASN1 to DER encoding
		t_ostring enc;
		ft_ostr_init_with_capacity(&enc, 128);
		if (SSL_OK != der_encode(asn1_node, &enc)) {
			SSL_LOG(ERROR, "der encode error");
			return (SSL_ERR);
		}
		// TODO: handle encrypted PEM input
		t_pem pem_ctx = {0};
		pem_ctx.label = pem_label;
		// Encode DER to PEM encoding
		if (SSL_OK != pem_encode_to_stream(&pem_ctx, &enc, out, NULL)) {
			SSL_LOG(ERROR, "der decode error");
			return (SSL_ERR);
		}
		break;

	case __ASN1_FORM_B64:
		;;
		// Add newline terminator at the end of output
		t_textutil_ctx *textutil_ctx = NULL;
		t_io_v2_stream *textutil_filter = NULL;
		SSL_ALLOC(textutil_ctx, sizeof(t_textutil_ctx));
		*textutil_ctx = (t_textutil_ctx){.delim = '\n'};
		if (io_v2_filter_writer(&textutil_filter, out, NULL, textutil_terminator_final, textutil_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		out = textutil_filter;
		// Feed DER output to base64 encoder
		t_b64_ctx b64_ctx = {0};
		t_io_v2_stream *b64_filter = NULL;
		if (SSL_OK != io_v2_filter_writer(&b64_filter, out, base64_encode_transform_update, base64_encode_transform_final, &b64_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		out = b64_filter;
		// Encode ASN1 and feed output to base64 encoder
		if (SSL_OK != der_encode_stream(asn1_node, out)) {
			SSL_LOG(ERROR, "der encode error");
			return (SSL_ERR);
		}
		break;

	case __ASN1_FORM_DER:
		;;
		if (SSL_OK != der_encode_stream(asn1_node, out)) {
			SSL_LOG(ERROR, "der encode error");
			return (SSL_ERR);
		}
		break;

	case __ASN1_FORM_JSON:
		;;
		char *asn1_s = asn1_node_dumps(asn1_node);
		if (io_v2_write_all(out, asn1_s, ft_strlen(asn1_s)) < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (SSL_ERR);
		}
		if (io_v2_write_all(out, "\n", 1) < 0) {
			SSL_LOG(ERROR, IO_WRITE_ERROR);
			return (SSL_ERR);
		}
		break;

	default:
		;;
		SSL_LOG(ERROR, "invalid output format");
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
