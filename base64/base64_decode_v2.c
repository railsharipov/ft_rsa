#include <common.h>
#include <logger.h>
#include <base64.h>
#include <libft.h>

t_transform_result base64_decode_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;
	return (t_transform_result){.status = TRANSFORM_OK};
}

t_transform_result base64_decode_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize)
{
	t_b64_ctx *ctx = vctx;
	return (t_transform_result){.status = TRANSFORM_OK};
}
