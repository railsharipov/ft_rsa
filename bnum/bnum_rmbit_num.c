#include <bnum.h>

int	rmbit_num(const t_num *num)
{
	int        setbit, i;
	uint64_t   rmint;

  for (i = 0; i < num->len-1 && num->val[i] == 0;)
	{
		i++;
	}
  rmint = num->val[i];
	for (setbit = 0; !((rmint >> setbit) & 0x1) && (setbit < BNUM_DIGIT_BIT);)
	{
		setbit++;
	}
	if (setbit == BNUM_DIGIT_BIT)
	{
		return (0);
	}
	return (setbit+1 + BNUM_DIGIT_BIT * i);
}
