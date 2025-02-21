#include <common.h>

void DECOMP_LOAD_VramFileCallback(struct LoadQueueSlot* lqs)
{	
	int* vramBuf = lqs->ptrDestination;

	if(vramBuf != 0)
	{
		struct VramHeader* vh = (struct VramHeader*)vramBuf;
		
		// if multiple TIMs are packed together
		if(vramBuf[0] == 0x20)
		{
			int size = vramBuf[1];
			vh = (struct VramHeader*)&vramBuf[2];
			
			while(size != 0)
			{
				LoadImage(&vh->rect, VRAMHEADER_GETPIXLES(vh));
				
				vramBuf = (int*)vh;
				vramBuf = &vramBuf[size>>2];
				size = vramBuf[0];
				vh = (struct VramHeader*)&vramBuf[1];
			}
		}
		
		// if just one TIM
		else
		{
			LoadImage(&vh->rect, VRAMHEADER_GETPIXLES(vh));
		}
	}
	
	sdata->frameFinishedVRAM = sdata->gGT->frameTimer_VsyncCallback;
	
	// BUGFIX: The OG CTR can crash in the loading screen
	if (sdata->frameFinishedVRAM == 0)
		sdata->frameFinishedVRAM = 1;
}