#include <common.h>
#include <base64.h>
#include <bnum.h>

static const char	SM[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
	'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
	'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '+', '/'
};

void	base64_encode_block(uint8_t *mesblock, uint8_t *encblock)
{
	encblock[0] = SM[( ( mesblock[0]>>2 )&0x3F )];
	encblock[1] = SM[( ( mesblock[0]<<4 )&0x30 )|( ( mesblock[1]>>4 )&0xF )];
	encblock[2] = SM[( ( mesblock[1]<<2 )&0x3C )|( ( mesblock[2]>>6 )&0x3 )];
	encblock[3] = SM[( mesblock[2]&0x3F )];
}
