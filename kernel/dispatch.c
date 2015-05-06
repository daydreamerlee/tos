
#include <kernel.h>

#include "disptable.c"


PROCESS active_proc;


/*
 * Ready queues for all eight priorities.
 */
PCB *ready_queue [MAX_READY_QUEUES];




/*
 * add_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is put the ready queue.
 * The appropiate ready queue is determined by p->priority.
 */

void add_ready_queue (PROCESS proc)
{
  //PROCESS head;
  int prio;
  prio = proc->priority;
  assert (proc->magic == MAGIC_PCB);

  if (ready_queue [prio] != NULL)
  {
    proc->next = ready_queue [prio];
    proc->prev = ready_queue [prio]->prev;
    ready_queue [prio]->prev->next = proc;
    ready_queue [prio]->prev = proc;
  }
  else
  {
    ready_queue [prio] = proc;
    proc->next = proc;
    proc->prev = proc;
  }
  proc->state = STATE_READY;
}



/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue (PROCESS proc)
{
  int prio;
  prio = proc->priority;
  assert (proc->magic == MAGIC_PCB);
  if (proc->next == proc)
    ready_queue [prio] = NULL;
  else
  {
    ready_queue [prio] = proc->next;
    proc->next->prev = proc->prev;
    proc->prev->next = proc->next;
  }
}



/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */

PROCESS dispatcher()
{
  int prio;
  for ( prio = MAX_READY_QUEUES - 1; prio >= 0; prio--)
  {
    if (ready_queue [prio] == NULL)
      continue;
    else if (prio == active_proc->priority)
      return active_proc->next;
    else
      return ready_queue [prio];
  }
  return active_proc;
}



/*
 * resign
 *----------------------------------------------------------------------------
 * The current process gives up the CPU voluntarily. The
 * next running process is determined via dispatcher().
 * The stack of the calling process is setup such that it
 * looks like an interrupt.
 */
void resign()
{
	asm("pushfl;"
        "cli;"
        "popl %%eax;"
        "xchg (%%esp), %%eax;"
        "pushl %%cs;"
        "pushl %%eax;"
        "pushl %%eax;"
	"pushl %%ecx;"
	"pushl %%edx;"
	"pushl %%ebx;"
	"pushl %%ebp;"
	"pushl %%esi;"
	"pushl %%edi;"
	"movl %%esp, %0"
	: "=r" (active_proc->esp)
	:
	);
	active_proc = dispatcher();
	asm("movl %0, %%esp;"
	"popl %%edi;"
	"popl %%esi;"
	"popl %%ebp;"
	"popl %%ebx;"
	"popl %%edx;"
	"popl %%ecx;"
	"popl %%eax;"
        "iret"
	:
	: "r" (active_proc->esp)
	);
}



/*
 * init_dispatcher
 *----------------------------------------------------------------------------
 * Initializes the necessary data structures.
 */

void init_dispatcher()
{
  int i;
  for (i = 0; i < MAX_READY_QUEUES; i++)
    ready_queue [i] = NULL;

  add_ready_queue(active_proc);
}
