#include <libft.h>

void    print_bits(void *ptr, int bytes)
{
    unsigned char   *p;

    p = (unsigned char *)ptr;

    for (int ix = 0; ix < bytes; ix++)
    {
        for (int iy = 7; iy >= 0; iy--)
        {
            if (p[ix] & (1 << iy))
                printf("%d", 1);
            else
                printf("%d", 0);
        }
    }
    printf("\n");
}

static const uint8_t	PMA[64] = {
	58,	50,	42,	34,	26,	18,	10,	2,	60,	52,	44,	36,	28,	20,	12,	4,
	62,	54,	46,	38,	30,	22,	14,	6,	64,	56,	48,	40,	32,	24,	16,	8,
	57,	49,	41,	33,	25,	17,	9,	1,	59,	51,	43,	35,	27,	19,	11,	3,
	61,	53,	45,	37,	29,	21,	13,	5,	63,	55,	47,	39,	31,	23,	15,	7
};

int main()
{
    int ix;

    for (int ix = 0; ix < 64; ix++)
    {
        if (ix % 4 == 0)
            printf("\n");

        printf("\t%#llx", (uint64_t)1 << (64 - PMA[ix]));
    }
    printf("\n");

    return 0;
}
