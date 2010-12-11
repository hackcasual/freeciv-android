/**********************************************************************
 Android-Freeciv - Copyright (C) 2010 - C Vaughn
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
***********************************************************************/

#include "sync.h"
#include <android/log.h>
#include <unwind.h>
#include <string.h>
#include <stdio.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "freecivjni", __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "freecivjni", __VA_ARGS__);

// =============================================================================
// stack trace functions
// =============================================================================

typedef struct
{
    size_t count;
    intptr_t* addrs;
} stack_crawl_state_t;


/* depends how the system includes define this */
#ifdef HAVE_UNWIND_CONTEXT_STRUCT
typedef struct _Unwind_Context __unwind_context;
#else
typedef _Unwind_Context __unwind_context;
#endif

pthread_mutex_t jni_lock = PTHREAD_MUTEX_INITIALIZER;

static _Unwind_Reason_Code trace_function(__unwind_context *context, void *arg)
{
    stack_crawl_state_t* state = (stack_crawl_state_t*)arg;
    if (state->count) {
        intptr_t ip = (intptr_t)_Unwind_GetIP(context);
        if (ip) {
            state->addrs[0] = ip;
            state->addrs++;
            state->count--;
            return _URC_NO_REASON;
        }
    }
    /*
     * If we run out of space to record the address or 0 has been seen, stop
     * unwinding the stack.
     */
    return _URC_END_OF_STACK;
}

static inline
int get_backtrace(intptr_t* addrs, size_t max_entries)
{
    stack_crawl_state_t state;
    state.count = max_entries;
    state.addrs = (intptr_t*)addrs;
    _Unwind_Backtrace(trace_function, (void*)&state);
    return max_entries - state.count;
}

void dump_stack_trace()
{
    intptr_t addrs[20];
    int c = get_backtrace(addrs, 20);
    char buf[16];
    char tmp[16*20];
    int i;

    tmp[0] = 0; // Need to initialize tmp[0] for the first strcat
    for (i=0 ; i<c; i++) {
        sprintf(buf, "%2d: %08x\n", i, addrs[i]);
        strlcat(tmp, buf, sizeof tmp);
    }
    LOGI(">>>call stack:\n%s", tmp);
}

int last_locked_by;
int lock_count = 0;
void civ_lock() {
	//LOGI("Acquiring lock for %d", pthread_self());
	if (pthread_self() == last_locked_by) {
		//LOGI("Already locked, incrementing count");
		lock_count++;
		return;
	}

	last_locked_by = pthread_self();

	pthread_mutex_lock(&jni_lock);
	//LOGI("Acquired lock for %d", pthread_self());
}

void civ_unlock() {
	if (lock_count > 0) {
		lock_count--;
		//LOGI("Releasing multi-lock for %d", pthread_self());
	}
	else {
		//LOGI("Releasing lock held by %d", pthread_self());
		pthread_mutex_unlock(&jni_lock);
		last_locked_by = NULL;
	}
}
