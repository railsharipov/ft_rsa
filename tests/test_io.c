#include <common.h>
#include <logger.h>
#include <io.h>
#include <file.h>
#include "test.h"
#include <rand.h>
#include <libft.h>

static int	__test_io_setup(void);
static int	__test_io_interface(void);
static int	__test_io_file_reader(void);
static int	__test_io_file_writer(void);
static int	__test_io_bytes_reader(void);
static int	__test_io_bytes_writer(void);
static int	__test_io_buffered_reader(void);
static int	__test_io_buffered_writer(void);
static int	__test_io_filter_reader(void);
static int	__test_io_filter_writer(void);
static int	__test_io_pipe_unidir(void);

static const char	*__small_text_file_path = "tests/files/text/small.txt";
static const char	*__medium_text_file_path = "tests/files/text/medium.txt";
static const char	*__large_text_file_path = "tests/files/text/large.txt";

static const char	*__test_text_file_path = "tests/files/text/test.txt";

static const size_t	__mock_data_size_max = 1024 * 1024;
static size_t		__mock_data_size_seed;

int	test_io(void)
{
	if (SSL_OK != __test_io_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	return (
		__test_io_interface()
		|| __test_io_file_reader()
		|| __test_io_file_writer()
		|| __test_io_bytes_reader()
		|| __test_io_bytes_writer()
		|| __test_io_buffered_reader()
		|| __test_io_buffered_writer()
		|| __test_io_filter_reader()
		|| __test_io_filter_writer()
		|| __test_io_pipe_unidir()
	);
}

static int	__test_io_setup(void)
{
	{
		uint64_t	seed;

		if (SSL_OK != rand_useed(&seed, sizeof(size_t))) {
			TEST_LOG(ERROR, "failed to get random seed");
			return (SSL_ERR);
		}
		__mock_data_size_seed = (size_t)seed;
	}
	return (SSL_OK);
}

struct s_mock_ctx {
	size_t	data_size;
	size_t	bytes_read;
	size_t	bytes_written;
	size_t	bytes_flushed;
	size_t	bytes_finished;
};

static ssize_t	__mock_interface_read_ok(void *ctx, void *buf, size_t nbytes)
{
	struct s_mock_ctx *mock_ctx;
	ssize_t		result;

	(void)buf;
	mock_ctx = (struct s_mock_ctx *)ctx;

	if (mock_ctx->data_size < nbytes) {
		result = mock_ctx->data_size;
		mock_ctx->data_size = 0;
	}
	else {
		result = nbytes;
		mock_ctx->data_size -= nbytes;
	}
	mock_ctx->bytes_read += result;
	return (result);
}

static ssize_t	__mock_interface_write_ok(void *ctx, const void *buf, size_t nbytes)
{
	struct s_mock_ctx *mock_ctx;

	(void)buf;
	mock_ctx = (struct s_mock_ctx *)ctx;
	mock_ctx->bytes_written += nbytes;
	return (nbytes);
}

static ssize_t	__mock_interface_finish_ok(void *ctx)
{
	struct s_mock_ctx *mock_ctx;

	mock_ctx = (struct s_mock_ctx *)ctx;
	mock_ctx->bytes_finished += mock_ctx->data_size;
	return (mock_ctx->data_size);
}

static ssize_t	__mock_interface_flush_ok(void *ctx)
{
	struct s_mock_ctx *mock_ctx;

	mock_ctx = (struct s_mock_ctx *)ctx;
	mock_ctx->bytes_flushed += mock_ctx->data_size;
	return (mock_ctx->data_size);
}

static ssize_t	__mock_interface_ok(void *ctx, void *buf, size_t nbytes)
{
	(void)ctx;
	(void)buf;
	(void)nbytes;
	return (IO_V2_STATUS_OK);
}

static ssize_t	__mock_interface_eof(void *ctx, void *buf, size_t nbytes)
{
	(void)ctx;
	(void)buf;
	(void)nbytes;
	return (IO_V2_STATUS_EOF);
}

static ssize_t	__mock_interface_error(void *ctx, void *buf, size_t nbytes)
{
	(void)ctx;
	(void)buf;
	(void)nbytes;
	return (IO_V2_STATUS_ERROR);
}

static void	*__mock_stream_ctx(void)
{
	struct s_mock_ctx *ctx;

	SSL_ALLOC(ctx, sizeof(struct s_mock_ctx));
	ctx->data_size = __mock_data_size_seed * (__mock_data_size_seed + 1) % __mock_data_size_max;
	ctx->bytes_read = 0;
	ctx->bytes_written = 0;
	ctx->bytes_flushed = 0;
	return (ctx);
}

static t_io_v2_stream	__mock_stream(t_io_v2_interface interface, t_io_v2_flag flags)
{
	return (t_io_v2_stream){
		.interface = interface,
		.flags = flags,
		.status = IO_V2_STATUS_OK,
		.ctx = __mock_stream_ctx(),
	};
}

// static t_io_v2_stream	__mock_stream_with_ctx(t_io_v2_interface interface, t_io_v2_flag flags, void *ctx)
// {
// 	return (t_io_v2_stream){
// 		.interface = interface,
// 		.flags = flags,
// 		.status = IO_V2_STATUS_OK,
// 		.ctx = ctx,
// 	};
// }

static int	__test_io_interface(void)
{
	t_io_v2_stream		stream;
	t_io_v2_interface	interface;
	size_t			bufsize = 10 * 1024;
	char			*buf;
	ssize_t			result;

	SSL_ALLOC(buf, bufsize);

	// invalid input

	result = io_v2_read(NULL, buf, bufsize);
	TEST_ASSERT(result == -1);

	result = io_v2_write(NULL, NULL, bufsize);
	TEST_ASSERT(result == -1);

	result = io_v2_flush(NULL);
	TEST_ASSERT(result == -1);

	result = io_v2_finish(NULL);
	TEST_ASSERT(result == -1);

	result = io_v2_close(NULL);
	TEST_ASSERT(result == -1);

	// invalid flags

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_read_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_read_ok,
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	result = io_v2_flush(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_read_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	result = io_v2_finish(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.close = (t_func_io_v2_close)__mock_interface_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	result = io_v2_close(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	// invalid status

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_read_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	stream.status = IO_V2_STATUS_ERROR;
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	stream.status = IO_V2_STATUS_EOF;
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_EOF);

	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	stream.status = IO_V2_STATUS_ERROR;
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.flush = (t_func_io_v2_flush)__mock_interface_flush_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH);
	stream.status = IO_V2_STATUS_ERROR;
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.finish = (t_func_io_v2_finish)__mock_interface_finish_ok
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FINISH);
	stream.status = IO_V2_STATUS_ERROR;
	result = io_v2_finish(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	stream.status = IO_V2_STATUS_FINISHED;
	result = io_v2_finish(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_FINISHED);

	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_finish(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	interface = (t_io_v2_interface){
		.close = (t_func_io_v2_close)__mock_interface_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_CLOSE);
	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_close(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	stream.status = IO_V2_STATUS_CLOSED;
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	// happy path

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_read_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result >= 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result >= 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.finish = (t_func_io_v2_finish)__mock_interface_finish_ok
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FINISH);
	result = io_v2_finish(&stream);
	TEST_ASSERT(result >= 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_FINISHED);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.flush = (t_func_io_v2_flush)__mock_interface_flush_ok,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH);
	result = io_v2_flush(&stream);
	TEST_ASSERT(result >= 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_OK);

	interface = (t_io_v2_interface){
		.close = (t_func_io_v2_close)__mock_interface_ok
	};
	stream = __mock_stream(interface, IO_V2_FLAG_CLOSE);
	result = io_v2_close(&stream);
	TEST_LOG(INFO, "stream.status = %d", stream.status);
	TEST_ASSERT(result == 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_CLOSED);

	// error path

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_error,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_error
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE);
	result = io_v2_write(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.finish = (t_func_io_v2_finish)__mock_interface_error
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FINISH);
	result = io_v2_finish(&stream);
	TEST_ASSERT(result < 0);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	interface = (t_io_v2_interface){
		.write = (t_func_io_v2_write)__mock_interface_write_ok,
		.flush = (t_func_io_v2_flush)__mock_interface_error,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH);
	result = io_v2_flush(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	interface = (t_io_v2_interface){
		.close = (t_func_io_v2_close)__mock_interface_error
	};
	stream = __mock_stream(interface, IO_V2_FLAG_CLOSE);
	result = io_v2_close(&stream);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_ERROR);

	// eof path

	interface = (t_io_v2_interface){
		.read = (t_func_io_v2_read)__mock_interface_eof,
	};
	stream = __mock_stream(interface, IO_V2_FLAG_READ);
	result = io_v2_read(&stream, buf, bufsize);
	TEST_ASSERT(result == -1);
	TEST_ASSERT(stream.status == IO_V2_STATUS_EOF);

	TEST_PASS();
}

static int	__test_io_file_reader(void)
{
	t_io_v2_stream	*stream;
	t_ostring		ref_content, test_content;
	char 			*buf;
	size_t			bufsize;
	ssize_t			rbytes, tbytes;
	int				ret;

	// Small file
	if (SSL_OK != file_read_all(__small_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	bufsize = ref_content.size;
	SSL_ALLOC(buf, bufsize);

	ret = io_v2_file_reader(&stream, __small_text_file_path);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(stream->interface.read != NULL);
	TEST_ASSERT(stream->interface.write == NULL);
	TEST_ASSERT(stream->interface.close != NULL);
	TEST_ASSERT(stream->interface.flush == NULL);
	TEST_ASSERT(stream->ctx != NULL);
	TEST_ASSERT(stream->flags == (IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE));

	rbytes = io_v2_read(stream, buf, bufsize);
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == bufsize);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, bufsize) == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);

	rbytes = io_v2_read(stream, buf, bufsize);
	TEST_ASSERT(rbytes == -1);
	TEST_ASSERT(stream->status == IO_V2_STATUS_EOF);

	rbytes = io_v2_close(stream);
	TEST_ASSERT(rbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	rbytes = io_v2_read(stream, buf, 1);
	TEST_ASSERT(rbytes == -1);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	ft_ostr_clear(&ref_content);

	// Large file
	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	bufsize = 1024 * 1024;
	SSL_ALLOC(buf, bufsize);

	ret = io_v2_file_reader(&stream, __large_text_file_path);
	TEST_ASSERT(SSL_OK == ret);

	ft_ostr_init(&test_content);
	tbytes = 0;

	while (stream->status == IO_V2_STATUS_OK) {
		rbytes = io_v2_read(stream, buf, bufsize);
		if (rbytes < 0) {
			break;
		}
		ft_ostr_append(&test_content, buf, rbytes);
		tbytes += rbytes;
	}
	TEST_ASSERT(stream->status == IO_V2_STATUS_EOF);
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, tbytes) == 0);

	rbytes = io_v2_close(stream);
	TEST_ASSERT(rbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	ft_ostr_clear(&test_content);
	ft_ostr_clear(&ref_content);
	SSL_FREE(buf);

	TEST_PASS();
}

static int	__test_io_file_writer(void)
{
	t_io_v2_stream	*stream;
	t_ostring		ref_content, test_content;
	ssize_t			wbytes, tbytes;
	int				ret;

	// Small file
	if (SSL_OK != file_read_all(__small_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	ret = io_v2_file_writer(&stream, __test_text_file_path);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(stream->interface.read == NULL);
	TEST_ASSERT(stream->interface.write != NULL);
	TEST_ASSERT(stream->interface.close != NULL);
	TEST_ASSERT(stream->interface.flush == NULL);
	TEST_ASSERT(stream->ctx != NULL);
	TEST_ASSERT(stream->flags == (IO_V2_FLAG_WRITE | IO_V2_FLAG_CLOSE));

	wbytes = io_v2_write(stream, ref_content.content, ref_content.size);
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(wbytes == ref_content.size);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);

	wbytes = io_v2_close(stream);
	TEST_ASSERT(wbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	if (SSL_OK != file_read_all(__test_text_file_path, &test_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	// Large file
	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	ret = io_v2_file_writer(&stream, __test_text_file_path);
	TEST_ASSERT(SSL_OK == ret);

	tbytes = 0;

	while (tbytes < ref_content.size && stream->status == IO_V2_STATUS_OK) {
		wbytes = io_v2_write(stream, ref_content.content + tbytes, 512);
		if (wbytes < 0) {
			break;
		}
		if (wbytes == 0) {
			break;
		}
		tbytes += wbytes;
	}
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(tbytes == ref_content.size);

	wbytes = io_v2_close(stream);
	TEST_ASSERT(wbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	if (SSL_OK != file_read_all(__test_text_file_path, &test_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	TEST_PASS();
}

static int	__test_io_bytes_reader(void)
{
	t_io_v2_stream	*stream;
	t_ostring		ref_content, test_content;
	char 			*buf;
	size_t			bufsize;
	ssize_t			rbytes;
	int				ret;

	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	bufsize = 10 * 1024;
	SSL_ALLOC(buf, bufsize);
	ft_ostr_init(&test_content);

	ret = io_v2_bytes_reader(&stream, &ref_content);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(stream->interface.read != NULL);
	TEST_ASSERT(stream->interface.write == NULL);
	TEST_ASSERT(stream->interface.close != NULL);
	TEST_ASSERT(stream->interface.flush == NULL);
	TEST_ASSERT(stream->ctx != NULL);
	TEST_ASSERT(stream->flags == (IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE));

	// read must be successful
	rbytes = io_v2_read(stream, buf, bufsize);
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == bufsize);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(ft_memcmp(buf, ref_content.content, bufsize) == 0);
	ft_ostr_append(&test_content, buf, rbytes);

	// read remaining bytes
	while (stream->status == IO_V2_STATUS_OK) {
		rbytes = io_v2_read(stream, buf, bufsize);
		if (rbytes < 0) {
			break;
		}
		ft_ostr_append(&test_content, buf, rbytes);
	}
	// EOF must be reached
	TEST_ASSERT(stream->status == IO_V2_STATUS_EOF);
	// test content must be the same as the reference content
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, test_content.size) == 0);

	// read must fail because the stream is at EOF
	rbytes = io_v2_read(stream, buf, bufsize);
	TEST_ASSERT(rbytes == -1);
	TEST_ASSERT(stream->status == IO_V2_STATUS_EOF);

	// close must be successful
	rbytes = io_v2_close(stream);
	TEST_ASSERT(rbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);
	SSL_FREE(buf);

	TEST_PASS();
}

static int	__test_io_bytes_writer(void)
{
	t_io_v2_stream	*stream;
	t_ostring		ref_content, test_content;
	ssize_t			wbytes, tbytes;
	size_t			write_size = 10 * 1024;
	int				ret;

	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(ref_content.size > write_size);
	ft_ostr_init(&test_content);
	tbytes = 0;

	ret = io_v2_bytes_writer(&stream, &test_content);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(stream->interface.read == NULL);
	TEST_ASSERT(stream->interface.write != NULL);
	TEST_ASSERT(stream->interface.close != NULL);
	TEST_ASSERT(stream->interface.flush == NULL);
	TEST_ASSERT(stream->ctx != NULL);
	TEST_ASSERT(stream->flags == (IO_V2_FLAG_WRITE | IO_V2_FLAG_CLOSE));

	// write must be successful
	wbytes = io_v2_write(stream, ref_content.content, write_size);
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(wbytes == write_size);
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	tbytes += wbytes;

	// write the rest of the data to the stream
	while (tbytes < ref_content.size && stream->status == IO_V2_STATUS_OK) {
		wbytes = io_v2_write(stream, ref_content.content + tbytes, MIN(write_size, ref_content.size - tbytes));
		if (wbytes <= 0) {
			break;
		}
		tbytes += wbytes;
	}
	// stream must be in OK status
	TEST_ASSERT(stream->status == IO_V2_STATUS_OK);
	// test content must be the same as the reference content
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, tbytes) == 0);

	// close must be successful
	wbytes = io_v2_close(stream);
	TEST_ASSERT(wbytes == 0);
	TEST_ASSERT(stream->status == IO_V2_STATUS_CLOSED);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	TEST_PASS();
}

static int	__test_io_buffered_reader(void)
{
	t_io_v2_stream	*buffered_stream, *upstream;
	t_ostring		test_content, ref_content;
	size_t			capacity = 1024;
	ssize_t			rbytes, tbytes;
	char 			*buf;
	size_t			bufsize;
	int				ret;

	/* Case 1: read size is less than the capacity of the buffered stream */

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);

	bufsize = 16;
	SSL_ALLOC(buf, bufsize);

	if (SSL_OK != file_read_all(__small_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_file_reader(&upstream, __small_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	ret = io_v2_buffered_reader(&buffered_stream, upstream, capacity);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(buffered_stream->interface.read != NULL);
	TEST_ASSERT(buffered_stream->interface.write == NULL);
	TEST_ASSERT(buffered_stream->interface.close != NULL);
	TEST_ASSERT(buffered_stream->interface.flush == NULL);
	TEST_ASSERT(buffered_stream->ctx != NULL);
	TEST_ASSERT(buffered_stream->flags == (IO_V2_FLAG_READ | IO_V2_FLAG_CLOSE));

	tbytes = 0;
	rbytes = io_v2_read(buffered_stream, buf, sizeof(buf));
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == sizeof(buf));
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	ft_ostr_append(&test_content, buf, rbytes);
	tbytes += rbytes;

	// upstream read must return EOF since buffered stream read all the data into its internal buffer
	rbytes = io_v2_read(upstream, buf, sizeof(buf));
	TEST_ASSERT(rbytes < 0);
	TEST_ASSERT(upstream->status == IO_V2_STATUS_EOF);

	// buffered stream read must return positive value since there is still some buffered data remaining
	rbytes = io_v2_read(buffered_stream, buf, sizeof(buf));
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	ft_ostr_append(&test_content, buf, rbytes);
	tbytes += rbytes;

	// read the rest of the data from the buffered stream
	while (buffered_stream->status == IO_V2_STATUS_OK) {
		rbytes = io_v2_read(buffered_stream, buf, sizeof(buf));
		if (rbytes < 0) {
			break;
		}
		ft_ostr_append(&test_content, buf, rbytes);
		tbytes += rbytes;
	}
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_EOF);
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, tbytes) == 0);

	// close the buffered stream
	rbytes = io_v2_close(buffered_stream);
	TEST_ASSERT(rbytes == 0);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_CLOSED);

	// upstream must have been closed by the buffered stream close
	TEST_ASSERT(upstream->status == IO_V2_STATUS_CLOSED);

	/* Case 2: read size is greater than the capacity of the buffered stream */

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);

	bufsize = 10 * capacity;
	SSL_ALLOC(buf, bufsize);

	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_file_reader(&upstream, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	ret = io_v2_buffered_reader(&buffered_stream, upstream, capacity);
	TEST_ASSERT(SSL_OK == ret);

	// data must be successfully read into the buffer
	tbytes = 0;
	rbytes = io_v2_read(buffered_stream, buf, bufsize);
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == bufsize);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	ft_ostr_append(&test_content, buf, rbytes);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, rbytes) == 0);
	tbytes += rbytes;

	// data must be successfully read into the buffer on consecutive reads
	rbytes = io_v2_read(buffered_stream, buf, bufsize);
	TEST_ASSERT(rbytes > 0);
	TEST_ASSERT(rbytes == bufsize);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	ft_ostr_append(&test_content, buf, rbytes);
	TEST_ASSERT(ft_memcmp(test_content.content + tbytes, ref_content.content + tbytes, rbytes) == 0);
	tbytes += rbytes;

	// read the rest of the data from the buffered stream
	while (buffered_stream->status == IO_V2_STATUS_OK) {
		rbytes = io_v2_read(buffered_stream, buf, bufsize);
		if (rbytes < 0) {
			break;
		}
		ft_ostr_append(&test_content, buf, rbytes);
		tbytes += rbytes;
	}
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_EOF);
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, tbytes) == 0);

	// close the buffered stream
	rbytes = io_v2_close(buffered_stream);
	TEST_ASSERT(rbytes == 0);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_CLOSED);
	// upstream must have been closed by the buffered stream close
	TEST_ASSERT(upstream->status == IO_V2_STATUS_CLOSED);

	TEST_PASS();
}

static int	__test_io_buffered_writer(void)
{
	t_io_v2_stream	*buffered_stream, *downstream;
	t_ostring		test_content, ref_content;
	size_t			capacity = 1024;
	char 			*buf;
	size_t			bufsize;
	size_t			chunk_size = 16;
	ssize_t			wbytes, nbytes, tbytes;
	int				ret;

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);

	bufsize = 16;
	SSL_ALLOC(buf, bufsize);

	if (SSL_OK != file_read_all(__small_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_file_writer(&downstream, __test_text_file_path)) {
		TEST_LOG(ERROR, FILE_WRITE_ERROR);
		TEST_FAIL();
	}

	ret = io_v2_buffered_writer(&buffered_stream, downstream, capacity);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(buffered_stream->interface.read == NULL);
	TEST_ASSERT(buffered_stream->interface.write != NULL);
	TEST_ASSERT(buffered_stream->interface.close != NULL);
	TEST_ASSERT(buffered_stream->interface.flush != NULL);
	TEST_ASSERT(buffered_stream->ctx != NULL);
	TEST_ASSERT(buffered_stream->flags == (IO_V2_FLAG_WRITE | IO_V2_FLAG_FLUSH | IO_V2_FLAG_CLOSE));

	/* Case 1: write size is less than the capacity of the buffered stream */

	// data must be successfully written to the buffered stream
	tbytes = 0;
	wbytes = io_v2_write(buffered_stream, ref_content.content + tbytes, chunk_size);
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(wbytes == chunk_size);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	tbytes += chunk_size;

	// write the rest of the data to the buffered stream
	while (tbytes < ref_content.size && buffered_stream->status == IO_V2_STATUS_OK) {
		nbytes = MIN(chunk_size, ref_content.size - tbytes);

		wbytes = io_v2_write(buffered_stream, ref_content.content + tbytes, nbytes);
		if (wbytes < 0) {
			break;
		}
		tbytes += nbytes;
	}
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);

	// force write to downstream by flushing the buffered stream
	wbytes = io_v2_flush(buffered_stream);
	// all bytes should have been flushed to downstream since no data must have been written to downstream
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(wbytes == ref_content.size);
	TEST_ASSERT(downstream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);

	// close the buffered stream
	wbytes = io_v2_close(buffered_stream);
	TEST_ASSERT(wbytes == 0);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_CLOSED);
	// downstream should have been closed by the buffered stream close
	TEST_ASSERT(downstream->status == IO_V2_STATUS_CLOSED);

	// read the content of the test file
	if (SSL_OK != file_read_all(__test_text_file_path, &test_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	/* Case 2: write size is greater than the capacity of the buffered stream */

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);

	// make sure capacity is not a multiple of bufsize
	capacity = 1024;
	bufsize = 10 * capacity + 123;
	SSL_ALLOC(buf, bufsize);

	if (SSL_OK != file_read_all(__large_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_file_writer(&downstream, __test_text_file_path)) {
		TEST_LOG(ERROR, FILE_WRITE_ERROR);
		TEST_FAIL();
	}

	ret = io_v2_buffered_writer(&buffered_stream, downstream, capacity);
	TEST_ASSERT(SSL_OK == ret);

	// data must be successfully written to the buffered stream
	tbytes = 0;
	wbytes = io_v2_write(buffered_stream, ref_content.content + tbytes, bufsize);
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(wbytes == bufsize);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);
	tbytes += bufsize;

	// force write to downstream by flushing the buffered stream
	// should return positive value since bytes from internal buffer should have been written to downstream
	wbytes = io_v2_flush(buffered_stream);
	TEST_ASSERT(wbytes > 0);
	// only part of the data should have been flushed to downstream since data must have been already written to downstream
	TEST_ASSERT(wbytes < tbytes);
	// the number of bytes written to downstream should be a number of bytes remaining in the buffered stream's internal buffer
	TEST_ASSERT(wbytes == tbytes % capacity);
	TEST_ASSERT(downstream->status == IO_V2_STATUS_OK);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);

	// write the rest of the data to the buffered stream
	while (tbytes < ref_content.size && buffered_stream->status == IO_V2_STATUS_OK) {
		nbytes = MIN(chunk_size, ref_content.size - tbytes);
		wbytes = io_v2_write(buffered_stream, ref_content.content + tbytes, nbytes);
		if (wbytes < 0) {
			break;
		}
		tbytes += nbytes;
	}
	TEST_ASSERT(tbytes == ref_content.size);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_OK);

	// close the buffered stream
	// downstream should have been closed by the buffered stream close
	wbytes = io_v2_close(buffered_stream);
	TEST_ASSERT(wbytes == 0);
	TEST_ASSERT(buffered_stream->status == IO_V2_STATUS_CLOSED);
	TEST_ASSERT(downstream->status == IO_V2_STATUS_CLOSED);

	// read the content of the test file
	if (SSL_OK != file_read_all(__test_text_file_path, &test_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	TEST_PASS();
}

typedef struct __s_b64_transform_ctx {
	int final;
	int done;
} __t_b64_transform_ctx;

static t_transform_result __b64_transform(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	const size_t B64_IN_BLOCK_SIZE = 3;
	const size_t B64_OUT_BLOCK_SIZE = 4;
	const char	SM[64] = {
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
		'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', '+', '/'
	};
	__t_b64_transform_ctx *b64_ctx = vctx;

	if (NULL == in || NULL == out || NULL == b64_ctx) {
		TEST_LOG(DEBUG, "transform validation: TRANSFORM_ERROR");
		return (t_transform_result){.status = TRANSFORM_ERROR};
	}
	if (b64_ctx->done) {
		TEST_LOG(DEBUG, "transform validation: already TRANSFORM_DONE");
		return (t_transform_result){.status = TRANSFORM_DONE};
	}

	const uint8_t *mesblock = in;
	uint8_t *encblock = out;
	size_t i = 0;
	size_t j = 0;

	if (!b64_ctx->final) {
		if (insize < B64_IN_BLOCK_SIZE) {
			TEST_LOG(DEBUG, "transform update: TRANSFORM_NEED_INPUT");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_INPUT};
		}
		if (outsize < B64_OUT_BLOCK_SIZE) {
			TEST_LOG(DEBUG, "transform update: TRANSFORM_NEED_OUTPUT");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
		}
		while (i+2 < insize && j+3 < outsize) {
			encblock[j+0] = SM[( ( mesblock[i+0]>>2 )&0x3F )];
			encblock[j+1] = SM[( ( mesblock[i+0]<<4 )&0x30 )|( ( mesblock[i+1]>>4 )&0xF )];
			encblock[j+2] = SM[( ( mesblock[i+1]<<2 )&0x3C )|( ( mesblock[i+2]>>6 )&0x3 )];
			encblock[j+3] = SM[( mesblock[i+2]&0x3F )];
			i += B64_IN_BLOCK_SIZE;
			j += B64_OUT_BLOCK_SIZE;
		}
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
	}
	else {
		if (outsize < B64_OUT_BLOCK_SIZE) {
			TEST_LOG(DEBUG, "transform update: TRANSFORM_NEED_OUTPUT");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_NEED_OUTPUT};
		}
		while (i+2 < insize && j+3 < outsize) {
			encblock[j+0] = SM[( ( mesblock[i+0]>>2 )&0x3F )];
			encblock[j+1] = SM[( ( mesblock[i+0]<<4 )&0x30 )|( ( mesblock[i+1]>>4 )&0xF )];
			encblock[j+2] = SM[( ( mesblock[i+1]<<2 )&0x3C )|( ( mesblock[i+2]>>6 )&0x3 )];
			encblock[j+3] = SM[( mesblock[i+2]&0x3F )];
			i += B64_IN_BLOCK_SIZE;
			j += B64_OUT_BLOCK_SIZE;
		}
		if (j + B64_OUT_BLOCK_SIZE < outsize) {
			if (i+2 < insize) {
				encblock[j+0] = SM[( ( mesblock[i+0]>>2 )&0x3F )];
				encblock[j+1] = SM[( ( mesblock[i+0]<<4 )&0x30 )|( ( mesblock[i+1]>>4 )&0xF )];
				encblock[j+2] = SM[( ( mesblock[i+1]<<2 )&0x3C )|( ( mesblock[i+2]>>6 )&0x3 )];
				encblock[j+3] = SM[( mesblock[i+2]&0x3F )];
				i += 3;
			}
			else if (i+1 < insize) {
				encblock[j+0] = SM[( ( mesblock[i+0]>>2 )&0x3F )];
				encblock[j+1] = SM[( ( mesblock[i+0]<<4 )&0x30 )|( ( mesblock[i+1]>>4 )&0xF )];
				encblock[j+2] = SM[( ( mesblock[i+1]<<2 )&0x3C )];
				encblock[j+3] = '=';
				i += 2;
			}
			else if (i < insize) {
				encblock[j+0] = SM[( ( mesblock[i+0]>>2 )&0x3F )];
				encblock[j+1] = SM[( ( mesblock[i+0]<<4 )&0x30 )];
				encblock[j+2] = '=';
				encblock[j+3] = '=';
				i += 1;
			}
			else {
				encblock[j+0] = '=';
				encblock[j+1] = '=';
				encblock[j+2] = '=';
				encblock[j+3] = '=';
			}
			j += B64_OUT_BLOCK_SIZE;
			b64_ctx->done = 1;

			TEST_LOG(DEBUG, "transform final: TRANSFORM_DONE");
			return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_DONE};
		}
		return (t_transform_result){.consumed = i, .produced = j, .status = TRANSFORM_OK};
	}
}

static t_transform_result __b64_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	__t_b64_transform_ctx *b64_ctx = vctx;
	b64_ctx->final = 0;
	return __b64_transform(b64_ctx, in, insize, out, outsize);
}

static t_transform_result __b64_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	__t_b64_transform_ctx *b64_ctx = vctx;
	b64_ctx->final = 1;
	return __b64_transform(b64_ctx, in, insize, out, outsize);
}

static t_transform_result __transform_mock_error(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	return (t_transform_result){.status = TRANSFORM_ERROR};
}

static int	__test_io_filter_reader(void)
{
	t_io_v2_stream	*filter_reader, *upstream;
	t_ostring		test_content, ref_content, enc_content;

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);
	ft_ostr_init(&enc_content);

	size_t bufsize = 1024;
	char *buf;
	SSL_ALLOC(buf, bufsize);

	const char *__bin_file_path = "tests/files/base64/message.bin";
	const char *__b64_file_path = "tests/files/base64/message.txt";

	if (SSL_OK != file_read_all(__b64_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	// 1. read is successful
	if (SSL_OK != io_v2_file_reader(&upstream, __bin_file_path)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	__t_b64_transform_ctx b64_ctx = {0};

	if (SSL_OK != io_v2_filter_reader(&filter_reader, upstream, __b64_transform_update, __b64_transform_final, &b64_ctx)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(filter_reader->status == IO_V2_STATUS_OK);
	TEST_ASSERT(filter_reader->interface.read != NULL);
	TEST_ASSERT(filter_reader->interface.write == NULL);
	TEST_ASSERT(filter_reader->interface.close != NULL);
	TEST_ASSERT(filter_reader->interface.flush == NULL);
	TEST_ASSERT(filter_reader->ctx != NULL);

	ssize_t	rbytes = 0;
	while (1) {
		rbytes = io_v2_read(filter_reader, buf, bufsize);
		if (rbytes < 0) {
			break ;
		}
		ft_ostr_append(&test_content, buf, rbytes);
	}
	TEST_ASSERT(filter_reader->status == IO_V2_STATUS_EOF);
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);
	// read should error since stream is at EOF
	rbytes = io_v2_read(filter_reader, buf, bufsize);
	TEST_ASSERT(rbytes < 0);

	// 2. close ok
	ssize_t ret = io_v2_close(filter_reader);
	TEST_ASSERT(ret == 0);
	TEST_ASSERT(filter_reader->status == IO_V2_STATUS_CLOSED);
	TEST_ASSERT(upstream->status == IO_V2_STATUS_CLOSED);
	// read should error since stream is closed
	rbytes = io_v2_read(filter_reader, buf, bufsize);
	TEST_ASSERT(rbytes < 0);

	// 3. read error
	if (SSL_OK != io_v2_file_reader(&upstream, __bin_file_path)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	b64_ctx = (__t_b64_transform_ctx){0};

	if (SSL_OK != io_v2_filter_reader(&filter_reader, upstream, __transform_mock_error, __transform_mock_error, &b64_ctx)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	rbytes = 0;
	while (1) {
		rbytes = io_v2_read(filter_reader, buf, bufsize);
		if (rbytes < 0) {
			break ;
		}
		ft_ostr_append(&test_content, buf, rbytes);
	}
	TEST_ASSERT(filter_reader->status == IO_V2_STATUS_ERROR);
	// read should error since stream is in error
	rbytes = io_v2_read(filter_reader, buf, bufsize);
	TEST_ASSERT(rbytes < 0);

	TEST_PASS();
}

static int	__test_io_filter_writer(void)
{
	t_io_v2_stream	*filter_writer, *downstream;
	t_ostring		test_content, ref_content, enc_content;

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);
	ft_ostr_init(&enc_content);

	const char *__bin_file_path = "tests/files/base64/message.bin";
	const char *__b64_file_path = "tests/files/base64/message.txt";

	if (SSL_OK != file_read_all(__b64_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}
	if (SSL_OK != file_read_all(__bin_file_path, &test_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	// 1. write is successful
	if (SSL_OK != io_v2_bytes_writer(&downstream, &enc_content)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	__t_b64_transform_ctx b64_ctx = {0};

	if (SSL_OK != io_v2_filter_writer(&filter_writer, downstream, __b64_transform_update, __b64_transform_final, &b64_ctx)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_OK);
	TEST_ASSERT(filter_writer->interface.read == NULL);
	TEST_ASSERT(filter_writer->interface.write != NULL);
	TEST_ASSERT(filter_writer->interface.finish != NULL);
	TEST_ASSERT(filter_writer->interface.close != NULL);
	TEST_ASSERT(filter_writer->interface.flush != NULL);
	TEST_ASSERT(filter_writer->ctx != NULL);

	const size_t B64_IN_BLOCK_SIZE = 3;

	ssize_t	wbytes = 0;
	uint8_t *buf = test_content.content;
	size_t bufsize = test_content.size;
	while (bufsize >= B64_IN_BLOCK_SIZE) {
		wbytes = io_v2_write(filter_writer, buf, bufsize);
		if (wbytes < 0) {
			break ;
		}
		if (wbytes > bufsize) {
			TEST_LOG(ERROR, UNEXPECTED_ERROR);
			TEST_FAIL();
		}
		buf += wbytes;
		bufsize -= wbytes;
	}
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_OK);
	// TEST_LOG(DEBUG, "filter write update: enc_content.size = %zu", enc_content.size);
	// TEST_LOG(DEBUG, "filter write update: enc_content.content = `%s`", ft_ostr_to_cstr(&enc_content, 0, enc_content.size));

	wbytes = io_v2_finish(filter_writer);
	TEST_ASSERT(wbytes > 0);
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_FINISHED);
	// TEST_LOG(DEBUG, "filter write finish: enc_content.size = %zu", enc_content.size);
	// TEST_LOG(DEBUG, "filter write finish: ref_content.size = %zu", ref_content.size);
	// TEST_LOG(DEBUG, "filter write finish: enc_content.content = `%s`", ft_ostr_to_cstr(&enc_content, 0, enc_content.size));
	// TEST_LOG(DEBUG, "filter write finish: ref_content.content = `%s`", ft_ostr_to_cstr(&ref_content, 0, ref_content.size));
	TEST_ASSERT(enc_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(enc_content.content, ref_content.content, ref_content.size) == 0);
	// write should error since stream is at FINISHED
	wbytes = io_v2_write(filter_writer, "banana", 6);
	TEST_LOG(DEBUG, "filter write close: wbytes = %zd", wbytes);
	TEST_ASSERT(wbytes < 0);
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_FINISHED);

	// 2. close ok
	ssize_t ret = io_v2_close(filter_writer);
	TEST_ASSERT(ret == 0);
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_CLOSED);
	TEST_ASSERT(downstream->status == IO_V2_STATUS_CLOSED);
	// write should error since stream is closed
	wbytes = io_v2_write(filter_writer, "banana", 6);
	TEST_ASSERT(wbytes < 0);

	// 3. write error
	ft_ostr_init(&enc_content);
	if (SSL_OK != io_v2_bytes_writer(&downstream, &enc_content)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	b64_ctx = (__t_b64_transform_ctx){0};

	if (SSL_OK != io_v2_filter_writer(&filter_writer, downstream, __transform_mock_error, __transform_mock_error, &b64_ctx)) {
		TEST_LOG(ERROR, IO_INIT_ERROR);
		TEST_FAIL();
	}
	buf = test_content.content;
	bufsize = test_content.size;
	while (bufsize >= B64_IN_BLOCK_SIZE) {
		wbytes = io_v2_write(filter_writer, buf, bufsize);
		if (wbytes < 0) {
			break ;
		}
		if (wbytes > bufsize) {
			TEST_LOG(ERROR, UNEXPECTED_ERROR);
			TEST_FAIL();
		}
		buf += wbytes;
		bufsize -= wbytes;
	}
	TEST_ASSERT(filter_writer->status == IO_V2_STATUS_ERROR);
	// read should error since stream is in error
	wbytes = io_v2_write(filter_writer, buf, bufsize);
	TEST_ASSERT(wbytes < 0);

	TEST_PASS();
}

static int	__test_io_pipe_unidir(void)
{
	t_io_v2_pipe	*pipe;
	t_io_v2_stream	*upstream, *downstream;
	t_ostring		test_content, ref_content;
	int				ret;

	ft_ostr_init_with_capacity(&test_content, IO_BUFSIZE * 128);
	ft_ostr_init_with_capacity(&ref_content, IO_BUFSIZE * 128);

	if (SSL_OK != file_read_all(__medium_text_file_path, &ref_content)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	// 1. Happy path
	if (SSL_OK != io_v2_bytes_reader(&upstream, &ref_content)) {
		TEST_LOG(ERROR, "failed to create bytes reader");
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_bytes_writer(&downstream, &test_content)) {
		TEST_LOG(ERROR, "failed to create bytes writer");
		TEST_FAIL();
	}
	ret = io_v2_pipe_unidir(&pipe, upstream, downstream);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_OK);
	TEST_ASSERT(pipe->type == IO_V2_PIPE_TYPE_UNIDIR);
	TEST_ASSERT(pipe->ctx != NULL);
	// pump must be successful
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret == 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_OK);
	// consecutive pumps must be successful
	while (pipe->status == IO_V2_STATUS_PIPE_OK) {
		if (io_v2_pipe_pump(pipe) < 0) {
			break;
		}
	}
	// pipe must be at END
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_END);
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, test_content.size) == 0);
	// delete must be successful
	ret = io_v2_pipe_del(pipe);
	TEST_ASSERT(ret == 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_END);
	TEST_ASSERT(pipe->ctx == NULL);

	// 2. Error path
	if (SSL_OK != io_v2_bytes_reader(&upstream, &ref_content)) {
		TEST_LOG(ERROR, "failed to create bytes reader");
		TEST_FAIL();
	}
	if (SSL_OK != io_v2_bytes_writer(&downstream, &test_content)) {
		TEST_LOG(ERROR, "failed to create bytes writer");
		TEST_FAIL();
	}
	ret = io_v2_pipe_unidir(&pipe, upstream, downstream);
	TEST_ASSERT(SSL_OK == ret);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_OK);
	TEST_ASSERT(pipe->type == IO_V2_PIPE_TYPE_UNIDIR);
	TEST_ASSERT(pipe->ctx != NULL);
	// pump must fail
	upstream->status = IO_V2_STATUS_ERROR;
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret < 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_ERROR);
	// pump must fail
	upstream->status = IO_V2_STATUS_CLOSED;
	pipe->status = IO_V2_STATUS_PIPE_OK;
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret < 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_ERROR);
	// pump must fail
	upstream->status = IO_V2_STATUS_OK;
	downstream->status = IO_V2_STATUS_ERROR;
	pipe->status = IO_V2_STATUS_PIPE_OK;
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret < 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_ERROR);
	// pump must fail
	upstream->status = IO_V2_STATUS_OK;
	downstream->status = IO_V2_STATUS_CLOSED;
	pipe->status = IO_V2_STATUS_PIPE_OK;
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret < 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_ERROR);
	// pump must fail
	upstream->status = IO_V2_STATUS_OK;
	downstream->status = IO_V2_STATUS_FINISHED;
	pipe->status = IO_V2_STATUS_PIPE_OK;
	ret = io_v2_pipe_pump(pipe);
	TEST_ASSERT(ret < 0);
	TEST_ASSERT(pipe->status == IO_V2_STATUS_PIPE_ERROR);

	TEST_PASS();
}
