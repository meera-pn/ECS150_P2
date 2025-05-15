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
	printf("uthread_yield called!\n");
	/* TODO Phase 2 */
	//reassign curr thread, move on to next in queue
	preempt_disable();
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
	else{
		preempt_enable();
	}
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	//private.h
	printf(">>> exiting current thread\n");
	preempt_disable();
	//uthread_ctx_destroy_stack(curr_thread->stack); //check address
	//free(curr_thread);

	struct uthread_tcb *next;
	struct uthread_tcb *ph = curr_thread;
	if (queue_dequeue(ready_queue, (void **) &next) == 0) {
		curr_thread = next;
		preempt_enable();
		//next->state = RUNNING; //set prev to READY?
		//uthread_ctx_switch(NULL, &next->context);
		uthread_ctx_switch(&ph->context, &next->context);
		//uthread_ctx_destroy_stack(curr_thread->stack);
		uthread_ctx_destroy_stack(ph->stack);
		//free(curr_thread);
		free(ph);
	} else {
		preempt_enable();
		//uthread_ctx_destroy_stack(curr_thread->stack);
		if (ph->stack) uthread_ctx_destroy_stack(ph->stack);
		//free(curr_thread);
		free(ph);
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
	printf("Thread %s created\n", (char*)arg);
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
	
	if (curr_thread != NULL) return -1; //fail => return -1

	ready_queue = queue_create();
	if (!ready_queue) {
		return -1; //fail => return -1
	}

	
	//main thread tcb
	static struct uthread_tcb idle_thread = {
		//.state = RUNNING,
		.stack = NULL
	};
	curr_thread = &idle_thread; //becomes idle once sched created


	if (uthread_create(func, arg) < 0) {
		return -1; //fail => return -1
	}

	//DEBUG
	sigset_t current;
	
	sigprocmask(0, NULL, &current);
	if (sigismember(&current, SIGVTALRM)) {
    	printf("IGVTALRM blocked in uthread_run()\n");
	} else {
    	printf("SIGVTALRM not blocked in uthread_run()\n");
	}	
	//DEBUG
	//scheduling; work thru queue of threads until empty
	/*
	static struct uthread_tcb main_tcb;
	main_tcb.stack = NULL;
	getcontext(&main_tcb.context);
	curr_thread = &main_tcb;
	*/

	while (1) {
		struct uthread_tcb *next;
		if (queue_dequeue(ready_queue, (void **) &next) < 0) {
			break; //once all threads have returned, prep to return on success
		}

		struct uthread_tcb *prev = curr_thread;
		curr_thread = next;
		//next->state = RUNNING;
		/*
		if(prev){
			uthread_ctx_switch(&prev->context, &next->context);
		}
		else{
			uthread_ctx_switch(NULL, &next->context);
		}*/
		uthread_ctx_switch(&prev->context, &next->context);
		preempt_start(preempt);
	}
	preempt_stop();
	return 0;  //success => return 0
}

void uthread_block(void)
{
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

