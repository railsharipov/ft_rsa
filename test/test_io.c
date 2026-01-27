#include <common.h>
#include <io.h>
#include <file.h>
#include <test.h>
#include <libft/string.h>

static int	__test_io_setup(void);
static int	__test_io_file_read(void);
static int	__test_io_file_write(void);
static int	__test_io_buffered_reader(void);
static int	__test_io_buffered_writer(void);

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
		| __test_io_buffered_reader()
		| __test_io_buffered_writer()
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

static int	__test_io_buffered_reader(void)
{
	t_io_v2_stream	*buffered_stream, *upstream;
	t_ostring		test_content, ref_content;
	size_t			capacity = 1024;
	ssize_t			rbytes, tbytes;
	char 			*buf;
	size_t			bufsize;
	int				ret;

	/* ****************************************************************** */
	/* Case 1: read size is less than the capacity of the buffered stream */
	/* ****************************************************************** */

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

	/* ********************************************************************* */
	/* Case 2: read size is greater than the capacity of the buffered stream */
	/* ********************************************************************* */

	ft_ostr_init(&test_content);
	ft_ostr_init(&ref_content);

	bufsize = 10 * 1024;
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

	/* ******************************************************************* */
	/* Case 1: write size is less than the capacity of the buffered stream */
	/* ******************************************************************* */

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

	/* ********************************************************************** */
	/* Case 2: write size is greater than the capacity of the buffered stream */
	/* ********************************************************************** */

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