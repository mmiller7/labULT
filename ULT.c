#include <assert.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/* We want the extra information from these definitions */
#ifndef __USE_GNU
#define __USE_GNU
#endif /* __USE_GNU */
#include <ucontext.h>

#include "ULT.h"

int debug=1;

ThrdCtlBlk *phead=NULL;
Tid currentTid = 0;
Tid nextTid = 1;

Tid ULT_CreateThread(void (*fn)(void *), void *parg)
{
	if(debug) printf("%s","Creating new thread\n");
	
	if(nextTid>ULT_MAX_THREADS)
		return ULT_NOMORE;
	//TODO put something to return ULT_NOMEMORY


  assert(1); /* TBD */
	//need to create a tbc somewhere in here and stick the new thing in it
	if(debug)printf("%s","Creating a new tcb\n");
	struct ThrdCtlBlk *tcb =  malloc(sizeof(struct ThrdCtlBlk));

	//Allocate space for a new stack
	if(debug)printf("%s","Creating a new stack\n");
	void *stack = (void *) malloc(ULT_MIN_STACK);	
	//move pointer to top
	stack=stack+ULT_MIN_STACK;
	//stub(fn,parg);

	//push stuff on
	if(debug)printf("%s","Making room for parg\n");
	stack=stack-4;
	if(debug)printf("%s","Pushing parg\n");
	strcpy((char*)stack,parg);
	if(debug)printf("%s","Making room for fn\n");
	stack=stack-4;
	if(debug)printf("%s","Pushing fn\n");
	strcpy(stack,(char*)fn);

	//get a context so we have something to mess with
	if(debug)printf("%s","getting context\n");
	getcontext(&tcb->context);

	//change stack pointer to point to new stack
	if(debug)printf("%s","point stack pointer to new stack\n");
	tcb->context.uc_stack.ss_sp=stack;

	//change EIP to point to stub
	if(debug)printf("%s","changing EIP to point to stub function\n");
	tcb->context.uc_mcontext.gregs[REG_EIP]=(int)stub;


	//assign it the next tid and increment nextTid
	tcb->tid=nextTid;
	nextTid++;
	//Locate end of queue

	struct ThrdCtlBlk *end = phead;
	while((end != NULL) && (end->next != NULL))
		end=end->next;

	//store the new tcb struct in the queue
	if(debug)printf("%s","Adding it to queue\n");
	if(end == NULL)
		phead=tcb;
	else
		end->next=tcb;

	if(debug) printf("%s%d%s","Created new thread ",tcb->tid,"\n");
	return tcb->tid;
}



Tid ULT_Yield(Tid wantTid)
{
  assert(1); /* TBD */

	//Temporary pointer starting at front of queue
	ThrdCtlBlk *found=phead;

	if((wantTid == ULT_SELF) || (wantTid == currentTid))
	{
		//If you want the current thread, it's already running so do nothing
		if(debug) printf("%s","Found SELF, Returning currentTid\n");
		return currentTid;
	}
	else if((phead == NULL) && (wantTid == ULT_ANY))
	{
		//If you ask to switch to any other thread
		//but there's no threads in the queue
		//return error saying there is nothing to do
		if(debug) printf("%s","Returning ULT_NONE\n");
		return ULT_NONE;
	}
	else
	{
		//If we get here, it means there is at least one thread in
		//the queue and one running.  We can switch threads.
		if(wantTid == ULT_ANY)
		{
			//If they ask for "any" other thread
			//Get the first tcb in the queue
			found=phead;
		}
		else
		{
			//If they want a specific thread
			//Start looking for the tcb you want
			//WHILE there are still more threads to search through
			//when we hit the end, found will become null
			ThrdCtlBlk *tempPrev=NULL; //temp to redirect pointers
			while(found!=NULL)
			{
				//If you found the tcb you want...
				if(found->tid==wantTid)
				{
					//point around found
					if(tempPrev == NULL)
						phead=found->next;
					else
						tempPrev->next=found->next;
					//stop looking becaue we found it
					break;
				}
				//temp to redirect pointers
				tempPrev=found;
				//must not have found it, move to the next one
				found=found->next;
			}
		}
		if(debug) printf("%s","About to test if found!=NULL\n");
		//If we found something, it will be in found
		//If found is null, that means we ran out of things to look for
		//and still didn't find it.
		if(found != NULL)
		{
			if(debug) printf("%s","found is not null!\n");

			//found will be the tcb for the new thread

			//make tcb for currently running thread
			struct ThrdCtlBlk *tcb =  malloc(sizeof(struct ThrdCtlBlk));
			tcb->tid=currentTid;
			if(debug) printf("%s","Made a tcb\n");

			//Locate end of queue
			struct ThrdCtlBlk *end = phead;
			while((end != NULL) && (end->next != NULL))
				end=end->next;
			if(debug) printf("%s","Found end of queue\n");

			//store the new tcb struct in the queue
			if(end == NULL)
				phead=tcb;
			else
				end->next=tcb;
			if(debug) printf("%s","Stored tcb in queue\n");

			//save the currently running thread
			getcontext(&tcb->context);
			if(debug) printf("%s","Context gotted\n");

			//do the IP jump for what we're about to load
			//needed to skip the load instruction and prevent infinite loop
			tcb->context.uc_mcontext.gregs[REG_EIP]=tcb->context.uc_mcontext.gregs[REG_EIP]+76;
			if(debug) printf("%s","jumped\n");

			//set the new context
			setcontext(&found->context);
			if(debug) printf("%s","Context setted\n");

			return currentTid;
		}
		else
		{
			if(debug) printf("%s","found is NULL!\n");
		}
	}
	
	//If we get here, it means we couldn't figure out what we're looking
	//for so it must be invalid.  Either it was trash or isn't in the queue.
	if(debug) printf("%s","Returning ULT_INVALID\n");
	return ULT_INVALID;

}


Tid ULT_DestroyThread(Tid tid)
{
  assert(1); /* TBD */
  return ULT_FAILED;
}

void stub(void (*root)(void *), void *arg)
{
		if(debug) printf("%s","In the stub\n");
		//if(debug) printf("%s%s%s","Running stub passed arg ",(char)arg,"\n");
    // thread starts here
		Tid ret;
	  root(arg); // call root function
    ret = ULT_DestroyThread(ULT_SELF);
		assert(ret == ULT_NONE); // we should only get here if we are the last thread.
		exit(0); // all threads are done, so process should exit
}
