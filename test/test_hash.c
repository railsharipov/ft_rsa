#include <common.h>
#include <asn.h>
#include <der.h>
#include <bnum.h>
#include <rand.h>
#include <test.h>
#include <io.h>

static int	__test_hash_setup(void);
static void	__test_hash_cleanup(void);

static int	__test_hash_md5(void);
static int	__test_hash_sha1(void);
static int	__test_hash_sha224(void);
static int	__test_hash_sha256(void);
static int	__test_hash_sha384(void);
static int	__test_hash_sha512(void);

static const char	*__text_file_path = "test/files/text/large.txt";
static t_ostring	__text_file;

static const char	*__md5_hash = "89a1081a26676f7fdf65ae3eddcbf0b1";
static const char	*__sha1_hash = "4f157b1875c07993e4938409936a28c16d544c06";
static const char	*__sha224_hash = "ed36e9f7d87966ba791d78a43320c11a5f59dafe276d20455bb0b151";
static const char	*__sha256_hash = "4574984f6ad330f87c264f5e10e36763cb64629bac449e562dde521ac600a946";
static const char	*__sha384_hash = "1046c7e6a928dbfc40ebca930b203ba2abcbf177a6fe552f17cf57df81c965021f4eeaef22e676780ccba8dcd69ed2df";
static const char	*__sha512_hash = "d2960cebf987d8df1c669cdfb2a7935e804e86de4f9bb7a21c8d306bfbda28d340bb6f723e54bd5d8ead1a1f9dbe7fcb7c75475f4fbb65327b2a2fee6996cc94";

int	test_hash(void)
{
	if (SSL_OK != __test_hash_setup()) {
		TEST_LOG(ERROR, "failed to setup test");
		TEST_FAIL();
	}

	__test_hash_cleanup();

    return (__test_hash_md5()
        | __test_hash_sha1()
        | __test_hash_sha224()
        | __test_hash_sha256()
        | __test_hash_sha384()
        | __test_hash_sha512());
}

static int	__test_hash_setup(void)
{
    if (SSL_OK != test_read_file(__text_file_path, &__text_file)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	return (SSL_OK);
}

static void	__test_hash_cleanup(void)
{
	return ;
}

static int	__test_hash_md5(void)
{
	TEST_ASSERT(__md5_hash);
	TEST_PASS();
}

static int	__test_hash_sha1(void)
{
	TEST_ASSERT(__sha1_hash);
	TEST_PASS();
}

static int	__test_hash_sha224(void)
{
	TEST_ASSERT(__sha224_hash);
	TEST_PASS();
}

static int	__test_hash_sha256(void)
{
	TEST_ASSERT(__sha256_hash);
	TEST_PASS();
}

static int	__test_hash_sha384(void)
{
	TEST_ASSERT(__sha384_hash);
	TEST_PASS();
}

static int	__test_hash_sha512(void)
{
	TEST_ASSERT(__sha512_hash);
	TEST_PASS();
}