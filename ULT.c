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

	ThrdCtlBlk *found=phead;

	if((wantTid == ULT_SELF) || (wantTid == currentTid))
	{
		if(debug) printf("%s","Found SELF, Returning currentTid\n");
		return currentTid;
	}
	else if((phead == NULL) && (wantTid == ULT_ANY))
	{
		if(debug) printf("%s","Returning ULT_NONE\n");
		return ULT_NONE;
	}
	else
	{
		if(wantTid == ULT_ANY)
		{
			found=phead;
		}
		else
		{
			while(found!=NULL)
			{
				if(found->tid==wantTid)
				{
					//get it
					break;
				}
				found=found->next;
			}
		}
		if(found != NULL)
		{
			assert(0);

			//switch the running thingy
			//struct ThrdCtlBlk *tcb =  malloc(sizeof(struct ThrdCtlBlk));
			//ucontext_t stopped = 
		}
	}
	
	
	if(debug) printf("%s","Returning ULT_INVALID\n");
	return ULT_INVALID;

}


Tid ULT_DestroyThread(Tid tid)
{
  assert(1); /* TBD */
  return ULT_FAILED;
}





