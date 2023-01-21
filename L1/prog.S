   .text
   .globl go

go:
   # save callee-saved registers
   pushq %rbx
   pushq %rbp
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15

   call _main

   # restore callee-saved registers and return
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %rbp
   popq %rbx
   retq


_encode:
   salq $1, %rdi
   addq $1, %rdi
   movq %rdi, %rax
   retq


_main:
   subq $8, %rsp # Allocate spills
   movq %r12, 0(%rsp)
   movq $22, %rdi
   movq $_ret1, -8(%rsp)
   subq $8, %rsp      # Allocate extra arguments on the stack as well as the return address 
   jmp _encode        # Call to @encode
_ret1:
   movq %rax, %r12
   movq %rax, %rdi
   call print # runtime system call
   movq %r12, %rdi
   movq $_ret2, -8(%rsp)
   subq $8, %rsp      # Allocate extra arguments on the stack as well as the return address 
   jmp _encode        # Call to @encode
_ret2:
   movq %rax, %rdi
   call print # runtime system call
   movq 0(%rsp), %r12
   addq $8, %rsp # Free stack locations
   retq

