#include <assert.h>
#include<stdlib.h>
#include<stdio.h>

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
  assert(1); /* TBD */

  return ULT_FAILED;
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
		//If we found something, it will be in found
		//If found is null, that means we ran out of things to look for
		//and still didn't find it.
		if(found != NULL)
		{
			assert(0);

			//found will be the tcb for the new thread

			//do the IP jump for what we're about to load
			//needed to skip the load instruction and prevent infinite loop
			//TODO

			//make tcb for currently running thread
			struct ThrdCtlBlk *tcb =  malloc(sizeof(struct ThrdCtlBlk));

			//save the currently running thread
			getcontext(&tcb->context);

			//set the new context
			setcontext(&found->context);
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





