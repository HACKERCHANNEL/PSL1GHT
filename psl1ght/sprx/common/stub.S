#include "config.h"

.align 2
.section ".rodata.sceFNID","a"
.global LIBRARY_SYMBOL
LIBRARY_SYMBOL:

.align 2
.section ".sceStub.text","ax"
__sprx_stub_call:
	stdu	%r1, -0x80(%r1)
	std		%r2, 0x70(%r1)
	mr		%r2, %r0
	mflr	%r0
	std		%r0, 0x90(%r1)
	lwz		%r2, 0x00(%r2)
	lwz		%r0, 0x00(%r2)
	lwz		%r2, 0x04(%r2)
	mtctr	%r0
	bctrl
	ld		%r2, 0x70(%r1)
	addi	%r1, %r1, 0x80
	ld		%r0, 0x10(%r1)
	mtlr	%r0
	blr

#define EXPORT(name, fnid) \
.align 2; \
.section ".sceStub.text","ax"; \
	.global __##name; \
	__##name: \
		li %r0, name##_stub@l; \
		oris %r0, %r0, name##_stub@ha; \
		b __sprx_stub_call; \
.align 3; \
.section ".opd","aw"; \
	.global name; \
	name: \
		.quad __##name, .TOC.@tocbase, 0

#include "exports.h"
