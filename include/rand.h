#ifndef SSL_RAND_H
# define SSL_RAND_H

# include <common.h>
#include <libft.h>

# define RAND_DEV	"/dev/random"

# define MT_A		0xB5026F5AA96619E9
# define MT_F		0x5851F42D4C957F2D
# define MT_W		64
# define MT_N		312
# define MT_M		156
# define MT_R		31
# define MT_U		29
# define MT_D		0x5555555555555555
# define MT_S		17
# define MT_B		0x71D67FFFEDA60000
# define MT_T		37
# define MT_C		0xFFF7EEE000000000
# define MT_L		43
# define MT_MASK	(((1ULL<<(MT_W-1))-1)|(1ULL<<(MT_W-1)))
# define MT_LOMASK	((1ULL<<MT_R)-1)
# define MT_HIMASK	((~MT_LOMASK)&MT_MASK)

int			rand_bytes(uint64_t seed, void *buf, size_t bufsize);
int			rand_useed(uint64_t *seed, int size);
int			rand_mseed(uint64_t *seed, int size);
int			rand_fseed(uint64_t *seed, const char *files);
int			rand_mgf(char *seed, int seedlen, char *mask, int masklen);
void  		rand_mtw_init(uint64_t seed);
uint64_t	rand_mtw_extract(void);
int			rand_openssl_kdf(uint8_t key[8], uint8_t salt[8], uint8_t vect[8], const char *pass);

int	rand_openssl_v3_kdf(
	uint8_t *key, size_t key_size,
	uint8_t *iv, size_t iv_size,
	const uint8_t *salt, size_t salt_size,
	const uint8_t *data, size_t data_size
);

#endif
