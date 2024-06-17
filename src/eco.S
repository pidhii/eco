#ifndef __x86_64__
# error "Nicht verstehen..."
#endif


.text


.globl _eco_swap_registers
.type  _eco_swap_registers, @function
.intel_syntax noprefix
_eco_swap_registers:
/*
    0x00                  -->                  0xff
    r12 r13 r14 r15 rip rsp rbx rbp fpucw16 mxcsr32
    0   8   10  18  20  28  30  38  40      44
*/
    // Arguments:
    // rdi - store regs addr
    // rsi - load regs addr
    // rdx - void *udata
    // rcx - eco_parcel_t *ret
    //
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Store current state:
    //
    mov     r8,QWORD PTR [rsp]       // retaddr
    lea     r9,[rsp+0x8]             // rsp
    mov     QWORD PTR [rdi+0x0], r12
    mov     QWORD PTR [rdi+0x8], r13
    mov     QWORD PTR [rdi+0x10],r14
    mov     QWORD PTR [rdi+0x18],r15
    mov     QWORD PTR [rdi+0x20],r8  // retaddr
    mov     QWORD PTR [rdi+0x28],r9  // rsp
    mov     QWORD PTR [rdi+0x30],rbx
    mov     QWORD PTR [rdi+0x38],rbp
    fnstcw  WORD PTR  [rdi+0x40]
    stmxcsr DWORD PTR [rdi+0x44]
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Load new state:
    //
    mov     r12,QWORD PTR [rsi+0x0]
    mov     r13,QWORD PTR [rsi+0x8]
    mov     r14,QWORD PTR [rsi+0x10]
    mov     r15,QWORD PTR [rsi+0x18]
    mov     r8,QWORD PTR [rsi+0x20]  // retaddr
    mov     r9,QWORD PTR [rsi+0x28]  // rsp
    mov     rbx,QWORD PTR [rsi+0x30]
    mov     rbp,QWORD PTR [rsi+0x38]
    fldcw   WORD PTR      [rsi+0x40]
    ldmxcsr DWORD PTR     [rsi+0x44]
    mov     rsp,r9
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Set up args for the first invocation of a thunk:
    //
    //                 thunk(this, caller, udata)
    //
    // swap(rdi, rsi) => thunk(this, caller, ???)
    mov     rax,rdi
    mov     rdi,rsi
    mov     rsi,rax
    // udata is already in rdx => thunk(this, caller, udata) => done
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Set up return-structure for a subsequent context switch
    //
    // ret->caller = caller
    mov     QWORD PTR [rcx],rsi
    // ret->udata = udata
    mov     QWORD PTR [rcx+0x8],rdx
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    // Done
    jmp     r8



.globl _eco_save_fpucw_mxcsr
.type  _eco_save_fpucw_mxcsr, @function
.intel_syntax noprefix
_eco_save_fpucw_mxcsr:
    fnstcw  WORD PTR  [rdi]
    stmxcsr DWORD PTR [rdi+0x4]
    ret



.globl _eco_return_handle
.globl _eco_return_handle_entry
.type  _eco_return_handle_entry, @function
.intel_syntax noprefix
_eco_return_handle_entry:
    pop     rdi
    push    0x00 // to preserve stack alignment
#if defined(__pic__) || defined(__PIC__)
    call    _eco_return_handle@PLT
#else
    call    _eco_return_handle
#endif
