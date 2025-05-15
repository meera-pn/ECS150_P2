#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

static struct uthread_tcb *curr_thread = NULL;
static queue_t ready_queue = NULL;

struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t context;  //private.h
	void *stack;
	// note: keep track of ready, running, etc. with: enum uthread_state state;
	// for sem?
};

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
	return curr_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	preempt_disable();
	
	//reassign curr thread, move on to next in queue
	struct uthread_tcb *prev = curr_thread;
	struct uthread_tcb *next;

	if (queue_dequeue(ready_queue, (void **) &next) == 0) {
	    queue_enqueue(ready_queue, prev);
	    curr_thread = next;
	    //next->state = RUNNING; //check ptr
	    //private.h
	    preempt_enable();
	    uthread_ctx_switch(&prev->context, &next->context);
	} // else { ...no returns
	else {
		preempt_enable();
	}
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	preempt_disable();
	struct uthread_tcb *prev = curr_thread;
	//private.h

	struct uthread_tcb *next;
	if (queue_dequeue(ready_queue, (void **) &next) == 0) {
		curr_thread = next;
		//next->state = RUNNING; //set prev to READY?
		preempt_enable();
		uthread_ctx_switch(&prev->context, &next->context);
		uthread_ctx_destroy_stack(prev->stack); //check address
		free(prev);
	} else {
		uthread_ctx_destroy_stack(prev->stack); //check address
		free(prev);
		preempt_enable();
		exit(0);
	}
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
/*
 * This function creates a new thread running the function @func to which
 * argument @arg is passed.
 */
	struct uthread_tcb *tcb = malloc(sizeof(*tcb));
    	if (!tcb) {
  		// malloc error => return -1
    		return -1;
    	}

	//private.h
	tcb->stack = uthread_ctx_alloc_stack();
	if (!tcb->stack) {
		free(tcb);
		//mem error => return -1
		return -1;
	}

	//private.h
	if (uthread_ctx_init(&tcb->context, tcb->stack, func, arg) != 0) {
		uthread_ctx_destroy_stack(tcb->stack);
		free(tcb);
		return -1;
	}

	//init new thread, add to queue w/ tcb
    	//tcb->state = READY;
  	queue_enqueue(ready_queue, tcb);
   	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */

/*
 * uthread_run - Run the multithreading library
 * @preempt: Preemption enable
 * @func: Function of the first thread to start
 * @arg: Argument to be passed to the first thread
 */
	preempt_start(preempt);
	
	if (curr_thread != NULL) return -1; //fail => return -1

	ready_queue = queue_create();
	if (!ready_queue) {
		return -1; //fail => return -1
	}

	//main thread tcb
	struct uthread_tcb idle_thread = {
		//.state = RUNNING,
		.stack = NULL
	};
	curr_thread = &idle_thread; //becomes idle once sched created

	if (uthread_create(func, arg) < 0) {
		return -1; //fail => return -1
	}

	//scheduling; work thru queue of threads until empty
	while (1) {
		struct uthread_tcb *next;
		if (queue_dequeue(ready_queue, (void **) &next) < 0) {
			break; //once all threads have returned, prep to return on success
		}

		struct uthread_tcb *prev = curr_thread;
		curr_thread = next;
		//next->state = RUNNING;
		uthread_ctx_switch(&prev->context, &next->context);
	}
	
	if (preempt) {
		preempt_stop();
	}
	return 0;  //success => return 0
}

void uthread_block(void)
{
	/* TODO Phase 3 */
	preempt_disable();
	/* TODO Phase 3 */
	struct uthread_tcb *prev = curr_thread;
	struct uthread_tcb *next; //oldest thread from queue will go here
	if(queue_dequeue(ready_queue, (void**) &next) == 0){
		curr_thread = next;
		preempt_enable();
		uthread_ctx_switch(&prev->context, &next->context);
	} 
	else{
		preempt_enable();
		exit(0);
	} 
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
	preempt_disable();
	queue_enqueue(ready_queue, uthread);
	preempt_enable();
}

