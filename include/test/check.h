#ifndef SSL_CHECK_H
# define SSL_CHECK_H

# include <libft/std.h>

# define SSL_CHECK(STATE) \
	do \
	{ \
    if (!(STATE)) \
		{ \
      ft_printf("Assertion failed (%s), function %s, file %s, line %d.\n", #STATE, __func__, __FILE__, __LINE__); \
      ssl_exit(0); \
		} \
	} \
	while(0)

#endif
