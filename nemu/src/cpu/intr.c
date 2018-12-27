#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_pop((uint32_t *)&cpu.eflags);
	rtl_pop(&cpu.cs);
	rtl_pop(&ret_addr);	
	rtl_lr(&t0, cpu.IDTR.len, 2);
	t1 = NO * 8;
	assert(0);
	assert(interpret_relop(RELOP_LT, t1, t0));	
	rtl_lr(&t0, cpu.IDTR.base, 4);
	rtl_add(&t0, &t1, &t0);
	printf("0x%08x\n", t0);
	rtl_lm(&t1, &t0, 2);
	t0 += 2;
	rtl_lm(&t2, &t0, 4);
	rtl_msb(&t2, &t2, 4);
	assert(t2);
	t0 += 4;
	rtl_lm(&t2, &t0, 2);
	t2 <<= 8;
	rtl_and(&t1, &t1, &t2);
	printf("0x%08x\n", t1);
	rtl_jr(&t1);

}

void dev_raise_intr() {
}
