#include "eco/eco.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


static eco_t
mainctx, fooctx, barctx;

void
foo(eco_t *this, eco_t *caller, void *udata)
{
  fprintf(stderr, "[foo] entered: udata = %p\n", udata);

  fprintf(stderr, "[foo] switch to last caller [%p] (1)\n", caller);
  eco_switch(this, caller, (void*)1, &caller, &udata);
  fprintf(stderr, "[foo] switched back: swret.udata = %p\n", udata);

  fprintf(stderr, "[foo] switch to last caller [%p] (2)\n", caller);
  eco_switch(this, caller, (void*)2, &caller, &udata);
  fprintf(stderr, "[foo] switched back: swret.udata = %p\n", udata);

  fprintf(stderr, "[foo] switch to last caller [%p] (3)\n", caller);
  eco_switch(this, caller, (void*)3, &caller, &udata);
}

void
bar(eco_t *this, eco_t *caller, void *udata)
{
  fprintf(stderr, "[bar] entered: udata = %p\n", udata);

  fprintf(stderr, "[bar] switch to last caller [%p] (4)\n", caller);
  eco_switch(this, caller, (void*)4, &caller, &udata);
  fprintf(stderr, "[bar] switched back: swret.udata = %p\n", udata);

  fprintf(stderr, "[bar] switch to [foo] (5)\n");
  eco_switch(this, &fooctx, (void*)5, &caller, &udata);
  fprintf(stderr, "[bar] switched back: swret.udata = %p\n", udata);

  fprintf(stderr, "[bar] switch to [foo] (6)\n");
  eco_switch(this, &fooctx, (void*)6, &caller, &udata);
  fprintf(stderr, "[bar] switched back: swret.udata = %p\n", udata);

  fprintf(stderr, "[bar] switch to [main] (7)\n");
  eco_switch(this, &mainctx, (void*)7, &caller, &udata);
}


//void
//sigsegv_handle(int signum)
//{
//}

int
main()
{
  //stack_t sigstk;
  //sigstk.ss_sp = malloc(SIGSTKSZ);
  //sigstk.ss_size = SIGSTKSZ;
  //sigstk.ss_flags = 0;
  //sigaltstack(&sigstk, NULL);

  //struct sigaction sa;
  //sa.sa_handler = sigsegv_handle;
  //sigemptyset(&sa.sa_mask);
  //sa.sa_flags = SA_ONSTACK;
  //sigaction(SIGSEGV, &sa, NULL);

  eco_init_this_thread();

  eco_stack_t foostack;
  eco_allocate_guarded_stack(1, &foostack);
  eco_init(&fooctx, foo, foostack.stack, foostack.stack_size);

  eco_stack_t barstack;
  eco_allocate_guarded_stack(1, &barstack);
  eco_init(&barctx, bar, barstack.stack, barstack.stack_size);

  fprintf(stderr, "[main] main: %p\n", &mainctx);
  fprintf(stderr, "[main] foo: %p\n", &fooctx);
  fprintf(stderr, "[main] bar: %p\n", &barctx);

  void *udata;

  fprintf(stderr, "[main] switch to [foo] (8)\n");
  if (eco_switch(&mainctx, &fooctx, (void*)8, NULL, &udata))
    fprintf(stderr, "[main] switched back: swret.udata = %p\n", udata);
  else
    fprintf(stderr, "[main] ...no reply...\n");

  fprintf(stderr, "[main] switch to [bar] (9)\n");
  if (eco_switch(&mainctx, &barctx, (void*)9, NULL, &udata))
    fprintf(stderr, "[main] switched back: swret.udata = %p\n", udata);
  else
    fprintf(stderr, "[main] ...no reply...\n");

  fprintf(stderr, "[main] switch to [bar] (10)\n");
  if (eco_switch(&mainctx, &barctx, (void*)10, NULL, &udata))
    fprintf(stderr, "[main] switched back: swret.udata = %p\n", udata);
  else
    fprintf(stderr, "[main] ...no reply...\n");

  fprintf(stderr, "[main] switch to [foo] (11)\n");
  if (eco_switch(&mainctx, &fooctx, (void*)11, NULL, &udata))
    fprintf(stderr, "[main] switched back: swret.udata = %p\n", udata);
  else
    fprintf(stderr, "[main] ...no reply...\n");

  fprintf(stderr, "[main] switch to [bar] (12)\n");
  if (eco_switch(&mainctx, &barctx, (void*)12, NULL, &udata))
    fprintf(stderr, "[main] switched back: swret.udata = %p\n", udata);
  else
    fprintf(stderr, "[main] ...no reply...\n");

  fprintf(stderr, "[main] done\n");

  eco_cleanup(&fooctx);
  eco_cleanup(&barctx);

  eco_destroy_guarded_stack(&foostack);
  eco_destroy_guarded_stack(&barstack);

  //signal(SIGSEGV, SIG_DFL);
  //free(sigstk.ss_sp);

  return EXIT_SUCCESS;
}
