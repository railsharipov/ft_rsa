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

#ifndef LIBFT_ALLOC_H
# define LIBFT_ALLOC_H

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <libft/common.h>
# include <libft/std.h>

/* Uncomment to enable libft malloc wrapper */
// # define LIBFT_MEM_ALLOC

# define LIBFT_MALLOC_HT_SIZE	512

enum	e_libft_alloc_flag
{
	LIBFT_ALLOC_NONE = 0,
	LIBFT_ALLOC_DEBUG,
};

enum	e_libft_alloc_err
{
	LIBFT_MEM_OK = 0,
	LIBFT_MEM_ERR,
	LIBFT_MEM_LEAK,
	LIBFT_MEM_DOUBLE_FREE,
	LIBFT_MEM_FATAL,
};

extern int	global_libft_alloc_error;

void    *ft_malloc(const char *memkey, size_t memsize);
void    ft_free(const char *memkey, void *memptr);
void    ft_free_all(void);

# if defined (LIBFT_MEM_ALLOC)

#  define MEM_ERROR() \
	do \
	{ \
		ft_printf("%@critical memory error %d\n", global_libft_alloc_error); \
		ft_printf("%@\t%s in %s:%d\n", __func__, __FILE__, __LINE__); \
	} \
	while (0)

#  define LIBFT_FREE(PTR) \
	do \
	{ \
		if (NULL != PTR) \
		{ \
			ft_free(#PTR, PTR); \
			\
			if (LIBFT_OK != global_libft_alloc_error) \
			{ \
				MEM_ERROR() \
			} \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_ALLOC(PTR, SZ) \
	do \
	{ \
		PTR = ft_malloc(#PTR, SZ); \
		if (LIBFT_OK != global_libft_alloc_error) \
		{ \
			MEM_ERROR() \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do \
	{ \
		void	*NEWPTR; \
		NEWPTR = ft_malloc(#PTR "_realloc_", NSZ); \
		if (LIBFT_OK != global_libft_alloc_error) \
		{ \
			MEM_ERROR() \
			NEWPTR = NULL; \
		} \
		if (NULL != PTR && NULL != NEWPTR) \
		{ \
			ft_memcpy(NEWPTR, PTR, SZ); \
			ft_free(#PTR, PTR); \
		} \
		PTR = NEWPTR; \
	} \
	while (0)

# else

#  define LIBFT_FREE(PTR) \
	do \
	{ \
		if (NULL != PTR) \
		{ \
			free(PTR); \
			PTR = NULL; \
		} \
	} \
	while (0)

#  define LIBFT_ALLOC(PTR, SZ) \
	do \
	{ \
		PTR = malloc(SZ); \
		if (ENOMEM == errno) \
		{ \
			perror(TXT_RED("critical memory error")); \
			PTR = NULL; \
		} \
		else \
		{ \
			ft_bzero(PTR, SZ); \
		} \
	} \
	while (0)

#  define LIBFT_REALLOC(PTR, SZ, NSZ) \
	do \
	{ \
		void	*NEWPTR; \
		LIBFT_ALLOC(NEWPTR, NSZ); \
		if (NULL != PTR && NULL != NEWPTR) \
		{ \
			ft_memcpy(NEWPTR, PTR, SZ); \
			free(PTR); \
		} \
		PTR = NEWPTR; \
	} \
	while (0)

# endif

#endif
