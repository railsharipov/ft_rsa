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
	t_iodes		in, out;
	t_ostring	os_in, os_out;
	t_func_b64	f_b64;
	int			ret;

	if (NULL == cmd) {
		SSL_LOG(ERROR, INVALID_INPUT_ERROR);
		return (SSL_ERR);
	}

	if (ft_htbl_has(cmd->opts, "-i")) {
		ret = io_fopen(&in, IO_READ|IO_FILE, ft_htbl_get(cmd->opts, "-i"));
	} else {
		ret = io_fopen(&in, IO_READ|IO_STDIN, NULL);
	}
	if (SSL_OK != ret) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}

	if (ft_htbl_has(cmd->opts, "-o")) {
		ret = io_fopen(&out, IO_WRITE|IO_FILE, ft_htbl_get(cmd->opts, "-o"));
	} else {
		ret = io_fopen(&out, IO_WRITE|IO_STDOUT, NULL);
	}
	if (SSL_OK != ret) {
		SSL_LOG(ERROR, IO_INIT_ERROR);
		return (SSL_ERR);
	}

	if (ft_htbl_has(cmd->opts, "-d")) {
		f_b64 = base64_decode;
		in.delim = '\n';
		out.delim = 0;
	} else {
		f_b64 = base64_encode;
	}

	if (ft_htbl_has(cmd->opts, "-b")) {
		out.lwidth = ft_atoi(ft_htbl_get(cmd->opts, "-b"));
		out.lwidth = MAX(0, out.lwidth);
		out.delim = '\n';
	}

	if (SSL_OK != __get_input(&in, &os_in)) {
		SSL_LOG(ERROR, "failed to get input");
		return (SSL_ERR);
	}

	if (SSL_OK != f_b64(os_in.content, os_in.size, &os_out.content, &os_out.size)) {
		SSL_LOG(ERROR, "base64 error");
		return (SSL_ERR);
	}

	if (SSL_OK != __write_output(&out, &os_out)) {
		SSL_LOG(ERROR, "failed to write output");
		return (SSL_ERR);
	}

	io_fclose_multi(&in, &out, NULL);

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
