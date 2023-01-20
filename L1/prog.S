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

   call _go

   # restore callee-saved registers and return
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %rbp
   popq %rbx
   retq


_finnish:
   subq $32, %rsp # Allocate spills
   movq %r15, 24(%rsp)
   movq %r14, 16(%rsp)
   movq %r13, 8(%rsp)
   movq %r12, 0(%rsp)
   movq %rdx, %r12
   movq $0, %r15
   movq %rsi, %r14
   movq %rdi, %r13
_outer:
   inc %r15
   cmpq %r12, %r15
   jl _END2
   movq $0, %rcx
   movq %r14, %rdx
   lea (%r13, %r15, 8), %rsi
   movq %rsi, %rdi
_inner:
   inc %rcx
   cmpq %rcx, %r12
   jl _print
   lea (%r13, %rcx, 8), %rsi
   movq 0(%rsi), %r8
   cmpq %rdx, %r8
   jl _newMin
   jmp _inner
_newMin:
   movq %r8, %rdx
   movq %rsi, %rdi
   jmp _inner
_print:
   movq %r14, 0(%rdi)
   movq %rdx, %rdi
   call print # runtime system call
   jmp _outer
_END2:
   movq $0, %rax
   movq 0(%rsp), %r12
   movq 8(%rsp), %r13
   movq 16(%rsp), %r14
   movq 24(%rsp), %r15
   addq $32, %rsp # Free stack locations
   retq


_getMax:
   movq $1, %rsi
   movq $0, %rdx
   movq 0(%rdi), %r8
_continue:
   inc %rdx
   cmpq %rdx, %r8
   jl _END
   lea (%rdi, %rdx, 8), %rcx
   movq 0(%rcx), %r9
   cmpq %rsi, %r9
   jl _continue
   movq %r9, %rsi
   jmp _continue
_END:
   movq %rsi, %rax
   retq


_go:
   subq $16, %rsp # Allocate spills
   movq %r13, 8(%rsp)
   movq %r12, 0(%rsp)
   call input # runtime system call
   movq %rax, %rdi
   movq $1, %rsi
   call allocate # runtime system call
   movq %rax, %rdi
   movq %rdi, %r12
   call print # runtime system call
   movq %r12, %rdi
   movq $_arrayInit, -8(%rsp)
   subq $8, %rsp      # Allocate extra arguments on the stack as well as the return address 
   jmp _initArray        # Call to @initArray
_arrayInit:
   movq %rax, %rdi
   movq %rdi, %r12
   movq $_gotMax, -8(%rsp)
   subq $8, %rsp      # Allocate extra arguments on the stack as well as the return address 
   jmp _getMax        # Call to @getMax
_gotMax:
   movq %r12, %rdi
   movq %rax, %r13
   call print # runtime system call
   movq %r12, %rdi
   movq %r13, %rsi
   movq 0(%rdi), %rdx
   movq $_fini, -8(%rsp)
   subq $8, %rsp      # Allocate extra arguments on the stack as well as the return address 
   jmp _finnish        # Call to @finnish
_fini:
   movq 0(%rsp), %r12
   movq 8(%rsp), %r13
   addq $16, %rsp # Free stack locations
   retq


_initArray:
   subq $24, %rsp # Allocate spills
   movq %r14, 16(%rsp)
   movq %r13, 8(%rsp)
   movq %r12, 0(%rsp)
   movq $0, %r12
   movq %rdi, %r14
   movq 0(%rdi), %rsi
   movq %rsi, %r13
_BEGIN:
   call input # runtime system call
   movq %r14, %rdi
   movq %r12, %rsi
   inc %rsi
   movq %rsi, %r12
   lea (%rdi, %rsi, 8), %rdx
   movq %rax, 0(%rdx)
   cmpq %r13, %rsi
   jl _BEGIN
   movq %rdi, %rax
   movq 0(%rsp), %r12
   movq 8(%rsp), %r13
   movq 16(%rsp), %r14
   addq $24, %rsp # Free stack locations
   retq

