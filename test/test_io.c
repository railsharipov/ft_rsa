#include <common.h>
#include <io.h>
#include <file.h>
#include <test.h>
#include <libft/string.h>

static int	__test_io_setup(void);
static int	__test_io_file_read(void);
static int	__test_io_file_write(void);

static const char	*__small_text_file_path = "test/files/text/small.txt";
static const char	*__large_text_file_path = "test/files/text/large.txt";

static const char	*__test_text_file_path = "test/files/text/test.txt";

int	test_io(void)
{
	if (SSL_OK != __test_io_setup()) {
		TEST_LOG(ERROR, UNSPECIFIED_ERROR);
		TEST_FAIL();
	}

	return (
		__test_io_file_read()
		| __test_io_file_write()
	);
}

static int	__test_io_setup(void)
{
	return (SSL_OK);
}

static int	__test_io_file_read(void)
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

	ret = file_reader(&stream, __small_text_file_path);
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

	ret = file_reader(&stream, __large_text_file_path);
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

static int	__test_io_file_write(void)
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

	ret = file_writer(&stream, __test_text_file_path);
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

	ret = file_writer(&stream, __test_text_file_path);
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
	ft_printf("test_content.size=%zu, ref_content.size=%zu\n", test_content.size, ref_content.size);
	TEST_ASSERT(test_content.size == ref_content.size);
	TEST_ASSERT(ft_memcmp(test_content.content, ref_content.content, ref_content.size) == 0);

	ft_ostr_clear(&ref_content);
	ft_ostr_clear(&test_content);

	TEST_PASS();
}