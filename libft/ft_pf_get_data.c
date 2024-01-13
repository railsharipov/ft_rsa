/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_get_data.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/02 09:49:49 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:37:22 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft/std.h>
#include <libft/string.h>

int		pf_is_flag(char c, char *flag)
{
	char	check;

	check = *flag;

	if (ft_strchr("#0 -+'", c))
	{
		(c == '#') ? (check |= PF_SHARP) : 0;
		(c == '0') ? (check |= PF_ZERO) : 0;
		(c == '-') ? (check |= PF_MINUS) : 0;
		(c == '+') ? (check |= PF_PLUS) : 0;
		(c == 32) ? (check |= PF_SPACE) : 0;
		(c == 39) ? (check |= PF_APOS) : 0;

		*flag |= check;

		return (1);
	}

	return (0);
}

int		pf_is_len(char c, char next, char *len, int *i)
{
	char	check;

	check = 0;

	if (!ft_strchr("hljz", c))
		return (0);

	if (c == 'h' && next == 'h' && (*i += 1))
		check |= PF_HH;
	else if (c == 'h')
		check |= PF_H;

	if (c == 'l' && next == 'l' && (*i += 1))
		check |= PF_LL;
	else if (c == 'l')
		check |= PF_L;

	(c == 'j') ? check |= PF_J : 0;
	(c == 'z') ? check |= PF_Z : 0;

	if (check > *len)
		*len = check;

	return (1);
}

void	pf_get_data(t_pf *data, const char *s, int *i)
{
	if (s[*i] == 0)
		return ;

	if (pf_is_flag(s[*i], &(data->flag)))
	{
		*i += 1;
	}
	else if (s[*i] >= '0' && s[*i] <= '9')
	{
		data->fwid = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9')
			*i += 1;
	}
	else if (s[*i] == '.' && (*i += 1))
	{
		data->flag |= PF_PREC;
		data->prec = (int)ft_atoi(s + *i);

		while (s[*i] && s[*i] >= '0' && s[*i] <= '9')
			*i += 1;
	}
	else if (s[*i] && pf_is_len(s[*i], s[*i + 1], &(data->len), i))
	{
		*i += 1;
	}
	else
	{
		data->type = s[*i];

		return ;
	}

	pf_get_data(data, s, i);
}
