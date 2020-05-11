#include <ft_ssl.h>

int main()
{
  const char *map = "a b c d { 1 2 3 4 5 { x y z } 6 7 } e f g";
  char    **keys = ft_strsplit(map, ' ');
  t_node  *ntree;
  t_node  *node;
  t_htbl  *htbl;
  int space = 0;

  ntree = ft_ntree_construct(map);
  htbl = ft_ntree_htable(ntree);

  for (int idx = 0; keys[idx] != NULL; idx++)
  {
    ft_htbl_assign(htbl, keys[idx], keys[idx]);
  }
  ft_ntree_print(ntree);

  return (0);
}
