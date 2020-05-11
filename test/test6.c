#include <libft.h>

int main()
{
  int **matrix;
  int size = 10;
  int asize = 15;

  matrix = ft_malloc(MEM, NULL, sizeof(int *)*(size+1));

  for (int idx = 0; idx < size; idx++)
  {
    matrix[idx] = ft_malloc(MEM, NULL, sizeof(int)*asize);
    for (int idy = 0; idy < asize; idy++)
    {
      matrix[idx][idy] = rand();
    }
  }
  matrix[size] = NULL;

  ft_2darray_del(matrix, -1);

  return (0);
}
