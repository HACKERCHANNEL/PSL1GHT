	.globl	spu_thread_exit

spu_thread_exit:
	wrch	$ch28,$3
	stop	0x102
	bi	$0
	lnop
