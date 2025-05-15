#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
 
#define HZ 100

/*
Install a signal handler that receives alarm signals (of type SIGVTALRM)
Configure a timer which will fire an alarm (through a SIGVTALRM signal) a hundred times per second (i.e. 100 Hz)
*/
static struct sigaction orig_action;
static struct itimerval orig_timer;
bool enabled_preempt;

static void alarm_handler(int signum)
{
    (void)signum;
	printf(">>> alarm_handler called\n");
    if (enabled_preempt)
        uthread_yield(); //from uthread
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
/*
 * preempt_disable - Disable preemption
 */
  	if(!enabled_preempt) {
 		return;
 	}
  	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGVTALRM);

	sigprocmask(SIG_BLOCK, &sigset, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
/*
 * preempt_enable - Enable preemption
 */
  	if(!enabled_preempt) {
 		return;
 	}
 	
 	//signal(SIGVTALRM, alarm_handler);
  	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGVTALRM);

	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
}


void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
/*
 * preempt_start - Start thread preemption
 * @preempt: Enable preemption if true
 *
 * Configure a timer that must fire a virtual alarm at a frequency of 100 Hz and
 * setup a timer handler that forcefully yields the currently running thread.
 *
 * If @preempt is false, don't start preemption; all the other functions from
 * the preemption API should then be ineffective. => need global bool for preempt enabled
 */
 
 	if(!preempt) {
 		return;
 	}

 	enabled_preempt = true;
 		
 	//https://man7.org/linux/man-pages/man2/sigaction.2.html
 	//https://www.gnu.org/software/libc/manual/html_node/Signal-Sets.html
 	
 	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = alarm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGVTALRM, &sa, &orig_action) < 0) {
    		perror("sigaction");
	}

	//100Hz timer
	struct itimerval timer;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000000/HZ;
	timer.it_value = timer.it_interval;

	if (setitimer(ITIMER_VIRTUAL, &timer, &orig_timer) < 0) {
    		perror("setitimer");
	}

 	
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
/*
 * preempt_stop - Stop thread preemption
 *
 * Restore previous timer configuration, and previous action associated to
 * virtual alarm signals.
 */
 	if(!enabled_preempt) {
 		return;
 	}
 	//TODO: stop preempt
	/*
 	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = alarm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGVTALRM, &sa, &orig_action) < 0) {
    		perror("sigaction");
	}
	*/

	if (sigaction(SIGVTALRM, &orig_action, NULL) < 0){
		perror("restore");
	}

	if (setitimer(ITIMER_VIRTUAL, &orig_timer, NULL) < 0) {
		perror("restore");
	}
 	
 	enabled_preempt = false;
}

