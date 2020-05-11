#include <ft_ssl.h>
#include <stdarg.h>

int util_null(int n, ...)
{
  va_list	ap;
  void	 *ptr;

  va_start(ap, n);

  while (n-- > 0)
  {
    ptr = va_arg(ap, void *);
    if (NULL == ptr)
      break ;
  }
  va_end(ap);
  return (NULL == ptr);
}
