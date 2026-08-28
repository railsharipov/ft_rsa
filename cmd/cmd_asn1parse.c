#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <json.h>
#include <asn1.h>
#include <io.h>

int	cmd_asn1parse(const t_cmd *cmd)
{
	t_io_v2_stream *out = NULL;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	// Sink
	if (SSL_OK != io_v2_fd_writer(&out, 1)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}

	// Parse ASN1 structure
	if (ft_htbl_has(cmd->opts, "--schema")) {
		const char *file_name = ft_htbl_get(cmd->opts, "--schema");
		t_node *json = NULL;
		if (SSL_OK != json_parse_file(file_name, &json)) {
			SSL_LOG(ERROR, "json error");
			return (SSL_ERR);
		}
		char *jsons = json_dumps(json);
		SSL_LOG(DEBUG, "parsed json:\n%s", jsons);

		t_node *asn1_tree = asn1_tree_create(json);
		if (NULL == asn1_tree) {
			SSL_LOG(ERROR, "asn1 error");
			return (SSL_ERR);
		}
		char *asn1_s = asn1_tree_dumps(asn1_tree);

		if (io_v2_write_all(out, asn1_s, ft_strlen(asn1_s)) < 0) {
			SSL_LOG(ERROR, "i/o error");
			return (SSL_ERR);
		}
		if (io_v2_write_all(out, "\n", 1) < 0) {
			SSL_LOG(ERROR, "i/o error");
			return (SSL_ERR);
		}
	}
	else {
		SSL_LOG(WARN, "noop");
	}

	// Close all streams
	if (io_v2_close(out) < 0) {
		SSL_LOG(ERROR, IO_CLOSE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}
