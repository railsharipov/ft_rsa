#include <libft.h>

void lst_print(t_node *lst)
{
  if (NULL == lst)
    return ;
  printf("hashkey=%u, content=%s\n", lst->hashkey, lst->content);
}

int main()
{
  const char *keys[] = {
    "apple",
    "banana",
    "kiwi",
    "orange",
    "mango",
    "carrot",
    "tomato",
    "potato",
    "vodka"
  };
  t_htbl *htbl;
  t_queue *queue;
  t_node *node;

  queue = ft_queue_init();

  for (int idx = 0; idx < sizeof(keys)/sizeof(*keys); idx++)
    ft_queue_enqueue(queue, keys[idx], keys[idx]);

  // ft_lst_iter(queue->first, &lst_print);
  htbl = ft_queue_htable(queue);

  for (int idx = 0; idx < sizeof(keys)/sizeof(*keys); idx++)
  {
    node = ft_htbl_get(htbl, keys[idx]);
    if (NULL != node)
      printf("hashkey=%u, content=%s\n", node->hashkey, node->content);
  }

  ft_htbl_del(htbl);
  ft_queue_clear(queue);

  return (0);
}
