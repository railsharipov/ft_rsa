#ifndef SSL_ALLOC_H
# define SSL_ALLOC_H

# include <libft/alloc.h>

# define SSL_ALLOC(PTR, SZ)			LIBFT_ALLOC(PTR, SZ)
# define SSL_REALLOC(PTR, SZ, NSZ)	LIBFT_REALLOC(PTR, SZ, NSZ)
# define SSL_FREE(PTR)				LIBFT_FREE(PTR)

#endif
