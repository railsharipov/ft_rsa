#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <base64.h>
#include <textutil.h>

int	cmd_base64(const t_cmd *cmd)
{
	t_io_v2_stream	*in = NULL, *out = NULL;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
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

	// Base64 transform
	t_b64_ctx b64_ctx = {.final = 0, .done = 0};
	t_io_v2_stream *b64_filter = NULL;

	// Base64 mode: encode or decode
	if (ft_htbl_has(cmd->opts, "-d")) {
		// Remove whitespace from input before feeding into decoder
		t_textutil_ctx *textutil_ctx = NULL;
		t_io_v2_stream *textutil_filter = NULL;
		SSL_ALLOC(textutil_ctx, sizeof(t_textutil_ctx));
		*textutil_ctx = (t_textutil_ctx){0};
		if (io_v2_filter_reader(&textutil_filter, in, textutil_del_eolws_update, textutil_del_eolws_final, textutil_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = textutil_filter;
		// Feed processed input into base64 decoder
		if (io_v2_filter_reader(&b64_filter, in, base64_decode_transform_update, base64_decode_transform_final, &b64_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = b64_filter;
	}
	else {
		// Feed input into base64 encoder
		if (io_v2_filter_reader(&b64_filter, in, base64_encode_transform_update, base64_encode_transform_final, &b64_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = b64_filter;
		// Add newline terminator at the end of output
		t_textutil_ctx *textutil_ctx = NULL;
		t_io_v2_stream *textutil_filter = NULL;
		SSL_ALLOC(textutil_ctx, sizeof(t_textutil_ctx));
		*textutil_ctx = (t_textutil_ctx){.delim = '\n'};
		if (io_v2_filter_reader(&textutil_filter, in, NULL, textutil_terminator_final, textutil_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
		in = textutil_filter;
	}

	// Output line breaks
	if (ft_htbl_has(cmd->opts, "-b")) {
		if (ft_htbl_has(cmd->opts, "-d")) {
			SSL_LOG(WARN, "ignoring line width option in decode mode");
		}
		else {
			int line_width = ft_atoi(ft_htbl_get(cmd->opts, "-b"));
			if (line_width < 0) {
				SSL_LOG(ERROR, "invalid line width: %d", line_width);
				return (SSL_ERR);
			}
			// Add newlines according to specified line width
			t_textutil_ctx *textutil_ctx = NULL;
			t_io_v2_stream *textutil_filter = NULL;
			SSL_ALLOC(textutil_ctx, sizeof(t_textutil_ctx));
			*textutil_ctx = (t_textutil_ctx){.line_width = line_width, .delim = '\n'};
			if (io_v2_filter_reader(&textutil_filter, in, textutil_insert_delim_update, textutil_insert_delim_final, textutil_ctx) < 0) {
				SSL_LOG(ERROR, IO_INIT_ERROR);
				return (SSL_ERR);
			}
			in = textutil_filter;
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
