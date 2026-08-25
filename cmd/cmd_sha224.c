#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <hash.h>
#include <textutil.h>

int	cmd_sha224(const t_cmd *cmd)
{
	t_io_v2_stream	*in = NULL, *out = NULL;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	// Source
	if (ft_htbl_has(cmd->opts, "-s")) {
		char *strarg = ft_htbl_get(cmd->opts, "-s");
		t_ostring *ostring = ft_ostr_create_from_cstr(strarg);
		if (SSL_OK != io_v2_bytes_reader(&in, ostring)) {
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
	if (ft_htbl_has(cmd->opts, "-p")) {
		if (SSL_OK != io_v2_fd_writer(&out, 1)) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (SSL_OK != io_v2_file_writer(&out, "/dev/null")) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}

	// Digest transform
	t_hash sha224_ctx = {0};
	sha224_init(&sha224_ctx);
	t_io_v2_stream *sha224_filter = NULL;
	if (io_v2_filter_reader(&sha224_filter, in, sha224_transform_update, sha224_transform_final, &sha224_ctx) < 0) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	in = sha224_filter;

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

	// Format and write output
	if (SSL_OK != io_v2_fd_writer(&out, 1)) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	// Add terminating newline when writing output
	t_textutil_ctx textutil_ctx = {.delim = '\n'};
	t_io_v2_stream *textutil_filter = NULL;
	if (io_v2_filter_writer(&textutil_filter, out, NULL, textutil_terminator_final, &textutil_ctx) < 0) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}
	out = textutil_filter;
	// Write checksum in hex format
	char *hexhash = ft_bytes_to_hex(sha224_ctx.hash, sha224_ctx.hashsize);
	if (io_v2_write_all(out, hexhash, ft_strlen(hexhash)) < 0) {
		SSL_LOG(ERROR, "failed to output digest checksum");
		return (SSL_ERR);
	}
	return (SSL_OK);
}
