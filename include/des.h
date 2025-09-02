#ifndef SSL_DES_H
# define SSL_DES_H

# include <common.h>
# include <io.h>
# include <libft/string.h>
# include <libft/logger.h>

# define DES_BLOCK_SIZE	8
# define DES_KSCHED_LEN	16

# define DES_LOG(LEVEL, MES, ...)	des_logger_log(__func__, __FILE__, __LINE__, LIBFT_LOG_LEVEL_##LEVEL, MES __VA_OPT__(,) __VA_ARGS__)

struct s_des;

typedef	void (*FUNC_DES_PERMUTE_BLOCK)(struct s_des *des, uint64_t *block);

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

typedef struct	s_des
{
	FUNC_DES_PERMUTE_BLOCK	f_permute_block;
	t_des_crypt crypt;
	t_des_mode	mode;
	uint64_t	ksched[DES_KSCHED_LEN];
	uint8_t		vect[DES_BLOCK_SIZE];
	uint8_t		buf[DES_BLOCK_SIZE];
	size_t		bufsize;
	size_t		messize;
}				t_des;

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

int		des_logger_log(const char *func_name, const char *file_name, int line_number, uint8_t level, const char *fmt, ...);

int		des_init(t_des *des, const uint8_t *key, const uint8_t *iv, t_des_crypt crypt, t_des_mode mode);
int 	des_update(t_des *des, t_iodes *in, t_iodes *out);
int 	des_final(t_des *des, t_iodes *in, t_iodes *out);

/* Low level functions */
void	des_permute_block_ecb(t_des *des, uint64_t *block);
void	des_permute_block_cbc(t_des *des, uint64_t *block);

#endif
