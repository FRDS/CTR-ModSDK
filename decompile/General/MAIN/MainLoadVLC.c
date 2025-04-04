#include <common.h>

void DECOMP_MainLoadVLC_Callback();

void DECOMP_MainLoadVLC(void)
{
  // VLC is not loaded
  sdata->bool_IsLoaded_VlcTable = 0;

  // This table is passed as parameter to DecDCTvlc2
  DECOMP_LOAD_AppendQueue(
	0, LT_SETADDR, 
	BI_VLCTABLE, 0, 
	DECOMP_MainLoadVLC_Callback);
}