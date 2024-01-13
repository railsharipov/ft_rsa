#include <ssl/ssl.h>
#include <ssl/error.h>
#include <ssl/base64.h>

static const char	UB64[128] = {
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	62,	0,	0,	0,	63,
	52,	53,	54,	55,	56,	57,	58,	59,	60,	61,	0,	0,	0,	-1,	0,	0,

	0,	-1,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,
	15,	16,	17,	18,	19,	20,	21,	22,	23,	24,	25,	0,	0,	0,	0,	0,
	0,	26,	27,	28,	29,	30,	31,	32,	33,	34,	35,	36,	37,	38,	39,	40,
	41,	42,	43,	44,	45,	46,	47,	48,	49,	50,	51,	0,	0,	0,	0,	0
};

int base64_check(const char *b64enc, size_t size)
{
	int ix;

	if (NULL == b64enc)
		return (SSL_FAIL);

	ix = 0;
	while (ix < size)
	{
		if (UB64[b64enc[ix++]] == 0)
			return (SSL_FAIL);
	}

	return (SSL_OK);
}
