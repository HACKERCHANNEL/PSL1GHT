#include <psl1ght/lv2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <io/pad.h>
#include <psl1ght/lv2/thread.h>
static void thread_start(u64 arg)
{
	int running = 0;
	sys_ppu_thread_t id;
	sys_ppu_thread_get_id(&id);
	
	
	sys_ppu_thread_stack_t stackinfo;
	sys_ppu_thread_get_stack_information(&stackinfo);
	printf("stack\naddr: %p\nsize: %08X\n", stackinfo.addr, stackinfo.size);
	while(running < 5){
		printf("Thread %08llX here\n", (unsigned long long int)id);
		sys_ppu_thread_yield();
		sleep(2);
		running++;
	}
	//you must call this, kthx
	sys_ppu_thread_exit(0);
}

int main(int argc, const char* argv[])
{
	PadInfo padinfo;
	PadData paddata;
	ioPadInit(7);
	
	sys_ppu_thread_t id;
	sys_ppu_thread_t id2;
	u64 thread_arg = 0x1337;
	u64 priority = 1500;
	size_t stack_size = 0x1000;
	char *thread_name = "My Thread";
	int s = sys_ppu_thread_create(	&id, thread_start, thread_arg, priority, stack_size, THREAD_JOINABLE, thread_name);


	
	printf("sys_ppu_thread_create returned %d:\n", s);
	s = sys_ppu_thread_create(	&id2, thread_start, thread_arg, priority, stack_size, THREAD_JOINABLE, thread_name);
	printf("sys_ppu_thread_create returned %d:\n", s);
	
	
	u64 retval;
	int t = sys_ppu_thread_join(id, &retval);
	printf("Thread tried to join with return: %llX, sys_ppu_thread_join returned %d\n", (unsigned long long int)retval, t);
	t = sys_ppu_thread_join(id2, &retval);
	printf("Thread tried to join with return: %llX, sys_ppu_thread_join returned %d\n", (unsigned long long int)retval, t);
	int running = 1;
	while(running){
		int i;
		ioPadGetInfo(&padinfo);
		for(i=0; i<MAX_PADS; i++){
			if(padinfo.status[i]){
				ioPadGetData(i, &paddata);
				
				if(paddata.BTN_CROSS){
					running = 0;
					break;
				}
			}
			
		}
	}
	printf("Exiting\n");
	return 0;
}
