#include <common.h>
#include <logger.h>
#include <base64.h>
#include <file.h>
#include "test.h"

static const char	*__binary_file_path = "tests/files/base64/message.bin";
static const char	*__base64_file_path = "tests/files/base64/message.txt";

static t_ostring	__binary;
static t_ostring	__base64;

static int	__test_base64_setup(void);
static int	__test_base64_encode_all(void);
static int	__test_base64_encode_transform(void);
static int	__test_base64_decode_all(void);
static int	__test_base64_decode_transform(void);

int	test_base64(void)
{
	if (SSL_OK != __test_base64_setup()) {
		TEST_LOG(ERROR, TEST_SETUP_ERROR);
		TEST_FAIL();
	}

	return (
		__test_base64_encode_all()
		| __test_base64_encode_transform()
		| __test_base64_decode_all()
		| __test_base64_decode_transform()
	);
}

static int	__test_base64_setup(void)
{
	if (SSL_OK != file_read_all(__binary_file_path, &__binary)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (SSL_OK != file_read_all(__base64_file_path, &__base64)) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}
	if (__binary.size == 0 || __base64.size == 0) {
		TEST_LOG(ERROR, FILE_READ_ERROR);
		return (SSL_ERR);
	}

	return (SSL_OK);
}

static int	__test_base64_encode_all(void)
{
	uint8_t	*out;
	size_t	outsize;
	int		ret_val;

	ret_val = base64_encode_all(__binary.content, __binary.size, &out, &outsize);

	TEST_ASSERT(SSL_OK == ret_val);
	TEST_ASSERT(NULL != out);
	TEST_ASSERT(outsize == __base64.size);
	TEST_ASSERT(ft_memcmp(out, __base64.content, outsize) == 0);

	TEST_PASS();
}

static int	__test_base64_encode_transform(void)
{
	size_t insize = __binary.size;
	uint8_t *in = __binary.content;
	size_t outsize = insize * 2;
	uint8_t	*out = NULL;
	SSL_ALLOC(out, outsize);
	t_b64_ctx ctx = {0};

	// base64 update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = base64_encode_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
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
	size_t expected_consumed = (insize/B64_MES_BLOCK_SIZE)*B64_MES_BLOCK_SIZE;
	size_t expected_produced = (expected_consumed/B64_MES_BLOCK_SIZE)*B64_ENC_BLOCK_SIZE;
	TEST_LOG(DEBUG, "tconsumed=%zu, expected=%zu", tconsumed, expected_consumed);
	TEST_LOG(DEBUG, "tproduced=%zu, expected=%zu", tproduced, expected_produced);
	TEST_ASSERT(tconsumed == expected_consumed);
	TEST_ASSERT(tproduced == expected_produced);

	// base64 final
	while (1) {
 		result = base64_encode_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
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
	TEST_ASSERT(tproduced == __base64.size);
	TEST_ASSERT(ft_memcmp(out, __base64.content, __base64.size) == 0);

	TEST_PASS();
}

static int	__test_base64_decode_all(void)
{
	uint8_t	*out;
	size_t	outsize;
	int		ret_val;

	ret_val = base64_decode_all(__base64.content, __base64.size, &out, &outsize);

	TEST_ASSERT(SSL_OK == ret_val);
	TEST_ASSERT(NULL != out);
	TEST_ASSERT(outsize == __binary.size);
	TEST_ASSERT(!ft_memcmp(out, __binary.content, outsize));

	TEST_PASS();
}

static int	__test_base64_decode_transform(void)
{
	size_t insize = __base64.size;
	uint8_t *in = __base64.content;
	size_t outsize = insize * 2;
	uint8_t *out = NULL;
	SSL_ALLOC(out, outsize);
	t_b64_ctx ctx = {0};

	// base64 update
	t_transform_result result = {0};
	ssize_t tconsumed = 0, tproduced = 0;
	while (1) {
 		result = base64_decode_transform_update(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
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
	size_t expected_consumed;
	if (insize > 0 && insize%B64_ENC_BLOCK_SIZE == 0) {
		expected_consumed = ((insize/B64_ENC_BLOCK_SIZE)-1)*B64_ENC_BLOCK_SIZE;
	} else {
		expected_consumed = (insize/B64_ENC_BLOCK_SIZE)*B64_ENC_BLOCK_SIZE;
	}
	size_t expected_produced = (expected_consumed/B64_ENC_BLOCK_SIZE)*B64_MES_BLOCK_SIZE;
	TEST_LOG(DEBUG, "tconsumed=%zu, expected=%zu", tconsumed, expected_consumed);
	TEST_LOG(DEBUG, "tproduced=%zu, expected=%zu", tproduced, expected_produced);
	TEST_ASSERT(tconsumed == expected_consumed);
	TEST_ASSERT(tproduced == expected_produced);

	// base64 final
	while (1) {
 		result = base64_decode_transform_final(&ctx, in+tconsumed, insize-tconsumed, out+tproduced, outsize-tproduced);
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
	TEST_ASSERT(tproduced == __binary.size);
	TEST_ASSERT(ft_memcmp(out, __binary.content, __binary.size) == 0);

	TEST_PASS();
}
