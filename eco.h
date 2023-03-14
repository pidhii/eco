#ifndef ECO_H
#define ECO_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif


#define ECO_REG_R12 0
#define ECO_REG_R13 1
#define ECO_REG_R14 2
#define ECO_REG_R15 3
#define ECO_REG_RETADDR 4
#define ECO_REG_SP 5
#define ECO_REG_BP 7
#define ECO_REG_FPU 8


typedef struct eco_t eco_t;

typedef void (*eco_entry_point_t)(/* eco_t *this, eco_t *caller, void *udata */);

typedef void (*eco_return_handle_t)(eco_t*);

typedef struct eco_stack_info_t {
  void *memptr;
  size_t memsize;
  unsigned valgrind_stack_id;
} eco_stack_info_t;

struct eco_t {
  uint64_t _regs[9];
  struct {
    void *memptr;
    size_t memsize;
    unsigned valgrind_stack_id;
  } _stack;

  eco_t *_last_caller;
  bool _returned;

  eco_return_handle_t return_handle;
};

void
eco_init_this_thread();

typedef struct eco_stack_t {
  void *stack;
  size_t stack_size;
  void *mem_ptr;
  size_t mem_size;
} eco_stack_t;

void
eco_allocate_guarded_stack(int npag, eco_stack_t *stack);

void
eco_destroy_guarded_stack(eco_stack_t *stack);

/**
 * XXX GLIC has some unspoken requirements regarding the stack size, so dont be
 * too greedy with it. E.g. vfprintf allocates an array of 1000 bytes on the
 * stack. So in case your stack is about this size or smaller, you'll experience
 * all of your most wicked nightmares in one go.
 */
void
eco_init(eco_t *eco, eco_entry_point_t entry, void *stack, size_t stacksize);

void
eco_cleanup(eco_t *eco);

bool
eco_switch(eco_t *from, eco_t *to, void *udata, eco_t **ret_from,
    void **ret_udata);


#ifdef __cplusplus
}
#endif

#endif
