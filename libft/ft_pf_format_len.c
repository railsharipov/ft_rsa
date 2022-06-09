/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_format_len.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/01 19:14:46 by rsharipo          #+#    #+#             */
/*   Updated: 2018/08/11 09:38:35 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_format_di(t_pf *data, char len, va_list *arg)
{
	char		type;
	intmax_t	val;

	val = 0;
	type = data->type;
	if (!(data->flag & PF_PREC))
		data->prec = 1;
	if (!len)
		val = (int)va_arg(*arg, int);
	else if (len & PF_HH)
		val = (signed char)va_arg(*arg, int);
	else if (len & PF_H)
		val = (short)va_arg(*arg, int);
	else if (len & PF_LL)
		val = (long long)va_arg(*arg, long long);
	else if (len & PF_L)
		val = (long)va_arg(*arg, long);
	else if (len & PF_J)
		val = va_arg(*arg, intmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);
	pf_print_di(data, val, data->flag, 0);
}

void	pf_format_oux(t_pf *data, char len, va_list *arg)
{
	char		type;
	uintmax_t	val;

	val = 0;
	type = data->type;
	if (!len)
		val = (unsigned int)va_arg(*arg, unsigned int);
	else if (len & PF_HH)
		val = (unsigned char)va_arg(*arg, unsigned int);
	else if (len & PF_H)
		val = (unsigned short)va_arg(*arg, unsigned int);
	else if (len & PF_LL)
		val = (unsigned long long)va_arg(*arg, unsigned long long);
	else if (len & PF_L)
		val = (unsigned long)va_arg(*arg, unsigned long);
	else if (len & PF_J)
		val = va_arg(*arg, uintmax_t);
	else if (len & PF_Z)
		val = (size_t)va_arg(*arg, size_t);
	pf_print_oux(data, val, data->flag, 0);
}

void	pf_format_c(t_pf *data, char len, va_list *arg)
{
	unsigned int	wc;
	char			c;

	wc = 0;
	c = 0;
	if (len & PF_L)
	{
		wc = va_arg(*arg, unsigned int);
		pf_print_wc(data, wc);
	}
	else
	{
		c = (char)va_arg(*arg, int);
		pf_print_c(data, &c, 1);
	}
}

void	pf_format_s(t_pf *data, char len, va_list *arg)
{
	unsigned int	*ws;
	char			*s;

	s = NULL;
	ws = NULL;
	if (len & PF_L)
		ws = va_arg(*arg, unsigned int *);
	else
		s = va_arg(*arg, char *);
	if (!s && !ws && !(data->flag & PF_PREC))
	{
		if (!(data->flag & PF_PREC))
			pf_out(data, "(null)", 6);
		return ;
	}
	if (len & PF_L)
		pf_print_ws(data, ws);
	else
		pf_print_s(data, s, 0);
}
