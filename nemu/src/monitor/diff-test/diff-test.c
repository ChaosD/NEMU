#include <dlfcn.h>

#include "nemu.h"
#include "monitor/monitor.h"
#include "diff-test.h"

static void (*ref_difftest_memcpy_from_dut)(paddr_t dest, void *src, size_t n);
static void (*ref_difftest_getregs)(void *c);
static void (*ref_difftest_setregs)(const void *c);
static void (*ref_difftest_exec)(uint64_t n);

static bool is_skip_ref;
static bool is_skip_dut;

void difftest_skip_ref() { is_skip_ref = true; }
void difftest_skip_dut() { is_skip_dut = true; }

void init_difftest(char *ref_so_file, long img_size) {
#ifndef DIFF_TEST
  return;
#endif

  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY | RTLD_DEEPBIND);
  assert(handle);

  ref_difftest_memcpy_from_dut = dlsym(handle, "difftest_memcpy_from_dut");
  assert(ref_difftest_memcpy_from_dut);

  ref_difftest_getregs = dlsym(handle, "difftest_getregs");
  assert(ref_difftest_getregs);

  ref_difftest_setregs = dlsym(handle, "difftest_setregs");
  assert(ref_difftest_setregs);

  ref_difftest_exec = dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  void (*ref_difftest_init)(void) = dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

  Log("Differential testing: \33[1;32m%s\33[0m", "ON");
  Log("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in include/common.h.", ref_so_file);

  ref_difftest_init();
  ref_difftest_memcpy_from_dut(ENTRY_START, guest_to_host(ENTRY_START), img_size);
  ref_difftest_setregs(&cpu);
}

void difftest_step(uint32_t eip) {
  CPU_state ref_r;

  if (is_skip_dut) {
    is_skip_dut = false;
    return;
  }

  if (is_skip_ref) {
    // to skip the checking of an instruction, just copy the reg state to reference design
    ref_difftest_setregs(&cpu);
    is_skip_ref = false;
    return;
  }

  ref_difftest_exec(1);
  ref_difftest_getregs(&ref_r);
/*
  // TODO: Check the registers state with the reference design.
  // Set `nemu_state` to `NEMU_ABORT` if they are not the same.
 	if(ref_r.eax != cpu.eax){
		printf("qemu.eax = 0x%x, nemu.eax = 0x%x", ref_r.eax, cpu.eax);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.ecx != cpu.ecx){
		printf("qemu.ecx = 0x%x, nemu.ecx = 0x%x", ref_r.ecx, cpu.ecx);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.edx != cpu.edx){
		printf("qemu.edx = 0x%x, nemu.edx = 0x%x", ref_r.edx, cpu.edx);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.ebx != cpu.ebx){
		printf("qemu.ebx = 0x%x, nemu.ebx = 0x%x", ref_r.ebx, cpu.ebx);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.esp != cpu.esp){
		printf("qemu.esp = 0x%x, nemu.esp = 0x%x", ref_r.esp, cpu.esp);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.ebp != cpu.ebp){
		printf("qemu.ebp = 0x%x, nemu.ebp = 0x%x", ref_r.ebp, cpu.ebp);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.esi != cpu.esi){
		printf("qemu.esi = 0x%x, nemu.esi = 0x%x", ref_r.esi, cpu.esi);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.edi != cpu.edi){
		printf("qemu.edi = 0x%x, nemu.edi = 0x%x", ref_r.edi, cpu.edi);
		nemu_state = NEMU_ABORT;
	}	
 	if(ref_r.eip != cpu.eip){
		printf("qemu.eip = 0x%x, nemu.eip = 0x%x", ref_r.eip, cpu.eip);
		nemu_state = NEMU_ABORT;
	}*/	

}
