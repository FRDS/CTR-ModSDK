#include <common.h>

void DECOMP_LOAD_NextQueuedFile()
{
	if(
		(sdata->queueReady != 0) &&
		(sdata->XA_State == 0) &&
		(sdata->queueLength != 0)
	)
	{
		sdata->queueReady = 0;
		
		struct LoadQueueSlot* curr = &data.currSlot;
		
		// retry previously-failed load
		if(sdata->queueRetry != 0)
		{
			sdata->queueRetry = 0;
		}
		
		// brand new load
		else
		{
			// Naughty Dog had inline copying,
			// is that faster on real PS1 hardware?
			
			memcpy(curr, &sdata->queueSlots[0], sizeof(struct LoadQueueSlot));
			
			for(int i = 1; i < sdata->queueLength; i++)
				memcpy(&sdata->queueSlots[i-1], &sdata->queueSlots[i], sizeof(struct LoadQueueSlot));
		}
		
		if(curr->type == LT_RAW)
		{
			curr->ptrDestination =
				DECOMP_LOAD_ReadFile(
					curr->ptrBigfileCdPos,
					LT_RAW,
					curr->subfileIndex,
					curr->ptrDestination,
					&curr->size,
					DECOMP_LOAD_CDRequestCallback);
		}
		
		else if(curr->type == LT_DRAM)
		{
			curr->ptrDestination =
				DECOMP_LOAD_DramFile(
					(void*)curr->ptrBigfileCdPos,
					(int)curr->subfileIndex,
					(int*)curr->ptrDestination,
					(int*)&curr->size,
					(int)curr->callback.funcPtr);
		}
		
		else if(curr->type == LT_VRAM)
		{
			curr->ptrDestination =
				DECOMP_LOAD_VramFile(
					(void*)curr->ptrBigfileCdPos,
					(int)curr->subfileIndex,
					(int*)curr->ptrDestination,
					(int*)&curr->size,
					(int)curr->callback.funcPtr);
		}
		
		sdata->queueLength--;

#if defined(REBUILD_PC) || defined(USE_PCDRV)
		DECOMP_LOAD_ReadFileASyncCallback(CdlComplete, NULL);
#endif

	}
	if(
		// two frames after end of loading
		(sdata->frameFinishedVRAM != 0) &&
		(2 < (unsigned int)(sdata->gGT->frameTimer_VsyncCallback - sdata->frameFinishedVRAM))
	)
	{
		struct LoadQueueSlot* curr = &data.currSlot;

		// Use callback if present
		if(curr->callback.funcPtr != 0)
		{
			(*curr->callback.funcPtr)(curr);
		}
		
		// reset timer
		sdata->frameFinishedVRAM = 0;
		
		sdata->queueReady = 1;
	}
}