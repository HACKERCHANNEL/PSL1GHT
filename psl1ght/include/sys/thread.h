#pragma once

#include <psl1ght/types.h>

EXTERN_BEGIN
#define THREAD_JOINABLE 1

typedef u64 sys_ppu_thread_t;
typedef struct sys_ppu_thread_stack_t_32 {
	u32 addr;
	u32 size;
}sys_ppu_thread_stack_t_32;

typedef struct sys_ppu_thread_stack_t {
	void* addr;
	u32 size;
}sys_ppu_thread_stack_t;

typedef struct sys_lwmutex_t {
	u32 owner;
	u32 waiter;
	u32 attribute;
	u32 recursive_count;
	u32 sleep_queue;
	u32 pad;
}sys_lwmutex_t;

#define LWMUTEX_ATTR_PROTOCOL  0x0002
#define LWMUTEX_ATTR_RECURSIVE 0x0010
typedef struct sys_lwmutex_attribute_t {
	u32 attr_protocol;
	u32 attr_recursive;
	char name[8];
}sys_lwmutex_attribute_t;

#define MUTEX_ATTR_PROTOCOL  0x0002
#define MUTEX_ATTR_RECURSIVE 0x0020
#define MUTEX_ATTR_PSHARED   0x0200
#define MUTEX_ATTR_ADAPTIVE  0x2000
typedef u32 sys_mutex_t;
typedef struct sys_mutex_attribute_t {
	u32 attr_protocol;
	u32 attr_recursive;
	u32 attr_pshared;
	u32 attr_adaptive;
	u64 key;
	s32 flags;
	u32 pad;
	char name[8];
}sys_mutex_attribute_t;

#define COND_ATTR_PSHARED  0x0200
typedef u32 sys_cond_t;
typedef struct sys_cond_attribute_t {
	u32 attr_pshared;
	s32 flags;
	u64 key;
	char name[8];
}sys_cond_attribute_t;

#define SEMAPHORE_ATTR_PROTOCOL  0x0002
#define SEMAPHORE_ATTR_PSHARED   0x0200
typedef u32 sys_semaphore_t;
typedef struct sys_semaphore_attribute_t {
	u32 attr_protocol;
	u32 attr_pshared;
	u64 key;
	s32 flags;
	u32 pad;
	char name[8];
}sys_semaphore_attribute_t;

s32 sys_ppu_thread_create_ex(sys_ppu_thread_t * threadid, opd32* opdentry, u64 arg, s32 priority, u64 stacksize, u64 flags, char * threadname);

void sys_ppu_thread_yield();
s32 sys_ppu_thread_join(sys_ppu_thread_t threadid, u64 *retval);
s32 sys_ppu_thread_detach(sys_ppu_thread_t thread_id);
void sys_ppu_thread_get_join_state(s32 *isjoinable);
s32 sys_ppu_thread_set_priority(sys_ppu_thread_t threadid, s32  priority);
s32 sys_ppu_thread_get_priority(sys_ppu_thread_t threadid, s32* priority);
s32 sys_ppu_thread_get_stack_information(sys_ppu_thread_stack_t *stackinfo);
s32 sys_ppu_thread_rename(sys_ppu_thread_t id, char* name);
s32 sys_ppu_thread_recover_page_fault(sys_ppu_thread_t id);

s32 sys_mutex_create(sys_mutex_t * mutexid, const sys_mutex_attribute_t *attr);
s32 sys_mutex_destroy(sys_mutex_t mutexid);
s32 sys_mutex_lock(sys_mutex_t mutexid, u64 timeout_usec);
s32 sys_mutex_trylock(sys_mutex_t mutexid);
s32 sys_mutex_unlock(sys_mutex_t mutexid);
s32 sys_cond_create(sys_cond_t * condid, sys_mutex_t mutexid, const sys_cond_attribute_t *attr);
s32 sys_cond_destroy(sys_cond_t condid);
s32 sys_cond_wait(sys_cond_t condid, u64 timeout_usec);
s32 sys_cond_signal(sys_cond_t condid);
s32 sys_cond_signal_all(sys_cond_t condid);
s32 sys_semaphore_create(sys_semaphore_t *semaid, const sys_semaphore_attribute_t *attr, s32 initial_value, s32 max_value);
s32 sys_semaphore_destroy(sys_semaphore_t semaid);
s32 sys_semaphore_wait(sys_semaphore_t semaid, u64 timeout_usec);
s32 sys_semaphore_trywait(sys_semaphore_t semaid);
s32 sys_semaphore_post(sys_semaphore_t semaid, s32 count);

EXTERN_END
