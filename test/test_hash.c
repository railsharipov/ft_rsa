#include <common.h>
#include <hash.h>
#include <test.h>
#include <io.h>
#include <libft/bytes.h>

static int	__test_hash_setup(void);

static int	__test_hash_md5(void);
static int	__test_hash_sha1(void);
static int	__test_hash_sha224(void);
static int	__test_hash_sha256(void);
static int	__test_hash_sha384(void);
static int	__test_hash_sha512(void);

static const char	*__large_text_file_path = "test/files/text/large.txt";

static const char	*__md5_hash_hex = "89a1081a26676f7fdf65ae3eddcbf0b1";
static const char	*__sha1_hash_hex = "4f157b1875c07993e4938409936a28c16d544c06";
static const char	*__sha224_hash_hex = "ed36e9f7d87966ba791d78a43320c11a5f59dafe276d20455bb0b151";
static const char	*__sha256_hash_hex = "4574984f6ad330f87c264f5e10e36763cb64629bac449e562dde521ac600a946";
static const char	*__sha384_hash_hex = "1046c7e6a928dbfc40ebca930b203ba2abcbf177a6fe552f17cf57df81c965021f4eeaef22e676780ccba8dcd69ed2df";
static const char	*__sha512_hash_hex = "d2960cebf987d8df1c669cdfb2a7935e804e86de4f9bb7a21c8d306bfbda28d340bb6f723e54bd5d8ead1a1f9dbe7fcb7c75475f4fbb65327b2a2fee6996cc94";

int	test_hash(void)
{
	if (SSL_OK != __test_hash_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

    return (__test_hash_md5()
        | __test_hash_sha1()
        | __test_hash_sha224()
        | __test_hash_sha256()
        | __test_hash_sha384()
        | __test_hash_sha512());
}

static int	__test_hash_setup(void)
{
	return (SSL_OK);
}

static int	__test_hash_md5(void)
{
	t_hash	md5;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_md5_init(&md5);
	hash_md5_update_stream(&md5, &iodes);
	hash_md5_final(&md5);

	io_fclose(&iodes);

	TEST_ASSERT(md5.hashsize == MD5_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(md5.hash, MD5_HASH_SIZE), __md5_hash_hex, MD5_HASH_SIZE * 2));
	TEST_PASS();
}

static int	__test_hash_sha1(void)
{
	t_hash	sha1;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_sha1_init(&sha1);
	hash_sha1_update_stream(&sha1, &iodes);
	hash_sha1_final(&sha1);

	io_fclose(&iodes);

	TEST_ASSERT(sha1.hashsize == SHA1_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(sha1.hash, SHA1_HASH_SIZE), __sha1_hash_hex, SHA1_HASH_SIZE * 2));
	TEST_PASS();
}

static int	__test_hash_sha224(void)
{
	t_hash	sha224;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_sha224_init(&sha224);
	hash_sha224_update_stream(&sha224, &iodes);
	hash_sha224_final(&sha224);

	io_fclose(&iodes);

	TEST_ASSERT(sha224.hashsize == SHA224_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(sha224.hash, SHA224_HASH_SIZE), __sha224_hash_hex, SHA224_HASH_SIZE * 2));
	TEST_PASS();
}

static int	__test_hash_sha256(void)
{
	t_hash	sha256;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_sha256_init(&sha256);
	hash_sha256_update_stream(&sha256, &iodes);
	hash_sha256_final(&sha256);

	io_fclose(&iodes);

	TEST_ASSERT(sha256.hashsize == SHA256_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(sha256.hash, SHA256_HASH_SIZE), __sha256_hash_hex, SHA256_HASH_SIZE * 2));
	TEST_PASS();
}

static int	__test_hash_sha384(void)
{
	t_hash	sha384;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_sha384_init(&sha384);
	hash_sha384_update_stream(&sha384, &iodes);
	hash_sha384_final(&sha384);

	io_fclose(&iodes);

	TEST_ASSERT(sha384.hashsize == SHA384_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(sha384.hash, SHA384_HASH_SIZE), __sha384_hash_hex, SHA384_HASH_SIZE * 2));
	TEST_PASS();
}

static int	__test_hash_sha512(void)
{
	t_hash	sha512;
	t_iodes	iodes;

	if (SSL_OK != io_fopen(&iodes, IO_READ|IO_FILE, __large_text_file_path)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	hash_sha512_init(&sha512);
	hash_sha512_update_stream(&sha512, &iodes);
	hash_sha512_final(&sha512);

	io_fclose(&iodes);

	TEST_ASSERT(sha512.hashsize == SHA512_HASH_SIZE);
	TEST_ASSERT(ft_strneq(ft_bytes_to_hex(sha512.hash, SHA512_HASH_SIZE), __sha512_hash_hex, SHA512_HASH_SIZE * 2));
	TEST_PASS();
}