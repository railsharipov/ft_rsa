#ifndef DIGEST_H
# define DIGEST_H

# include <common.h>
# include <logger.h>
# include <libft.h>

# define MD5_BLOCK_SIZE			64
# define SHA1_BLOCK_SIZE		64
# define SHA256_BLOCK_SIZE		64
# define SHA512_BLOCK_SIZE		128

# define MD5_HASH_SIZE			16
# define SHA1_HASH_SIZE			20
# define SHA224_HASH_SIZE		28
# define SHA256_HASH_SIZE		32
# define SHA384_HASH_SIZE		48
# define SHA512_224_HASH_SIZE	28
# define SHA512_256_HASH_SIZE	32
# define SHA512_HASH_SIZE		64

# define MD5_HASH_LEN			MD5_HASH_SIZE/sizeof(t_md5_word)
# define SHA1_HASH_LEN			SHA1_HASH_SIZE/sizeof(t_sha1_word)
# define SHA256_HASH_LEN		SHA256_HASH_SIZE/sizeof(t_sha256_word)
# define SHA512_HASH_LEN		SHA512_HASH_SIZE/sizeof(t_sha512_word)

typedef struct	s_digest_ctx
{
	uint8_t		messize[16];
	uint8_t		blocksize;
	uint8_t		hashsize;
	uint8_t		var[128];
	uint8_t		hash[128];
	uint8_t		buf[128];
	uint8_t		bufsize;
	int			done;
}				t_digest_ctx;

int		hash_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

void	md5_init(t_digest_ctx *ctx);
void	md5_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	md5_final(t_digest_ctx *ctx);
void	md5_update_block(t_digest_ctx *ctx, const uint8_t mesblock[MD5_BLOCK_SIZE]);
void	md5_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result md5_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result md5_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha1_init(t_digest_ctx *ctx);
void	sha1_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha1_final(t_digest_ctx *ctx);
void	sha1_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA1_BLOCK_SIZE]);
void	sha1_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result sha1_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha1_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha256_init(t_digest_ctx *ctx);
void	sha256_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha256_final(t_digest_ctx *ctx);
void	sha256_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA256_BLOCK_SIZE]);
void	sha256_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result sha256_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha256_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha224_init(t_digest_ctx *ctx);
void	sha224_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha224_final(t_digest_ctx *ctx);

t_transform_result sha224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha512_init(t_digest_ctx *ctx);
void	sha512_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha512_final(t_digest_ctx *ctx);
void	sha512_update_block(t_digest_ctx *ctx, const uint8_t mesblock[SHA512_BLOCK_SIZE]);
void	sha512_final_block(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result sha512_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha512_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha384_init(t_digest_ctx *ctx);
void	sha384_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha384_final(t_digest_ctx *ctx);

t_transform_result sha384_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha384_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha512_256_init(t_digest_ctx *ctx);
void	sha512_256_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha512_256_final(t_digest_ctx *ctx);

t_transform_result sha512_224_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha512_224_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	sha512_224_init(t_digest_ctx *ctx);
void	sha512_224_update(t_digest_ctx *ctx, const unsigned char *buf, size_t bufsize);
void	sha512_224_final(t_digest_ctx *ctx);

t_transform_result sha512_256_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha512_256_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

#endif
