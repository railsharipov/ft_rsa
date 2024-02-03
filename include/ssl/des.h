#ifndef SSL_DES_H
# define SSL_DES_H

# include <ssl/ssl.h>
# include <libft/string.h>
# include <util/io.h>

# define DES_MES_BLOCK_SIZE	8

# define DES_ERROR(MES, ...)	ssl_error_log(__func__, __FILE__, __LINE__, "des error: ", MES __VA_OPT__(,) __VA_ARGS__)

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

typedef struct  s_des
{
	unsigned char		*key;
	unsigned char		*salt;
	unsigned char		*vect;
	uint32_t	vflag;
}               t_des;

typedef void    (*FUNC_DES)(t_des *const, char *const, ssize_t);

t_des	*des_init(const unsigned char *key, const unsigned char *salt, const unsigned char *vect);
t_des	*des_hexinit(const char *key, const char *salt, const char *vect);
int		des_ecb_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph);
int		des_ecb_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes);
int		des_cbc_encrypt(t_des *des, t_ostring *mes, t_ostring *ciph);
int		des_cbc_decrypt(t_des *des, t_ostring *ciph, t_ostring *mes);

/* Low level functions */
void	des_encrypt_schedule(uint64_t *, uint64_t *);
void	des_decrypt_schedule(uint64_t *, uint64_t *);
void	des_permute_key(uint64_t *, unsigned char *);
void	des_permute_block_init(uint64_t *);
void	des_permute_block(uint64_t *, uint64_t *);
void	des_permute_block_final(uint64_t *);

#endif
