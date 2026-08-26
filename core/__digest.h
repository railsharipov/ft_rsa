#ifndef __DIGEST_H
# define __DIGEST_H

#include <libft.h>
#include <digest.h>

typedef uint32_t	t_md5_word;
typedef uint32_t	t_sha1_word;
typedef uint32_t	t_sha256_word;
typedef uint64_t	t_sha512_word;

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

typedef void (__t_func_update)(t_digest_ctx *ctx, const uint8_t *mesblock);
typedef void (__t_func_final)(t_digest_ctx *ctx, const uint8_t *mesblock, size_t messsize);

t_transform_result __transform_update(void *vctx, __t_func_update f_update, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize);

t_transform_result __transform_final(void *vctx, __t_func_update f_update, __t_func_final f_final, size_t blocksize,
	const void *in, size_t insize, void *out, size_t outsize);

#endif
