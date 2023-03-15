#ifndef __x86_64__
# error "Nicht verstehen..."
#endif

#include "eco/eco.h"

#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/mman.h>

#include <valgrind/valgrind.h>


static __thread uint64_t
eco_gtls_fpucw_mxcsr;


typedef struct {
  union { eco_t *sender, *receiver; };
  void *udata;
} _parcel_t;

extern void
_eco_swap_registers(eco_t *savctx, eco_t *newctx, void *udata, _parcel_t *ret);

extern void
_eco_save_fpucw_mxcsr(void* regs);

extern void
_eco_return_handle_entry();

void
_eco_return_handle(eco_t *this)
{
  this->_returned = true;

  if (this->return_handle)
    this->return_handle(this);

  while (true)
  {
    this->_returned = true; // just in case someone decides to touch it
    eco_switch(this, this->_last_caller, NULL, NULL, NULL);
  }
}

void
eco_init_this_thread()
{
  _eco_save_fpucw_mxcsr(&eco_gtls_fpucw_mxcsr);
}

void
eco_allocate_guarded_stack(int npag, eco_stack_t *stack)
{
  // compute size (including guard page)
  int pagesize = sysconf(_SC_PAGESIZE);
  size_t memsize = (npag + 1/* gaurd page */)*pagesize;

  // allocate stack
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE | MAP_ANONYMOUS;
  void *mem = mmap(NULL, memsize, prot, flags, -1, 0);

  // set up guard page
  mprotect(mem, pagesize, PROT_READ);

  stack->stack = mem + pagesize;
  stack->stack_size = memsize - pagesize;
  stack->mem_ptr = mem;
  stack->mem_size = memsize;
}

void
eco_destroy_guarded_stack(eco_stack_t *stack)
{
  munmap(stack->mem_ptr, stack->mem_size);
}


void
eco_init(eco_t *eco, eco_entry_point_t entry, void *stack, size_t stacksize)
{
  memset(eco, 0, sizeof(eco_t));

  // Set up stack:
  uint64_t bp = (uint64_t)stack + stacksize;
  // System V ABI requires 16-bit stack alignment BEFORE the call-instruction
  bp &= ~0x0F;
  // insert return-handle (and make it look like we did a call-instruction):
  void** sp = (void**)(bp - sizeof(void*)*3);
  sp[0] = _eco_return_handle_entry;
  sp[1] = eco;

  eco->_stack.memptr = stack;
  eco->_stack.memsize = stacksize;
  eco->_stack.valgrind_stack_id =
    VALGRIND_STACK_REGISTER(stack, (uint64_t)stack + stacksize);

  // set up registers
  eco->_regs[ECO_REG_RETADDR] = (uint64_t)entry;
  eco->_regs[ECO_REG_SP] = (uint64_t)sp;
  eco->_regs[ECO_REG_BP] = (uint64_t)bp;
  eco->_regs[ECO_REG_FPU] = eco_gtls_fpucw_mxcsr;
}

void
eco_cleanup(eco_t *eco)
{
  VALGRIND_STACK_DEREGISTER(eco->_stack.valgrind_stack_id);
}

bool
eco_switch(eco_t *from, eco_t *to, void *udata, eco_t **ret_from,
    void **ret_udata)
{
  static __thread _parcel_t parcel;

  to->_last_caller = from;
  _eco_swap_registers(from, to, udata, &parcel);
  if (__builtin_expect(to->_returned, 0))
    return false;

  if (ret_from) *ret_from = parcel.sender;
  if (ret_udata) *ret_udata = parcel.udata;
  return true;
}

