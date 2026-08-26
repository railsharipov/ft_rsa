#include <des.h>
#include <logger.h>
#include <libft.h>

static const unsigned char	CHA[DES_KSCHED_LEN] = {
	1,  1,  2,  2,  2,  2,  2,  2,
	1,  2,  2,  2,  2,  2,  2,  1
};

static const unsigned char	CSA[48] = {
	14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
	23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

static const unsigned char	PKA[56] = {
	57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
	10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
	14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4
};

static const uint64_t	MASK28 = ((uint64_t)1 << 28) - 1;
static const uint64_t	MASK56 = ((uint64_t)1 << 56) - 1;

static void	__init_encrypt_schedule(uint64_t *kshed, const uint8_t *key);
static void	__init_decrypt_schedule(uint64_t *kshed, const uint8_t *key);

int    des_init(t_des_ctx *des, const uint8_t key[8], const uint8_t iv[8], t_des_crypt crypt, t_des_mode mode)
{
	SSL_LOG(TRACE, "init start");
	SSL_LOG(DEBUG, "init: key: %x%x%x%x%x%x%x%x", key);
	if (iv) {
		SSL_LOG(DEBUG, "init: iv: %x%x%x%x%x%x%x%x", iv);
    }
    if (NULL == des || NULL == key) {
        SSL_LOG(ERROR, INVALID_INPUT_ERROR);
        return (SSL_ERR);
    }
    ft_bzero(des, sizeof(t_des_ctx));

    switch (crypt) {
        case DES_CRYPT_ECB:
            des->f_permute_block = des_permute_block_ecb;
            break;
        case DES_CRYPT_CBC:
            des->f_permute_block = des_permute_block_cbc;
            break;
        default:
            SSL_LOG(ERROR, INVALID_INPUT_ERROR ": unknown crypt");
            return (SSL_ERR);
    }

    switch (mode) {
        case DES_MODE_ENCRYPT:
            __init_encrypt_schedule(des->ksched, key);
            break ;
        case DES_MODE_DECRYPT:
            __init_decrypt_schedule(des->ksched, key);
            break ;
        default:
            SSL_LOG(ERROR, INVALID_INPUT_ERROR ": unknown mode");
            return (SSL_ERR);
    }

    des->crypt = crypt;
    des->mode = mode;

    if (NULL != iv) {
        ft_memcpy(des->vect, iv, DES_BLOCK_SIZE);
    }

	SSL_LOG(TRACE, "init finish");
    return (SSL_OK);
}

static void	__init_encrypt_schedule(uint64_t *ksched, const uint8_t *key)
{
	uint64_t	lblock, rblock;
	uint64_t	temp_key, permuted_key;
	uint64_t	tn;
	int			ix, iy;

	temp_key = *(uint64_t *)key;
# if BYTE_ORDER == LITTLE_ENDIAN
	temp_key = ft_uint_bswap64(temp_key);
# endif
	permuted_key = 0;

	ix = 0;
	while (ix < 56) {
		permuted_key <<= 1;
		permuted_key |= (temp_key >> (64 - PKA[ix])) & 1;
		ix++;
	}
	lblock = permuted_key >> 28;
	rblock = permuted_key & MASK28;

    ix = 0;
    while (ix < 16) {
        lblock = ((lblock << CHA[ix]) & MASK28) | (lblock >> (28 - CHA[ix]));
        rblock = ((rblock << CHA[ix]) & MASK28) | (rblock >> (28 - CHA[ix]));

        tn = ((lblock << 28) & MASK56) | rblock;
        iy = 0;
        while (iy < 48) {
            ksched[ix] <<= 1;
            ksched[ix] |= (tn >> (56 - CSA[iy])) & 1;
            iy++;
        }
        ix++;
    }
}

static void	__init_decrypt_schedule(uint64_t *ksched, const uint8_t *key)
{
	uint64_t	lblock, rblock;
	uint64_t	temp_key, permuted_key;
	uint64_t	tn;
	int			ix, iy;

	temp_key = *(uint64_t *)key;
# if BYTE_ORDER == LITTLE_ENDIAN
	temp_key = ft_uint_bswap64(temp_key);
# endif
	permuted_key = 0;

	ix = 0;
	while (ix < 56) {
		permuted_key <<= 1;
		permuted_key |= (temp_key >> (64 - PKA[ix])) & 1;
		ix++;
	}
	lblock = permuted_key >> 28;
	rblock = permuted_key & MASK28;

    ix = 0;
    while (ix < 16) {
        lblock = ((lblock << CHA[ix]) & MASK28) | (lblock >> (28 - CHA[ix]));
        rblock = ((rblock << CHA[ix]) & MASK28) | (rblock >> (28 - CHA[ix]));

        tn = ((lblock << 28) & MASK56) | rblock;
        iy = 0;
        while (iy < 48) {
            ksched[15-ix] <<= 1;
            ksched[15-ix] |= (tn >> (56 - CSA[iy])) & 1;
            iy++;
        }
        ix++;
    }
}
