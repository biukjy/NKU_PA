#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push((rtlreg_t *)&cpu.eflags);
  rtl_push((rtlreg_t *)&cpu.cs);
  rtl_push((rtlreg_t *)&ret_addr);
  
  vaddr_t desc_addr=cpu.idtr.base+8*NO;
  uint32_t eip_low,eip_high,offset;
  eip_low=vaddr_read(desc_addr,4)&0x0000ffff;
  eip_high=vaddr_read(desc_addr+4,4)&0xffff0000;
  offset=eip_low|eip_high;

  decoding.is_jmp=1;
  decoding.jmp_eip=offset;
}

void dev_raise_intr() {
}
