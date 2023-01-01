/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utf_8.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 09:39:22 by rsharipo          #+#    #+#             */
/*   Updated: 2018/09/27 10:21:08 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

void	pf_utf_8_size(unsigned int *ws, int size, int *chars)
{
	int	i;

	i = 0;
	while (ws && i < size)
	{
		if (ws[i] < 0x80)
			*chars += 1;
		else if (ws[i] < 0x800)
			*chars += 2;
		else if (ws[i] < 0x10000)
			*chars += 3;
		else if (ws[i] <= 0x10FFFF)
			*chars += 4;
		i++;
	}
}

void	pf_encode_utf_8(unsigned int *ws, char *s, int chars, int j)
{
	int	tmp;

	tmp = 0;
	while (j < chars)
	{
		tmp = j;

		if (*ws < 0x80)
		{
			s[j++] = (char)(*ws);
		}
		else if (*ws < 0x800)
		{
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0xC0);
		}
		else if (*ws < 0x10000)
		{
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0xE0);
		}
		else if (*ws <= 0x10FFFF)
		{
			s[j++] = (char)(((*ws >> 18) & 0x3F) | 0xF0);
			s[j++] = (char)(((*ws >> 12) & 0x3F) | 0x80);
		}

		if (*ws >= 0x800)
			s[j++] = (char)(((*ws >> 6) & 0x3F) | 0x80);
		
		if (*ws >= 0x80)
			s[j++] = (char)((*ws & 0x3F) | 0x80);
		
		ws++;
	}
	s[(j == chars) ? j : tmp] = 0;
}

char	*pf_convert_to_utf_8(unsigned int *ws, int size, int prec)
{
	int		chars;
	char	*s;

	chars = 0;
	
	if (ws)
		pf_utf_8_size(ws, size, &chars);
	
	if (prec > 0 && prec < chars)
		chars = prec;

	if (chars && (s = malloc(chars + 1)))
	{
		pf_encode_utf_8(ws, s, chars, 0);
		return (s);
	}

	return (NULL);
}
