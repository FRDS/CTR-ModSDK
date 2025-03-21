#include <common.h>

struct LngFile
{
	int numStrings;
	int offsetToPtrArr;
	char strings[1];
};

// param_1 - Pointer to "cd position of bigfile"
// param_2 - language index - 0 ja, 1 en, 2 en2, 3 fr, 4 de, 5 it, 6 es, 7 ne
void DECOMP_LOAD_LangFile(int bigfilePtr, int lang)
{
	struct LngFile *lngFile;
	int size;

	int i;
	int numStrings;
	char **strArray;

#if BUILD == EurRetail
	// This is to turn the screen black for a bit (optional)
	DECOMP_CTR_ErrorScreen(0, 0, 0);
	VSync(0);
#endif

	if (sdata->lngFile == 0)
	{
		sdata->lngFile =
			DECOMP_MEMPACK_AllocMem(sdata->langBufferSize /* "lang buffer" */);
	}

	lngFile =
		DECOMP_LOAD_ReadFile(
			(struct BigHeader*)bigfilePtr, BI_LANGUAGEFILE + lang, 
			(void*)sdata->lngFile, NULL);

	// This is not ReadFileAsync, this is ReadFile,
	// so the program halts until completion of read

	numStrings = lngFile->numStrings;
	strArray = (char**)((unsigned int)lngFile + lngFile->offsetToPtrArr);

	sdata->numLngStrings = numStrings;
	sdata->lngStrings = strArray;

	for (i = 0; i < numStrings; i++)
	{
		strArray[i] =
			(char*)((unsigned int)strArray[i] +
			(unsigned int)lngFile);
	}
#if BUILD == EurRetail
	// set voicelines to new lang
	DECOMP_CDSYS_SetXAToLang(lang);
#endif
}