#ifndef SSL_HASH_H
# define SSL_HASH_H

# include <common.h>
# include <io.h>
#include <libft.h>

typedef uint32_t	t_md5_word;
typedef uint32_t	t_sha1_word;
typedef uint32_t	t_sha224_word;
typedef uint32_t	t_sha256_word;
typedef uint64_t	t_sha512_224_word;
typedef uint64_t	t_sha512_256_word;
typedef uint64_t	t_sha384_word;
typedef uint64_t	t_sha512_word;

# define MD5_BLOCK_SIZE			64
# define SHA1_BLOCK_SIZE		64
# define SHA224_BLOCK_SIZE		64
# define SHA256_BLOCK_SIZE		64
# define SHA384_BLOCK_SIZE		128
# define SHA512_224_BLOCK_SIZE	128
# define SHA512_256_BLOCK_SIZE	128
# define SHA512_BLOCK_SIZE		128

# define MD5_HASH_LEN			4
# define SHA1_HASH_LEN			5
# define SHA224_HASH_LEN		7
# define SHA256_HASH_LEN		8
# define SHA384_HASH_LEN		6
# define SHA512_224_HASH_LEN	4
# define SHA512_256_HASH_LEN	4
# define SHA512_HASH_LEN		8

# define MD5_HASH_SIZE			MD5_HASH_LEN * sizeof(t_md5_word)
# define SHA1_HASH_SIZE			SHA1_HASH_LEN * sizeof(t_sha1_word)
# define SHA224_HASH_SIZE		SHA224_HASH_LEN * sizeof(t_sha224_word)
# define SHA256_HASH_SIZE		SHA256_HASH_LEN * sizeof(t_sha256_word)
# define SHA384_HASH_SIZE		SHA384_HASH_LEN * sizeof(t_sha384_word)
# define SHA512_224_HASH_SIZE	SHA512_224_HASH_LEN * sizeof(t_sha512_224_word)
# define SHA512_256_HASH_SIZE	SHA512_256_HASH_LEN * sizeof(t_sha512_256_word)
# define SHA512_HASH_SIZE		SHA512_HASH_LEN * sizeof(t_sha512_word)

# define CH(X,Y,Z)		(((X)&(Y))^((~X)&(Z)))
# define MAJ(X,Y,Z)		(((X)&(Y))^((X)&(Z))^((Y)&(Z)))
# define RROT(X,N)		(((X)>>(N))|((X)<<(32-(N))))
# define LROT(X,N)		(((X)<<(N))|((X)>>(32-(N))))
# define RROT64(X,N)	(((X)>>(N))|((X)<<(64-(N))))
# define LROT64(X,N)	(((X)<<(N))|((X)>>(64-(N))))

# define E0(X)			(RROT(X,2)^RROT(X,13)^RROT(X,22))
# define E1(X)			(RROT(X,6)^RROT(X,11)^RROT(X,25))
# define S0(X)			(RROT(X,7)^RROT(X,18)^((X)>>(3)))
# define S1(X)			(RROT(X,17)^RROT(X,19)^((X)>>(10)))

# define E2(X)			(RROT64(X,28)^RROT64(X,34)^RROT64(X,39))
# define E3(X)			(RROT64(X,14)^RROT64(X,18)^RROT64(X,41))
# define S2(X)			(RROT64(X,1)^RROT64(X,8)^((X)>>(7)))
# define S3(X)			(RROT64(X,19)^RROT64(X,61)^((X)>>(6)))

# define F0(X,Y,Z)		(((X)&(Y))|((~X)&(Z)))
# define G0(X,Y,Z)		(((X)&(Z))|((Y)&(~Z)))
# define H0(X,Y,Z)		((X)^(Y)^(Z))
# define I0(X,Y,Z)		((Y)^((X)|(~Z)))

enum  e_hash_flag
{
	HASH_P = 1UL << 1,
	HASH_Q = 1UL << 2,
	HASH_R = 1UL << 3,
	HASH_S = 1UL << 4
};

typedef struct	s_hash
{
	uint8_t		messize[16];
	uint8_t		blocksize;
	uint8_t		hashsize;
	uint8_t		var[128];
	uint8_t		hash[128];
	uint8_t		buf[128];
	uint8_t		bufsize;
	int			done;
}				t_hash;

int		hash_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

void	hash_md5_init(t_hash *ctx);
void	hash_md5_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_md5_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_md5_final(t_hash *ctx);
void	md5_update_block(t_hash *ctx, const uint8_t mesblock[MD5_BLOCK_SIZE]);
void	md5_final_block(t_hash *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result md5_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result md5_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	hash_sha1_init(t_hash *ctx);
void	hash_sha1_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha1_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha1_final(t_hash *ctx);
void	sha1_update_block(t_hash *ctx, const uint8_t mesblock[SHA1_BLOCK_SIZE]);
void	sha1_final_block(t_hash *ctx, const uint8_t *mesblock, size_t messize);

t_transform_result sha1_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result sha1_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

void	hash_sha256_init(t_hash *ctx);
void	hash_sha256_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha256_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha256_final(t_hash *ctx);

void	hash_sha224_init(t_hash *ctx);
void	hash_sha224_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha224_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha224_final(t_hash *ctx);

void	hash_sha512_init(t_hash *ctx);
void	hash_sha512_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha512_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha512_final(t_hash *ctx);

void	hash_sha384_init(t_hash *ctx);
void	hash_sha384_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha384_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha384_final(t_hash *ctx);

void	hash_sha512_256_init(t_hash *ctx);
void	hash_sha512_256_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha512_256_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha512_256_final(t_hash *ctx);

void	hash_sha512_224_init(t_hash *ctx);
void	hash_sha512_224_update(t_hash *ctx, const unsigned char *buf, size_t bufsize);
void	hash_sha512_224_update_stream(t_hash *ctx, t_iodes *iodes);
void	hash_sha512_224_final(t_hash *ctx);

#endif
