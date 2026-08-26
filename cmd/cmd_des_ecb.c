#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <des.h>
#include <base64.h>
#include <textutil.h>

int	cmd_des_ecb(const t_cmd *cmd)
{
	t_io_v2_stream	*in = NULL, *out = NULL;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	// Key is required
	uint8_t des_key[8] = {0};

	if (ft_htbl_has(cmd->opts, "-k")) {
		char *des_hexkey = ft_htbl_get(cmd->opts, "-k");
		if (!ft_str_ishex(des_hexkey)) {
			SSL_LOG(ERROR, "key must be in hex format");
			return (SSL_ERR);
		}
		if (ft_strlen(des_hexkey) != 16) {
			SSL_LOG(ERROR, "key must be 16 characters in length");
			return (SSL_ERR);
		}
		ft_hex_to_bytes(&des_key, des_hexkey, 16);
	} else {
		SSL_LOG(ERROR, "key must be provided");
		return (SSL_ERR);
	}

	if (ft_htbl_has(cmd->opts, "-s")) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
	}
	if (ft_htbl_has(cmd->opts, "-p")) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
	}
	if (ft_htbl_has(cmd->opts, "-n")) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
	}
	if (ft_htbl_has(cmd->opts, "-v")) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
	}

	// Source
	if (ft_htbl_has(cmd->opts, "-i")) {
		if (SSL_OK != io_v2_file_reader(&in, ft_htbl_get(cmd->opts, "-i"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (SSL_OK != io_v2_fd_reader(&in, 0)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}

	// Sink
	if (ft_htbl_has(cmd->opts, "-o")) {
		if (SSL_OK != io_v2_file_writer(&out, ft_htbl_get(cmd->opts, "-o"))) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (SSL_OK != io_v2_fd_writer(&out, 1)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}

	t_des_ctx des_ctx = {0};
	t_io_v2_stream *des_filter = NULL;

	if (ft_htbl_has(cmd->opts, "-d")) {
		// Decrypt mode
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
	}
	else {
		// Encrypt mode
		if (SSL_OK != des_ecb_encrypt_init(&des_ctx, des_key)) {
			SSL_LOG(ERROR, "des-ecb init error");
			return (SSL_ERR);
		}
		// Feed input into des encrypter
		if (SSL_OK != io_v2_filter_reader(&des_filter, in, des_ecb_encrypt_transform_update, des_ecb_encrypt_transform_final, &des_ctx)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = des_filter;

		if (ft_htbl_has(cmd->opts, "-a")) {
			// Output must be base64 encoded
			t_b64_ctx *b64_ctx = NULL;
			t_io_v2_stream *b64_filter = NULL;
			SSL_ALLOC(b64_ctx, sizeof(t_b64_ctx));
			*b64_ctx = (t_b64_ctx){0};
			// Feed des crypt into base64 encoder
			if (io_v2_filter_reader(&b64_filter, in, base64_encode_transform_update, base64_encode_transform_final, b64_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = b64_filter;
			// Break base64 output into lines of 64 characters
			t_textutil_ctx *linebreak_ctx = NULL;
			t_io_v2_stream *linebreak_filter = NULL;
			SSL_ALLOC(linebreak_ctx, sizeof(t_textutil_ctx));
			*linebreak_ctx = (t_textutil_ctx){.delim = '\n', .line_width = 64};
			if (io_v2_filter_reader(&linebreak_filter, in, textutil_insert_delim_update, textutil_insert_delim_final, linebreak_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = linebreak_filter;
			// Add newline terminator at the end of output
			t_textutil_ctx *terminator_ctx = NULL;
			t_io_v2_stream *terminator_filter = NULL;
			SSL_ALLOC(terminator_ctx, sizeof(t_textutil_ctx));
			*terminator_ctx = (t_textutil_ctx){.delim = '\n'};
			if (io_v2_filter_reader(&terminator_filter, in, NULL, textutil_terminator_final, terminator_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = terminator_filter;
		}
	}

	// Create and run the pipe
	t_io_v2_pipe *pipe = NULL;
	if (io_v2_pipe_unidir(&pipe, in, out) < 0) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	while (pipe->status == IO_V2_STATUS_PIPE_OK) {
		io_v2_pipe_pump(pipe);
	}
	if (pipe->status == IO_V2_STATUS_PIPE_ERROR) {
		SSL_LOG(ERROR, "i/o operation failed");
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
