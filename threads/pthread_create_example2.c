#include <pthread.h>
#include <stdio.h>

void * compute(void * unused)
{
  // Do dome computations
  for(int i = 0 ; i < 100; i++)
    fputc('x',stderr);
  return NULL;
}
int main()
{
  pthread_t tid;
  int ret = pthread_create(&tid, NULL, compute, NULL);
  if (ret == -1)
    perror("pthread_create failed");
  for(int i = 0; i < 10; i++)
    fputc('o',stderr);
  return 0;
}
