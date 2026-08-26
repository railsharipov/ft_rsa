#ifndef SSL_BASE64_H
# define SSL_BASE64_H

# include <common.h>
# include <io.h>
#include <libft.h>

# define B64_A(X)	(((X)&(0xfc))>>(2))
# define B64_B(X)	((((X)<<(4))&(0x30))|(((X)>>(12))&(0xf)))
# define B64_C(X)	((((X)>>(6))&(0x3c))|(((X)>>(22))&(0x3)))
# define B64_D(X)	(((X)>>(16))&(0x3f))

# define B64_ENC_BLOCK_SIZE	4
# define B64_MES_BLOCK_SIZE	3

typedef void (*t_func_b64_process_block)(uint8_t *, uint8_t *);

typedef enum	e_b64_mode
{
	B64_MODE_ENCODE = 0,
	B64_MODE_DECODE,
}				t_b64_mode;

typedef struct	s_b64
{
	t_func_b64_process_block	f_process_block;
	t_b64_mode	mode;
	uint8_t		blocksize;
	size_t		bufsize;
	size_t		messize;
}				t_b64;

enum	e_b64_flag
{
	B64_D	= 1 << 1
};

int		base64_encode_all(const unsigned char *mes, size_t messize, unsigned char **enc, size_t *encsize);
int		base64_decode_all(const unsigned char *enc, size_t encsize, unsigned char **mes, size_t *messize);
int		base64_check(const unsigned char *b64enc, size_t size);

////////////////////////////////////////////////////////////////////

typedef struct s_b64_ctx {
	uint8_t	buf[4];
	int size;
	int used;
	int final;
	int done;
} t_b64_ctx;

void	base64_encode_block(const uint8_t mesblock[3], uint8_t encblock[4]);
void	base64_decode_block(uint8_t *encblock, uint8_t *mesblock);

t_transform_result base64_encode_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result base64_encode_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result base64_decode_transform_update(void *vctx, const void *in, size_t insize, void *out, size_t outsize);
t_transform_result base64_decode_transform_final(void *vctx, const void *in, size_t insize, void *out, size_t outsize);

#endif
