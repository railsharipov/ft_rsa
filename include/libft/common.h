/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rsharipo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/09 13:36:51 by rsharipo          #+#    #+#             */
/*   Updated: 2018/10/13 09:00:28 by rsharipo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_COMMON_H
# define LIBFT_COMMON_H

# include <limits.h>

# define COLOR_RED				"\x1b[31m"
# define COLOR_GREEN			"\x1b[32m"
# define COLOR_YELLOW			"\x1b[33m"
# define COLOR_BLUE				"\x1b[34m"
# define COLOR_MAGENTA			"\x1b[35m"
# define COLOR_CYAN				"\x1b[36m"
# define COLOR_RED_BOLD			"\033[1m\033[31m"
# define COLOR_RESET			"\x1b[0m"

# define TXT_RED(STR_LITERAL)	COLOR_RED STR_LITERAL COLOR_RESET
# define TXT_GREEN(STR_LITERAL)	COLOR_GREEN STR_LITERAL COLOR_RESET
# define TXT_YELL(STR_LITERAL)	COLOR_YELLOW STR_LITERAL COLOR_RESET
# define TXT_CYAN(STR_LITERAL)	COLOR_CYAN STR_LITERAL COLOR_RESET
# define TXT_MAG(STR_LITERAL)	COLOR_MAGENTA STR_LITERAL COLOR_RESET

# define FLAG(F,X)				((int)(((X)&(F))==(F)))

/* Round X down to a multiple of Y */
# define FLOOR(X,Y)				((X)-((X)%(Y)))

/* Round X up to a multiple of Y */
# define CEIL(X,Y)				(((X)%(Y)==0)?(X):((X)+((Y)-((X)%(Y)))))

# define MAX(A,B)				((A)>(B)?(A):(B))
# define MIN(A,B)				((A)<(B)?(A):(B))
# define ASC(X)					(X-48)
# define ABS(X)					((X)>=(0)?(X):(-X))

# define NBITS_TO_NWORDS(X, WORD_BIT)	((((ssize_t)X)-1)/WORD_BIT+1)
# define NBITS_TO_NBYTES(X)				((((ssize_t)X)-1)/CHAR_BIT+1)
# define NWORDS_TO_NBITS(X, WORD_BIT)	((X)*WORD_BIT)
# define NBYTES_TO_NBITS(X)				((X)*CHAR_BIT)
# define IS_NONZERO_INT32(X)			((((X)|(0-(X)))>>31)&1)
# define IS_NONZERO_INT64(X)			((((X)|(0-(X)))>>63)&1)

# define IS_OF_TYPE(X, T)		_Generic((X), T:1, default:0)
# define DO_NOTHING				(void)0;

typedef enum	e_libft_status
{
	LIBFT_OK = 0,
	LIBFT_ERR,
}				t_libft_status;

#endif
