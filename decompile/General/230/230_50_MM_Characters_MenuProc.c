#include <common.h>

void DECOMP_MM_Characters_MenuProc(struct RectMenu* unused)
{
	u_char numPlyrNextGame;
	int bVar2;
	int bVar3;
	u_short characterSelectFlags5bit;
	short* psVar5;
	short sVar6;
	short nextDriver;
	int iVar8;
	u_int button;
	short sVar10;
	int globalIconPerPlayerCopy5;
	u_char* puVar12;
	u_int characterSelectType;
	u_int fontType;
	u_int iconColor;
	short globalIconPerPlayerCopy3;
	int nextDriverCopy;
	short globalIconPerPlayerCopy4;
	short globalIconPerPlayerCopy;
	short globalIconPerPlayerCopy2;
	u_short* globalIconPerPlayerPtr2;
	int iVar24;
	u_int k;
	u_short* puVar26;
	short globalIconPerPlayer[4];
	u_char auStack120[8];
	Color color;

	u_char colorRGBA[4];

	#ifdef REBUILD_PC
	RECT r1;
	RECT* r = &r1;
	#else
	RECT* r = (RECT*)0x1f800000; //todo: replace 0x1f800000 with reference to scratchpad
	#endif
	RECT r58;

	short local_50;
	short* globalIconPerPlayerPtr;

	int i;
	int j;
	int posX;
	int posY;
	short playerIcon;
	int direction;

	short* psVar22;
	struct CharacterSelectMeta* csm_Active;

	struct GameTracker* gGT = sdata->gGT;
	
	u_long* ot = gGT->backBuffer->otMem.startPlusFour;

	for (i = 0; i < 4; i++)
	{
		globalIconPerPlayer[i] = D230.characterMenuID[data.characterIDs[i]];
	}

	// if menu is not in focus
	if (D230.isMenuTransitioning != 1)
	{
		DECOMP_MM_TransitionInOut(D230.ptrTransitionMeta, (int)D230.transitionFrames, FPS_DOUBLE(8));
	}

	DECOMP_MM_Characters_SetMenuLayout();
	DECOMP_MM_Characters_DrawWindows(1);

	// if transitioning in
	if (D230.isMenuTransitioning == 0)
	{
		// if no more frames
		if (D230.transitionFrames == 0)
		{
			// menu is now in focus
			D230.isMenuTransitioning = 1;
		}
		else
		{
			D230.transitionFrames--;
		}
	}

	// if transitioning out
	if (D230.isMenuTransitioning == 2)
	{
		// increase frame
		D230.transitionFrames++;

		// if more than 12 frames
		if (D230.transitionFrames > FPS_DOUBLE(12))
		{
			// Make a backup of the characters
			// you selected in character selection screen
			DECOMP_MM_Characters_BackupIDs();

			DECOMP_MM_Characters_HideDrivers();

			// if returning to main menu
			if (D230.movingToTrackMenu == 0)
			{
				DECOMP_MM_JumpTo_Title_Returning();
				return;
			}

			// if you are in a cup
			if ((gGT->gameMode2 & CUP_ANY_KIND) != 0)
			{
				sdata->ptrDesiredMenu = &D230.menuCupSelect;
				DECOMP_MM_CupSelect_Init();
				return;
			}
			
			#if defined(USE_LEVELDEV) || defined(USE_LEVELDISC)
			gGT->currLEV = CUSTOM_LEVEL_ID;
			MainRaceTrack_RequestLoad(CUSTOM_LEVEL_ID);
			sdata->ptrActiveMenu = 0;
			return;
			#endif

			// if going to track selection
			sdata->ptrDesiredMenu = &D230.menuTrackSelect;
			DECOMP_MM_TrackSelect_Init();
			return;
		}
	}

	posX = D230.ptrTransitionMeta[15].currX;
	posY = D230.ptrTransitionMeta[15].currY;

	char* characterSelectString;
	switch(D230.characterSelectIconLayout)
	{
		// 3P character selection
		case 2:

			// If you have a lot of characters unlocked, do not draw SELECT CHARACTER
			if (D230.isRosterExpanded) goto dontDrawSelectCharacter;

			// SELECT
			DECOMP_DecalFont_DrawLine
			(
				sdata->lngStrings[96],
				posX + 0x9c,
				posY + 0x14,
				FONT_BIG, (JUSTIFY_CENTER | ORANGE)
			);
			characterSelectType = FONT_BIG;

			// CHARACTER
			characterSelectString = sdata->lngStrings[97];

			posX = posX + 0x9c;
			posY = posY + 0x26;
			break;

		// 4P character selection
		case 3:

			// If Fake Crash is unlocked, do not draw "Select Character"
			if (sdata->gameProgress.unlocks[0] & 0x800) goto dontDrawSelectCharacter;

			// SELECT
			DECOMP_DecalFont_DrawLine
			(
				sdata->lngStrings[96],
				posX + 0xfc,
				posY + 8,
				FONT_CREDITS, (JUSTIFY_CENTER | ORANGE)
			);
			characterSelectType = FONT_CREDITS;

			// CHARACTER
			characterSelectString = sdata->lngStrings[97];

			posX = posX + 0xfc;
			posY = posY + 0x18;
			break;

		// If you are in 1P or 2P character selection,
		// when you do NOT have a lot of characters selected
		case 4:
		case 5:
			characterSelectType = FONT_BIG;

			// SELECT CHARACTER
			characterSelectString = sdata->lngStrings[95];

			posX = posX + 0xfc;
			posY = posY + 10;
			break;

		default:
			goto dontDrawSelectCharacter;
	}

	// Draw String
	DECOMP_DecalFont_DrawLine(characterSelectString, posX, posY, characterSelectType, (JUSTIFY_CENTER | ORANGE));

	dontDrawSelectCharacter:

	globalIconPerPlayerPtr = &globalIconPerPlayer[0];

	for (i = 0; i < gGT->numPlyrNextGame; i++)
	{
		characterSelectFlags5bit = (u_short)(1 << i);
		globalIconPerPlayerCopy = globalIconPerPlayerPtr[i];
		globalIconPerPlayerCopy2 = globalIconPerPlayerCopy;

		DECOMP_MM_Characters_AnimateColors(auStack120, i, (int)(short)(sdata->characterSelectFlags & characterSelectFlags5bit));

		puVar26 = (u_short*)&D230.csm_Active[globalIconPerPlayerCopy];

		if
		(
			#ifdef USE_PROFILER
			((gGT->gameMode1 & DEBUG_MENU) == 0) &&
			#endif
		
			(D230.isMenuTransitioning == 1) &&
			(
				// get input from this player
				button = sdata->buttonTapPerPlayer[i],

				// If you press the D-Pad, or Cross, Square, Triangle, Circle
				button & (BTN_TRIANGLE | BTN_CIRCLE | BTN_SQUARE_one | BTN_CROSS_one | BTN_RIGHT | BTN_LEFT | BTN_DOWN | BTN_UP)
			)
		)
		{
			// if character has not been selected by this player
			if (((int)(short)sdata->characterSelectFlags >> i & 1U) == 0)
			{
				// If you pressed any of the D-pad buttons
				if ((button & (BTN_RIGHT | BTN_LEFT | BTN_DOWN | BTN_UP)) != 0)
				{
					local_50 = 0;

					// If you do not press Up
					if ((button & BTN_UP) == 0)
					{
						// If you do not press Down
						if ((button & BTN_DOWN) == 0)
						{
							// This must be if you press Left,
							// because the variable will change
							// if it is anything that isn't Left

							// Left
							direction = 2;

							// If you press Left
							if ((button & BTN_LEFT) != 0) goto LAB_800aec08;

							// At this point, you must have pressed Right

							// Right
							direction = 3;

							// Move down character selection list
							D230.characterSelect_MoveDir[i] = 1;
						}

						// If you pressed Down
						else
						{
							// Down
							direction = 1;

							// Move down character selection list
							D230.characterSelect_MoveDir[i] = 1;
						}
					}

					// If you pressed Up
					else
					{
						// Up
						direction = 0;
						LAB_800aec08:
						// If you press Up or Left

						// Move up character selection list
						(D230.characterSelect_MoveDir)[i] = 0xffff;
					}

					j = i;
					globalIconPerPlayerPtr2 = &globalIconPerPlayerPtr[j];
					globalIconPerPlayerCopy3 = globalIconPerPlayerCopy2;
					do
					{
						globalIconPerPlayerCopy2 = DECOMP_MM_Characters_GetNextDriver(direction, globalIconPerPlayerCopy3);
						globalIconPerPlayerCopy4 = globalIconPerPlayerCopy2;

						if (globalIconPerPlayerCopy2 == globalIconPerPlayerCopy3)
						{
							local_50 = 1;
							nextDriver = DECOMP_MM_Characters_GetNextDriver(direction, (int)(short)*globalIconPerPlayerPtr2);
							nextDriverCopy = (int)nextDriver;
							globalIconPerPlayerCopy2 = DECOMP_MM_Characters_GetNextDriver((u_int)(u_char)D230.getNextDriver1[direction], nextDriverCopy);
							globalIconPerPlayerCopy5 = (int)(short)globalIconPerPlayerCopy2;

							if
							(
								(((globalIconPerPlayerCopy5 == globalIconPerPlayerCopy4) || (nextDriverCopy == globalIconPerPlayerCopy4)) || (nextDriverCopy == globalIconPerPlayerCopy5)) ||
								(button = DECOMP_MM_Characters_boolIsInvalid(globalIconPerPlayerPtr, globalIconPerPlayerCopy5, j), (button & 0xffff) != 0)
							)
							{
								nextDriver = DECOMP_MM_Characters_GetNextDriver((u_int)(u_char)D230.getNextDriver1[direction], (int)(short)*globalIconPerPlayerPtr2);
								globalIconPerPlayerCopy5 = (int)nextDriver;
								globalIconPerPlayerCopy2 = DECOMP_MM_Characters_GetNextDriver(direction, globalIconPerPlayerCopy5);
								globalIconPerPlayerCopy4 = (int)(short)globalIconPerPlayerCopy2;

								if
								(
									((globalIconPerPlayerCopy4 == globalIconPerPlayerCopy3) || (globalIconPerPlayerCopy5 == globalIconPerPlayerCopy3)) ||
									(
										(globalIconPerPlayerCopy5 == globalIconPerPlayerCopy4 ||
										(button = DECOMP_MM_Characters_boolIsInvalid(globalIconPerPlayerPtr, globalIconPerPlayerCopy4, j), (button & 0xffff) != 0))
									)
								)
								{
									nextDriver = DECOMP_MM_Characters_GetNextDriver(direction, (int)(short)*globalIconPerPlayerPtr2);
									globalIconPerPlayerCopy5 = (int)nextDriver;
									globalIconPerPlayerCopy2 = DECOMP_MM_Characters_GetNextDriver((u_int)(u_char)D230.getNextDriver2[direction], globalIconPerPlayerCopy5);
									globalIconPerPlayerCopy4 = (int)(short)globalIconPerPlayerCopy2;

									if
									(
										((globalIconPerPlayerCopy4 == globalIconPerPlayerCopy3) || (globalIconPerPlayerCopy5 == globalIconPerPlayerCopy3)) ||
										(
											(
												globalIconPerPlayerCopy5 == globalIconPerPlayerCopy4 ||
												(button = DECOMP_MM_Characters_boolIsInvalid(globalIconPerPlayerPtr, globalIconPerPlayerCopy4, j), (button & 0xffff) != 0)
											)
										)
									)
									{
										nextDriver = DECOMP_MM_Characters_GetNextDriver((u_int)(u_char)D230.getNextDriver2[direction], (int)(short)*globalIconPerPlayerPtr2);
										globalIconPerPlayerCopy5 = (int)nextDriver;
										globalIconPerPlayerCopy2 = DECOMP_MM_Characters_GetNextDriver(direction, globalIconPerPlayerCopy5);
										globalIconPerPlayerCopy4 = (int)(short)globalIconPerPlayerCopy2;

										if
										(
											(((globalIconPerPlayerCopy4 == globalIconPerPlayerCopy3) || (globalIconPerPlayerCopy5 == globalIconPerPlayerCopy3)) || (globalIconPerPlayerCopy5 == globalIconPerPlayerCopy4)) ||
											(button = DECOMP_MM_Characters_boolIsInvalid(globalIconPerPlayerPtr, globalIconPerPlayerCopy4, j), (button & 0xffff) != 0)
										)
										{
											globalIconPerPlayerCopy2 = (u_int)*globalIconPerPlayerPtr2;
										}
									}
								}
							}
						}
						bVar2 = false;

						for (k = 0; k < (u_int)gGT->numPlyrNextGame; k++)
						{
							if(((int)k != j) && ((short)globalIconPerPlayerCopy2 == globalIconPerPlayerPtr[k]))
							{
								bVar2 = true;
								break;
							}
						}

						if (globalIconPerPlayerCopy3 << 0x10 != globalIconPerPlayerCopy2 << 0x10)
						{
							// Play sound
							DECOMP_OtherFX_Play(0, 1);
						}
						if (local_50 != 0)
						{
							bVar3 = !bVar2;
							bVar2 = false;
							if (bVar3) break;
							globalIconPerPlayerCopy2 = (u_int)*globalIconPerPlayerPtr2;
						}
						globalIconPerPlayerCopy3 = globalIconPerPlayerCopy2;
					} while (bVar2);
				}
				globalIconPerPlayerCopy = (u_short)globalIconPerPlayerCopy2;

				for (j = 0; j < gGT->numPlyrNextGame; j++)
				{
					if ((j != i) && ((short)globalIconPerPlayerCopy2 == globalIconPerPlayerPtr[j]))
					{
						globalIconPerPlayerCopy2 = (u_int)(u_short)globalIconPerPlayerPtr[i];
					}
					globalIconPerPlayerCopy = (u_short)globalIconPerPlayerCopy2;
				}

				// If this player pressed Cross or Circle
				if (((sdata->buttonTapPerPlayer)[i] & (BTN_CIRCLE | BTN_CROSS_one)) != 0)
				{
					// this player has now selected a character
					sdata->characterSelectFlags = sdata->characterSelectFlags | (u_short)(1 << i);

					numPlyrNextGame = gGT->numPlyrNextGame;

					// Play sound
					DECOMP_OtherFX_Play(1,1);

					// if all players have selected their characters
					if ((int)(short)sdata->characterSelectFlags == (1 << numPlyrNextGame)-1)
					{
						// move to track selection
						D230.movingToTrackMenu = 1;
						D230.isMenuTransitioning = 2;
					}
				}

				if
				(
					// if this is the first iteration of the loop
					((i & 0xffff) == 0) &&

					// if you press Square or Triangle
					((sdata->buttonTapPerPlayer[0] & (BTN_TRIANGLE | BTN_SQUARE_one)) != 0)
				)
				{
					// return to main menu
					D230.movingToTrackMenu = 0;
					D230.isMenuTransitioning = 2;

					// Play sound
					DECOMP_OtherFX_Play(2, 1);
				}
			}
			else
			{
				// if you press Square or Triangle
				if ((button & (BTN_TRIANGLE | BTN_SQUARE_one)) != 0)
				{
					// Play sound
					DECOMP_OtherFX_Play(2, 1);

					// this player has de-selected their character
					sdata->characterSelectFlags = sdata->characterSelectFlags & ~characterSelectFlags5bit;
				}
			}

			// clear input
			sdata->buttonTapPerPlayer[i] = 0;
		}

		globalIconPerPlayerPtr[i] = globalIconPerPlayerCopy;

		// transition of each icon
		iVar24 = (int)&D230.ptrTransitionMeta[globalIconPerPlayerCopy];

		#ifdef USE_OXIDE
		if (globalIconPerPlayerCopy == NITROS_OXIDE)
		{
			// use same transition as Fake Crash
			iVar24 = &D230.ptrTransitionMeta[FAKE_CRASH];
		}
		#endif

		// if player has not selected a character
		if (((sdata->characterSelectFlags >> i) & 1U) == 0)
		{
			// draw string
			// "1", "2", "3", "4", above the character icon
			DECOMP_DecalFont_DrawLine
			(
				D230.PlayerNumberStrings[i],
				((struct TransitionMeta*)iVar24)->currX + (u_int)*puVar26 + -6,
				((struct TransitionMeta*)iVar24)->currY + (u_int)puVar26[1] + -3,
				FONT_BIG, WHITE
			);
			puVar12 = auStack120;
		}
		else
		{
			puVar12 = (u_char*)&D230.characterSelect_Outline;
		}

		r->x = ((struct TransitionMeta*)iVar24)->currX + *puVar26;
		r->y = ((struct TransitionMeta*)iVar24)->currY + puVar26[1];
		r->w = 0x34;
		r->h = 0x21;

		color = *(Color *) puVar12;
		DECOMP_RECTMENU_DrawOuterRect_HighLevel(r, color, 0, ot);
	}

	DECOMP_MM_Characters_PreventOverlap();

	csm_Active = D230.csm_Active;

#ifndef USE_OXIDE
	#define NUM_ICONS 0xF
#else
	#define NUM_ICONS 0x10
#endif

	// loop through character icons
	for (i = 0; i < NUM_ICONS; i++)
	{
		iVar8 = csm_Active->unlockFlags;
		if
		(
			// If Icon is unlocked by default,
			// dont use iVar8, must be interpeted as "short"
			((short)csm_Active->unlockFlags == -1) ||

			// if character is unlocked
			// from 4-byte variable that handles all rewards
			// also the variable written by cheats
			(((sdata->gameProgress.unlocks[iVar8>>5] >> (iVar8&0x1f)) & 1) != 0)
		)
		{
			iconColor = D230.characterSelect_NeutralColor;

			for (j = 0; j < gGT->numPlyrNextGame; j++)
			{
				if
				(
					((short)i == globalIconPerPlayer[j]) &&

					// if player selected a character
					 (((int)(short)sdata->characterSelectFlags >> (j & 0x1fU) & 1U) != 0)
				)
				{
					iconColor = D230.characterSelect_ChosenColor;
				}
			}

			iVar8 = (int)&D230.ptrTransitionMeta[i];

			#ifdef USE_OXIDE
			if (i == NITROS_OXIDE)
			{
				// use same transition as Fake Crash
				iVar8 = &D230.ptrTransitionMeta[FAKE_CRASH];
			}
			#endif

			DECOMP_RECTMENU_DrawPolyGT4
			(
				gGT->ptrIcons[data.MetaDataCharacters[csm_Active->characterID].iconID],
				((struct TransitionMeta*)iVar8)->currX + csm_Active->posX + 6,
				((struct TransitionMeta*)iVar8)->currY + csm_Active->posY + 4,

				&gGT->backBuffer->primMem,
				gGT->pushBuffer_UI.ptrOT,

				iconColor, iconColor, iconColor, iconColor, TRANS_50_DECAL, FP(1.0)
			);
		}

		csm_Active++;
	}

	// reset
	csm_Active = D230.csm_Active;

	for (i = 0; i < 4; i++)
	{
		data.characterIDs[i] = csm_Active[(int)globalIconPerPlayer[i]].characterID;
	}

	for (i = 0; i < gGT->numPlyrNextGame; i++)
	{
		j = i;
		playerIcon = globalIconPerPlayer[j];
		csm_Active = &D230.csm_Active[playerIcon];

		// if player has not selected a character
		if (((int)(short)sdata->characterSelectFlags >> j & 1U) == 0)
		{
			DECOMP_MM_Characters_AnimateColors
			(
				(unsigned char*)&colorRGBA, j,

				// flags of which characters are selected
				(int)(short)(sdata->characterSelectFlags & (u_short)(1 << j))
			);

			colorRGBA[0] = (u_char)((int)((u_int)colorRGBA[0] << 2) / 5);
			colorRGBA[1] = (u_char)((int)((u_int)colorRGBA[1] << 2) / 5);
			colorRGBA[2] = (u_char)((int)((u_int)colorRGBA[2] << 2) / 5);

			iVar8 = (int)&D230.ptrTransitionMeta[playerIcon];

			#ifdef USE_OXIDE
			if (playerIcon == NITROS_OXIDE)
			{
				// use same transition as Fake Crash
				iVar8 = &D230.ptrTransitionMeta[FAKE_CRASH];
			}
			#endif

			r->x = ((struct TransitionMeta*)iVar8)->currX + csm_Active->posX + 3;
			r->y = ((struct TransitionMeta*)iVar8)->currY + csm_Active->posY + 2;
			r->w = 0x2e;
			r->h = 0x1d;

			Color color = *(Color *)&colorRGBA;
			// this draws the flashing blue square that appears when you highlight a character in the character select screen
			DECOMP_CTR_Box_DrawSolidBox(r, color, ot);
		}
		if
		(
			(D230.timerPerPlayer[j] == 0) &&
			(D230.characterSelect_charIDs_curr[j] == data.characterIDs[j])
		)
		{
			// get number of players
			numPlyrNextGame = gGT->numPlyrNextGame;

			// if number of players is 1 or 2
			fontType = FONT_CREDITS;

			// if number of players is 3 or 4
			if (numPlyrNextGame >= 3) fontType = FONT_SMALL;

			iVar8 = (int)&D230.ptrTransitionMeta[j + 0x10];
			sVar10 = ((struct TransitionMeta*)iVar8)->currY + D230.characterSelect_ptrWindowXY[j*2+1];
			sVar6 = (short)((((u_int)(numPlyrNextGame < 3) ^ 1) << 0x12) >> 0x10);

			if ((numPlyrNextGame == 4) && (j > 1)) sVar6 = sVar10 + sVar6 - 6;
			else							  sVar6 = sVar10 + D230.textPos + sVar6;

			// draw string
			DECOMP_DecalFont_DrawLine
			(
				sdata->lngStrings[data.MetaDataCharacters[csm_Active->characterID].name_LNG_long],
				(int)((struct TransitionMeta*)iVar8)->currX + D230.characterSelect_ptrWindowXY[j * 2] + (int)((u_int)D230.characterSelect_sizeX >> 1),
				(int)sVar6, fontType, (JUSTIFY_CENTER | ORANGE)
			);
		}

		// spin the character
		D230.characterSelect_angle[i] += FPS_HALF(0x40);
	}

	// reset
	csm_Active = D230.csm_Active;

	// loop through all icons
	for (i = 0; i < NUM_ICONS; i++)
	{
		iVar8 = csm_Active[i].unlockFlags;

		if
		(
			// If Icon is unlocked (from array of icons)
			((short)csm_Active[i].unlockFlags == -1) ||

			// if character is unlocked
			// from 4-byte variable that handles all rewards
			// also the variable written by cheats
			((sdata->gameProgress.unlocks[iVar8 >> 5] >> (iVar8 & 0x1fU) & 1) != 0)
		)
		{
			iVar8 = (int)&D230.ptrTransitionMeta[i];

			#ifdef USE_OXIDE
			if (i == NITROS_OXIDE)
			{
				// use same transition as Fake Crash
				iVar8 = &D230.ptrTransitionMeta[FAKE_CRASH];
			}
			#endif

			r->x = ((struct TransitionMeta*)iVar8)->currX + csm_Active[i].posX;
			r->y = ((struct TransitionMeta*)iVar8)->currY + csm_Active[i].posY;
			r->w = 0x34;
			r->h = 0x21;

			// Draw 2D Menu rectangle background
			DECOMP_RECTMENU_DrawInnerRect(r, 0, ot);
		}
	}

		psVar22 = D230.characterSelect_ptrWindowXY;

		for (i = 0; i < gGT->numPlyrNextGame; i++)
		{
			j = i;
			iVar8 = (int)&D230.ptrTransitionMeta[j];

			// store window width and height in one 4-byte variable
			r->x = *(short *)(iVar8 + 0xa6) + *psVar22;
			r->y = *(short *)(iVar8 + 0xa8) + psVar22[1];
			r->w = D230.characterSelect_sizeX;
			r->h = D230.characterSelect_sizeY;

			DECOMP_MM_Characters_AnimateColors
			(
				(unsigned char*)&colorRGBA, j,

				// flags of which characters are selected
				((int)(short)sdata->characterSelectFlags >> j ^ 1U) & 1
			);

			color = *(Color *) &colorRGBA;
			DECOMP_RECTMENU_DrawOuterRect_HighLevel(r, color, 0, ot);

			// if player selected a character
			if (((int)(short)sdata->characterSelectFlags >> j & 1U) != 0)
			{
				r58.x = r->x;
				r58.y = r->y;
				r58.w = r->w;
				r58.h = r->h;

				for (iVar8 = 0; iVar8 < 2; iVar8++)
				{
					r58.x += 3;
					r58.y += 2;
					r58.w -= 6;
					r58.h -= 4;

					colorRGBA[0] = (u_char)((int)((u_int)colorRGBA[0] << 2) / 5);
					colorRGBA[1] = (u_char)((int)((u_int)colorRGBA[1] << 2) / 5);
					colorRGBA[2] = (u_char)((int)((u_int)colorRGBA[2] << 2) / 5);

					color = *(Color *) &colorRGBA;
					DECOMP_RECTMENU_DrawOuterRect_HighLevel(&r58, color, 0, ot);
				}
			}
			psVar22 = psVar22 + 2;

			// Draw 2D Menu rectangle background
			DECOMP_RECTMENU_DrawInnerRect(r, 9, &ot[3]);

			// not screen-space anymore,
			// this is viewport-space
			r->x = 0;
			r->y = 0;

			DECOMP_RECTMENU_DrawRwdBlueRect
			(
				r, &D230.characterSelect_BlueRectColors[0],
				&gGT->pushBuffer[i].ptrOT[0x3ff], &gGT->backBuffer->primMem
			);
		}
	return;
}
