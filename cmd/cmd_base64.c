#include <common.h>
#include <logger.h>
#include <args.h>
#include <cmd.h>
#include <base64.h>

static int	__get_input(t_iodes *in, t_ostring *os_in);
static int	__write_output(t_iodes *out, t_ostring *os_out);

typedef int	(*t_func_b64)(const unsigned char *, size_t, unsigned char **, size_t *);

int	cmd_base64(const t_cmd *cmd)
{
	t_io_v2_stream	*in, *out;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

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

	if (ft_htbl_has(cmd->opts, "-b")) {
		SSL_LOG(ERROR, NOT_IMPLEMENTED_ERROR);
		return (SSL_ERR);
		// int line_width = ft_atoi(ft_htbl_get(cmd->opts, "-b"));
		// line_width = MAX(0, line_width);
	}

	t_b64_ctx b64_ctx = {.final = 0, .done = 0};
	t_io_v2_stream *b64_filter = NULL;

	if (ft_htbl_has(cmd->opts, "-d")) {
		if (io_v2_filter_reader(&b64_filter, in, base64_decode_update, base64_decode_final, &b64_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	} else {
		if (io_v2_filter_reader(&b64_filter, in, base64_encode_update, base64_encode_final, &b64_ctx) < 0) {
			SSL_LOG(ERROR, IO_INIT_ERROR);
			return (SSL_ERR);
		}
	}

	t_io_v2_pipe *pipe = NULL;
	if (io_v2_pipe_unidir(&pipe, b64_filter, out) < 0) {
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

	// if (SSL_OK != __get_input(&in, &os_in)) {
	// 	SSL_LOG(ERROR, "failed to get input");
	// 	return (SSL_ERR);
	// }
	// if (SSL_OK != f_b64(os_in.content, os_in.size, &os_out.content, &os_out.size)) {
	// 	SSL_LOG(ERROR, "base64 error");
	// 	return (SSL_ERR);
	// }
	// if (SSL_OK != __write_output(&out, &os_out)) {
	// 	SSL_LOG(ERROR, "failed to write output");
	// 	return (SSL_ERR);
	// }

	if (io_v2_close(b64_filter) < 0) {
		SSL_LOG(ERROR, "base64 input stream close error");
		return (SSL_ERR);
	}
	if (io_v2_close(out) < 0) {
		SSL_LOG(ERROR, "base64 output stream close error");
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__get_input(t_iodes *in, t_ostring *os_in)
{
	// TODO: use streams to avoid reading too much data into memory
	const size_t	MAX_SIZE = 1024 * 1024;
	char	buf[IO_BUFSIZE];
	int		rbytes;

	ft_ostr_init(os_in);

	while ((rbytes = io_read(in, buf, IO_BUFSIZE)) > 0) {
		ft_ostr_append(os_in, buf, rbytes);
		if (os_in->size >= MAX_SIZE) {
			SSL_LOG(ERROR, "input size is too large");
			return (SSL_ERR);
		}
	}
	if (rbytes < 0) {
		ft_ostr_clear(os_in);
		SSL_LOG(ERROR, IO_READ_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static int	__write_output(t_iodes *out, t_ostring *os_out)
{
	if (io_write(out, (char *)os_out->content, os_out->size) < 0) {
		SSL_LOG(ERROR, IO_WRITE_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}
