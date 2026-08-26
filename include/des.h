#ifndef SSL_DES_H
# define SSL_DES_H

# include <common.h>
# include <io.h>
#include <libft.h>

# define DES_BLOCK_SIZE	8
# define DES_KSCHED_LEN	16

struct s_des_ctx;

typedef	void (*t_func_der_permute_block)(struct s_des_ctx *des, uint64_t *block);

typedef enum	e_des_mode
{
	DES_MODE_ENCRYPT = 0,
	DES_MODE_DECRYPT,
}				t_des_mode;

typedef enum	e_des_crypt
{
	DES_CRYPT_ECB = 0,
	DES_CRYPT_CBC,
}				t_des_crypt;

typedef struct	s_des_ctx
{
	t_func_der_permute_block	f_permute_block;
	t_des_crypt crypt;
	t_des_mode	mode;
	uint64_t	ksched[DES_KSCHED_LEN];
	uint8_t		vect[DES_BLOCK_SIZE];
	uint8_t		buf[DES_BLOCK_SIZE];
	size_t		bufsize;
	size_t		messize;
}				t_des_ctx;

enum	e_des
{
	DES_K = 1UL << 1,
	DES_P = 1UL << 2,
	DES_S = 1UL << 3,
	DES_V = 1UL << 4,
	DES_N = 1UL << 5,
	DES_A = 1UL << 6,
	DES_E = 1UL << 7,
	DES_D = 1UL << 8
};

int		des_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8], t_des_crypt crypt, t_des_mode mode);
ssize_t	des_update(t_des_ctx *des, const char *in, char *out, size_t size);
ssize_t	des_final(t_des_ctx *des, char *out, size_t size);

/* Low level functions */
void	des_permute_block_ecb(t_des_ctx *des, uint64_t *block);
void	des_permute_block_cbc(t_des_ctx *des, uint64_t *block);

#endif
