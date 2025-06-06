
void FastAnim_Start(struct GameTracker* gGT)
{	
	for(int i = 0; i < gGT->numPlyrCurrGame; i++)
	{
		struct Driver* d = gGT->drivers[i];
		
		if(d == 0)
			continue;
		
		struct Instance* inst = d->instSelf;

#ifdef REBUILD_PC
		if (inst == 0)
			continue;
#endif

		struct Model* m = inst->model;

#ifdef REBUILD_PC
		if (m == 0)
			continue;
#endif
		
		for(int j = 0; j < m->numHeaders; j++)
		{
			struct ModelHeader* h = &m->headers[j];

			for(int k = 0; k < h->numAnimations; k++)
			{
				struct ModelAnim* ma = h->ptrAnimations[k];

				// if animation is already decompressed, skip
				if(ma->ptrDeltaArray == 0)
					continue;
				
				void FastAnim_Decompress(struct Model* m, int j, int k);
				FastAnim_Decompress(m, j, k);
				
				// if 2mb RAM, only first animation "turn"
				#ifndef USE_RAMEX
				break;
				#endif
			}
			
			// if 2mb RAM, only first header
			#ifndef USE_RAMEX
			break;
			#endif
		}
	}
}