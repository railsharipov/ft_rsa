#ifndef SSL_HASH_H
# define SSL_HASH_H

# include <util/io.h>

typedef uint32_t	t_md5_word;
typedef uint32_t	t_sha1_word;
typedef uint32_t	t_sha224_word;
typedef uint32_t	t_sha256_word;
typedef uint64_t	t_sha512_224_word;
typedef uint64_t	t_sha512_256_word;
typedef uint64_t	t_sha384_word;
typedef uint64_t	t_sha512_word;

# define HASH_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "hash error: ", MES __VA_OPT__(,) __VA_ARGS__)

# define MD5_BLOCK_SIZE			64
# define MD5_HASH_LEN			4

# define SHA1_BLOCK_SIZE		64
# define SHA1_HASH_LEN			5

# define SHA224_HASH_LEN		7

# define SHA256_BLOCK_SIZE		64
# define SHA256_HASH_LEN		8

# define SHA512_224_HASH_LEN	4
# define SHA512_256_HASH_LEN	4
# define SHA384_HASH_LEN		6

# define SHA512_BLOCK_SIZE		128
# define SHA512_HASH_LEN		8

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
	void		*var;
	void		*msize;
	void		*hash;
	int			size;
}				t_hash;

t_hash	*hash_md5_init(void);
void	hash_md5_update(t_hash *, const char *, size_t);
void	hash_md5_final(t_hash *, const char *, size_t);

t_hash	*hash_sha1_init(void);
void	hash_sha1_update(t_hash *, const char *, size_t);
void	hash_sha1_final(t_hash *, const char *, size_t);

t_hash	*hash_sha256_init(void);
void	hash_sha256_update(t_hash *, const char *, size_t);
void	hash_sha256_final(t_hash *, const char *, size_t);

t_hash	*hash_sha224_init(void);
void	hash_sha224_update(t_hash *, const char *, size_t);
void	hash_sha224_final(t_hash *, const char *, size_t);

t_hash	*hash_sha512_init(void);
void	hash_sha512_update(t_hash *, const char *, size_t);
void	hash_sha512_final(t_hash *, const char *, size_t);

t_hash	*hash_sha384_init(void);
void	hash_sha384_update(t_hash *, const char *, size_t);
void	hash_sha384_final(t_hash *, const char *, size_t);

t_hash	*hash_sha512_256_init(void);
void	hash_sha512_256_update(t_hash *, const char *, size_t);
void	hash_sha512_256_final(t_hash *, const char *, size_t);

t_hash	*hash_sha512_224_init(void);
void	hash_sha512_224_update(t_hash *, const char *, size_t);
void	hash_sha512_224_final(t_hash *, const char *, size_t);

#endif
