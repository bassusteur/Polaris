#include <debug/debug.h>
#include <kernel.h>
#include <sched/sched.h>
#include <sys/prcb.h>

extern void user_thread(void);

void kernel_main(void *args) {
	// thread_create((uintptr_t)user_thread, 0, 1, prcb_return_current_cpu()->running_thread->mother_proc);
	process_create("user_process", 0, 2000, (uintptr_t)user_thread, 0, 1);
	kprintf("Got args 0x%x\n", args);
	kprintf("Hello I am %s\n",
			prcb_return_current_cpu()->running_thread->mother_proc->name);
	kprintf("Creating user process\n");
	for (;;)
		;
}
