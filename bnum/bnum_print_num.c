#include <sys/ioctl.h>
#include <bnum.h>

static void	___in_bar(char *s, unsigned char c)
{
	struct winsize	w;
	int 			len;

	len = ft_strlen(s);
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	ft_printf("\n\033[1m\033[37m");

	for (int i = 0; i < 20; i++)
		ft_printf("%c", c);

	ft_printf(" %s ", s);

	for (int i = len+22; i < w.ws_col; i++)
		ft_printf("%c", c);
	ft_printf("\033[0m\n");
}

static void	___out_num(const t_num *num)
{
	char		*buf;
	ssize_t		bufsize;
	const char	dstr[] = "0123456789abcdef";
	t_num		copy;
	char		*bptr;

	if (BNUM_ZERO(num))
		ft_printf("0");

	bufsize = num->len * (BNUM_INT_BIT / 4);
	LIBFT_ALLOC(buf, bufsize + 1);
	ft_bzero(buf, bufsize + 1);

	init_num(&copy);
	bptr = buf + bufsize - 1;
	copy_num(num, &copy);

	while (!(BNUM_ZERO(&copy)))
	{
		*--bptr = dstr[(*(copy.val) & 0xF)];
		rsh_num_b_inpl(&copy, 4);
	}

	if (num->sign == BNUM_NEG)
		ft_printf("-");
	else if ((BNUM_POS != num->sign) && (BNUM_POS != num->sign))
		ft_printf("[ NO SIGN ] ");

	ft_printf("%s\n", bptr);
	clear_num(&copy);
	LIBFT_FREE(buf);
}

void	print_num(const char *prefix, const t_num *num)
{
	char	*s = NULL;
	int		bits;

	if (prefix)
	{
		bits = lmbit_num(num);
		asprintf(&s, "%s [base=16] [len=%d] [bits=%d]",
			prefix, num->len, bits);
		___in_bar(s, '_');

		free(s);
	}
	___out_num(num);
}

void	print_num_raw(const t_num *num)
{
	if (num->sign == BNUM_NEG)
		ft_printf("-");
	else if ((BNUM_POS != num->sign) && (BNUM_POS != num->sign))
		ft_printf("[ NO SIGN ]");

	for (int i = num->len-1; i >= 0; i--)
		ft_printf("%llu ", num->val[i]);
	ft_printf("\n");
}

void	print_num_bits(const t_num *num)
{
	if (num->sign == BNUM_NEG)
		ft_printf("-");
	else if ((BNUM_POS != num->sign) && (BNUM_POS != num->sign))
		ft_printf("[ NO SIGN ]");

	for (int i = num->len-1; i >= 0; i--)
	{
		for (int j = BNUM_DIGIT_BIT-1; j >= 0; j--)
		{
			ft_printf("%llu", (num->val[i] >> j) & 0x1);
			if (j % 5 == 0)
				ft_printf(" ");
		}
	}
	ft_printf("\n");
}
