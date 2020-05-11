#include <bnum.h>

int main(void)
{
  t_num a;

  do {
    set_randnum(&a, 8192);
  } while (!prime_test(&a, lmbit_num(&a)+1, 20));

  return 0;
}
