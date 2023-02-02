		.text
		.globl go
go:
		pushq %rbx
		pushq %rbp
		pushq %r12
		pushq %r13
		pushq %r14
		pushq %r15
		call _go
		popq %r15
		popq %r14
		popq %r13
		popq %r12
		popq %rbp
		popq %rbx
		retq
_go:
		movq $_funct_ret, -8(%rsp)
		movq $3, %rdi
		movq $5, %rsi
		movq $7, %rdx
		movq $9, %rcx
		movq $11, %r8
		movq $13, %r9
		movq $15, -16(%rsp)
		movq $-17, -24(%rsp)
		subq $24, %rsp
		jmp _funct
_funct_ret:
		movq %rax, %rdi
		call print
		retq
_funct:
		movq 8(%rsp), %rax
		retq
