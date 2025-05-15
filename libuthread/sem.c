#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
	/* TODO Phase 3 */
	size_t count;
	queue_t blocked;
};

sem_t sem_create(size_t count)
{
	/* TODO Phase 3 */
	sem_t sem = malloc(sizeof(struct semaphore));
	if(!sem){
		return NULL;
	}

	sem->count = count;
	sem->blocked = queue_create();

	if(!sem->blocked){
		free(sem);
		return NULL;
	}
	return sem;
}

int sem_destroy(sem_t sem)
{
	//TODO Phase 3 
	if(!sem || queue_length(sem->blocked) > 0){
		return -1;
	}
	queue_destroy(sem->blocked);
	free(sem);
	return 0;
}

//writing these under the assumption that uthread and preemption are done, will move on to preemption after if needed
int sem_down(sem_t sem)
{
	// TODO Phase 3 
	if(!sem){
		return -1;
	}
	preempt_disable();
	if(sem->count > 0){
		sem->count--;
		preempt_enable();
		return 0;
	}
	struct uthread_tcb *current  = uthread_current();
	queue_enqueue(sem->blocked, current);
	uthread_block();
	preempt_enable();
	uthread_yield();

	return 0;
}

int sem_up(sem_t sem)
{
	//TODO Phase 3
	if(!sem){
		return -1;
	}
	preempt_disable();
	if(queue_length(sem->blocked) > 0){
		struct uthread_tcb *tcb;
		queue_dequeue(sem->blocked, (void**)&tcb);
		uthread_unblock(tcb);
	}
	else{
		sem->count++;
	}
	preempt_enable();
	return 0;
}

