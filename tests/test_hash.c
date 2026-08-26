#include <common.h>
#include <logger.h>
#include <digest.h>
#include <io.h>
#include <file.h>
#include <libft.h>

#include "test.h"

static int	__test_hash_setup(void);

static int	__test_hash_md5(void);
static int	__test_md5_transform(void);

static int	__test_hash_sha1(void);
static int	__test_sha1_transform(void);

static int	__test_hash_sha256(void);
static int	__test_sha256_transform(void);
static int	__test_sha224_transform(void);

static int	__test_hash_sha512(void);
static int	__test_sha512_transform(void);
static int	__test_sha384_transform(void);

static const char	*__small_text_file_path = "tests/files/text/small.txt";
static const char	*__large_text_file_path = "tests/files/text/large.txt";

static const char	*__small_text_md5_hash_hex = "41ea606938d6dfdeab2ac96a08114598";
static const char	*__small_text_sha1_hash_hex = "2d3302cfd46bedd06600433e04325c9779a30710";
static const char	*__small_text_sha256_hash_hex = "e3cc9b1cbd3c9a0cea76629b3951fd412df5dc8810f47ef498a76f30d6d8ca18";
static const char	*__small_text_sha512_hash_hex = "2c38c8a19dddbc1c3187e54ceb55a0634db2f541bc11c2db2970e1cb8f37be450d5be343dcaba9db2d7372832f399841d7fbf40c5acc1ea81237303a1891521c";

static const char	*__large_text_md5_hash_hex = "54500f5736bb406b1414a38997bf4b9b";
static const char	*__large_text_sha1_hash_hex = "f6e52b13d88233e73294e2b14bbdcb147abe7cc8";
static const char	*__large_text_sha224_hash_hex = "99f1e118af8e1298c7d08da62c0b6e84f5d69f634fdac7447183bd1d";
static const char	*__large_text_sha256_hash_hex = "d148da3b9e486b4d6b7cbbf254e1ac01ca1d0cf6aa639b28db64bc9b19841d8c";
static const char	*__large_text_sha384_hash_hex = "0bd865253f4fc9084bdcba4603c81e5fb7b387f11f9ab91cf74d92a2f6d2e165aa2558c00a5fba9ff5372214779c1ecf";
static const char	*__large_text_sha512_hash_hex = "a4ea60088ba626a8bcbe47891ca25708f8ff789aff305620573d9969fdb4d1ca1ed8812f70a9fe5b885cfb0a193952cede38f81708479bcaf36176c7ee4749f5";

int	test_hash(void)
{
	if (SSL_OK != __test_hash_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	return __test_hash_md5()
		| __test_md5_transform()
        | __test_hash_sha1()
		| __test_sha1_transform()
        | __test_hash_sha256()
        | __test_sha256_transform()
        | __test_sha224_transform()
        | __test_hash_sha512()
        | __test_sha512_transform()
        | __test_sha384_transform()
    	;
}

static int	__test_hash_setup(void)
{
	return (SSL_OK);
}

static int	__test_hash_md5(void)
{
	t_ostring in;
	if (SSL_OK != file_read_all(__small_text_file_path, &in)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	t_hash ctx = {0};
	md5_init(&ctx);
	TEST_ASSERT(ctx.blocksize == MD5_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == MD5_HASH_SIZE);

	size_t rbytes = 0;
	while (rbytes + MD5_BLOCK_SIZE <= in.size) {
		md5_update_block(&ctx, in.content + rbytes);
		rbytes += MD5_BLOCK_SIZE;
	}
	md5_final_block(&ctx, in.content + rbytes, in.size - rbytes);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __small_text_md5_hash_hex));

	TEST_PASS();
}

static int	__test_md5_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	md5_init(&ctx);
	TEST_ASSERT(ctx.blocksize == MD5_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == MD5_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = md5_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = md5_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_md5_hash_hex));

	TEST_PASS();
}

static int	__test_hash_sha1(void)
{
	t_ostring in;
	if (SSL_OK != file_read_all(__small_text_file_path, &in)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	t_hash ctx = {0};
	sha1_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA1_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA1_HASH_SIZE);

	size_t rbytes = 0;
	while (rbytes + SHA1_BLOCK_SIZE <= in.size) {
		sha1_update_block(&ctx, in.content + rbytes);
		rbytes += SHA1_BLOCK_SIZE;
	}
	sha1_final_block(&ctx, in.content + rbytes, in.size - rbytes);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __small_text_sha1_hash_hex));

	TEST_PASS();
}

static int	__test_sha1_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	sha1_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA1_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA1_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = sha1_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = sha1_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_sha1_hash_hex));

	TEST_PASS();
}

static int	__test_hash_sha256(void)
{
	t_ostring in;
	if (SSL_OK != file_read_all(__small_text_file_path, &in)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	t_hash ctx = {0};
	sha256_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA256_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA256_HASH_SIZE);

	size_t rbytes = 0;
	while (rbytes + SHA256_BLOCK_SIZE <= in.size) {
		sha256_update_block(&ctx, in.content + rbytes);
		rbytes += SHA256_BLOCK_SIZE;
	}
	sha256_final_block(&ctx, in.content + rbytes, in.size - rbytes);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __small_text_sha256_hash_hex));

	TEST_PASS();
}

static int	__test_sha256_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	sha256_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA256_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA256_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = sha256_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = sha256_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_sha256_hash_hex));

	TEST_PASS();
}

static int	__test_sha224_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	sha224_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA256_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA224_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = sha224_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = sha224_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_sha224_hash_hex));

	TEST_PASS();
}

static int	__test_hash_sha512(void)
{
	t_ostring in;
	if (SSL_OK != file_read_all(__small_text_file_path, &in)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	t_hash ctx = {0};
	sha512_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA512_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA512_HASH_SIZE);

	size_t rbytes = 0;
	while (rbytes + SHA512_BLOCK_SIZE <= in.size) {
		sha512_update_block(&ctx, in.content + rbytes);
		rbytes += SHA512_BLOCK_SIZE;
	}
	sha512_final_block(&ctx, in.content + rbytes, in.size - rbytes);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __small_text_sha512_hash_hex));

	TEST_PASS();
}

static int	__test_sha512_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	sha512_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA512_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA512_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = sha512_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = sha512_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_sha512_hash_hex));

	TEST_PASS();
}

static int	__test_sha384_transform(void)
{
	t_ostring input;
	if (SSL_OK != file_read_all(__large_text_file_path, &input)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		TEST_FAIL();
	}

	size_t insize = input.size;
	uint8_t *in = input.content;
	size_t outsize = insize;
	uint8_t	out[insize];

	t_hash ctx = {0};
	sha384_init(&ctx);
	TEST_ASSERT(ctx.blocksize == SHA512_BLOCK_SIZE);
	TEST_ASSERT(ctx.hashsize == SHA384_HASH_SIZE);

	// update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = sha384_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
 		if (result.status == TRANSFORM_ERROR) {
   			break;
   		}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
   		if (result.status != TRANSFORM_OK) {
     		break;
     	}
	}
	TEST_ASSERT(result.status != TRANSFORM_ERROR);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	// final
	while (1) {
 		result = sha384_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
   		if (result.status == TRANSFORM_ERROR) {
     		break;
     	}
     	tconsumed += result.consumed;
     	tproduced += result.produced;
  		if (result.status != TRANSFORM_OK) {
      		break;
      	}
	}
	TEST_ASSERT(result.status == TRANSFORM_DONE);
	TEST_ASSERT(tconsumed == insize);
	TEST_ASSERT(tproduced == insize);

	char *hex = ft_bytes_to_hex(ctx.hash, ctx.hashsize);
	TEST_ASSERT(ft_streq(hex, __large_text_sha384_hash_hex));

	TEST_PASS();
}
