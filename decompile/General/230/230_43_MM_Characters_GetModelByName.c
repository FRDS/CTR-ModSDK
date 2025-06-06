#include <common.h>

#if 0
// Search for character model by string,
// specific to main menu lev, altered in oxide mod
struct Model* DECOMP_MM_Characters_GetModelByName(int *name) 
{
  struct Model** models;
  struct Model* model;
  struct Level* level1 = sdata->gGT->level1;

  // if LEV is invalid
  if (level1 == NULL)
    return NULL;

  models = level1->ptrModelsPtrArray;
  
  // loop through all models in array 
  // of model pointers, until nullptr
  for (
			model = models[0]; 
			model != NULL; 
			models++, model = models[0]
	  ) 
  {
    // if first 4 bytes of name are matching
    if (*(int*)model == *(int*)name) 
	{
	  // found it
      return model;
    }
  }
  return NULL;
}
#else
struct Model* MM_Characters_GetModelByID(int id)
{
  struct Model** models;
  struct Model* model;
  struct Level* level1 = sdata->gGT->level1;

  // if LEV is invalid
  if (level1 == NULL)
    return NULL;

  #ifdef USE_OXIDE
  if(id == 0xf)
	  return data.driverModelExtras[0];
  #endif
  
  // Only use Preload in the main menu
  // if an entirely new roster of characters
  // is ready. Otherwise it breaks animations
  
  // #ifdef USE_PRELOAD
  // int* arr = 0x8000a000;
  // return arr[id];
  // #endif

  models = level1->ptrModelsPtrArray;
  return models[0xE - id];
}
#endif