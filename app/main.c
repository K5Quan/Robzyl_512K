/* Original work Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Modified work Copyright 2024 kamilsss655
 * https://github.com/kamilsss655
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>

#include "app/action.h"
#include "app/app.h"
#include "app/common.h"
#include "app/fm.h"
#include "app/generic.h"
#include "app/main.h"
#include "app/scanner.h"
#include "app/spectrum.h"

#include "board.h"
#include "driver/bk4819.h"
#include "frequencies.h"
#include "misc.h"
#include "radio.h"
#include "settings.h"
#include "ui/inputbox.h"
#include "ui/ui.h"
#include <stdlib.h>
#include "driver/backlight.h"  // правильный путь

// Флаг для постоянной подсветки (toggle F+8)
bool gBacklightAlwaysOn = false;
uint8_t gSavedBacklightLevel = 10;  // начальное значение (максимум)


static void MAIN_Key_STAR(bool closecall)
{
	if (gCurrentFunction == FUNCTION_TRANSMIT) return;
	gWasFKeyPressed          = false;
	SCANNER_Start(closecall);
	gRequestDisplayScreen = DISPLAY_SCANNER;
}

static void processFKeyFunction(const KEY_Code_t Key, const bool beep)
{
	if (gScreenToDisplay == DISPLAY_MENU)
	{
		return;
	}
	
	switch (Key)
	{
		case KEY_0:
			ACTION_FM();
			break;

		case KEY_1:
			if (!IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE)) {
				gWasFKeyPressed = false;
				   

#ifdef ENABLE_COPY_CHAN_TO_VFO
				if (gEeprom.VFO_OPEN && !gCssBackgroundScan)
				{

					if (IS_MR_CHANNEL(gEeprom.ScreenChannel))
					{	// copy Channel to VFO, then swap to the VFO

						const uint16_t Channel = FREQ_CHANNEL_FIRST + gEeprom.VfoInfo.Band;

						gEeprom.ScreenChannel = Channel;
						gEeprom.VfoInfo.CHANNEL_SAVE = Channel;

						RADIO_SelectVfos();
						RADIO_ApplyTxOffset(gTxVfo);
						RADIO_ConfigureSquelchAndOutputPower(gTxVfo);
						RADIO_SetupRegisters(true);
						
						gRequestSaveChannel = 1;
						gUpdateDisplay = true;
					}
				}
				
#endif
				return;
			}

			if(gTxVfo->pRX->Frequency < 100000000) { //Robby69 directly go to 1Ghz
					gTxVfo->Band = 7;
					gTxVfo->pRX->Frequency = 100000000;
					return;
			}
			gRequestSaveVFO            = true;
			gVfoConfigureMode          = VFO_CONFIGURE_RELOAD;
			gRequestDisplayScreen      = DISPLAY_MAIN;
			break;

		case KEY_2:
			if (++gTxVfo->SCANLIST > 15) gTxVfo->SCANLIST = 0;
			SETTINGS_UpdateChannel(gTxVfo->CHANNEL_SAVE, gTxVfo, true);
			gVfoConfigureMode = VFO_CONFIGURE;
			gFlagResetVfos    = true;
			break;

		case KEY_3:
			COMMON_SwitchVFOMode();
			break;
		case KEY_4:
			if (beep) APP_RunSpectrum(1); //Channel scan
			break;
		case KEY_5:
			if (beep) APP_RunSpectrum(4); //basic spectrum}
			break;

		case KEY_6:
			if (beep) APP_RunSpectrum(2); // Band scan
			else {
			gTxVfo->FrequencyReverse = gTxVfo->FrequencyReverse == false;
			gRequestSaveChannel = 1;
			}
			break;
		
		case KEY_7:
			ACTION_SwitchDemodul();
			break;

			//****************УПРАВЛЕНИЕ ПОДСВЕТКОЙ F8 */
			case KEY_8:
			if (gWasFKeyPressed) {
				// F+8 — toggle "подсветка всегда включена"
				gBacklightAlwaysOn = !gBacklightAlwaysOn;

				if (gBacklightAlwaysOn) {
					gBacklightCountdown = 0;
					backlightOn = true;
				} else {
					backlightOn = true;

					if (gEeprom.BACKLIGHT_TIME == 7) {
						gBacklightCountdown = 0;
					} else {
						switch (gEeprom.BACKLIGHT_TIME)
						{
							case 1: gBacklightCountdown = 10;  break;
							case 2: gBacklightCountdown = 20;  break;
							case 3: gBacklightCountdown = 40;  break;
							case 4: gBacklightCountdown = 120; break;
							case 5: gBacklightCountdown = 240; break;
							case 6: gBacklightCountdown = 480; break;
							default: gBacklightCountdown = 0; break;
						}
					}
				}

				gUpdateDisplay = true;
			} else {
				ACTION_Power();  // длинное нажатие 8 — мощность
			}
			break;

			
		case KEY_9:
			gTxVfo->CHANNEL_BANDWIDTH =
				ACTION_NextBandwidth(gTxVfo->CHANNEL_BANDWIDTH, gTxVfo->Modulation != MODULATION_AM, 0);
			break;

		default:
			gWasFKeyPressed = false;
			break;
	}
}

//ФУНКЦИИ КНОПОК 4
static void MAIN_Key_DIGITS(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	if (bKeyHeld)
	{
		if (bKeyPressed)
		{
			if (gScreenToDisplay == DISPLAY_MAIN)
			{
				if (gInputBoxIndex > 0)
				{
					gInputBoxIndex = 0;
					gRequestDisplayScreen = DISPLAY_MAIN;
				}
				gWasFKeyPressed = false;
				processFKeyFunction(Key, false);  // длинное F+цифра
			}
		}
		return;
	}

	if (bKeyPressed)
	{
		return;
	}

	// === F+8 — toggle "подсветка всегда включена" ===
	if (gWasFKeyPressed && Key == KEY_8) {
		gBacklightAlwaysOn = !gBacklightAlwaysOn;

		if (gBacklightAlwaysOn) {
			gBacklightCountdown = 0;
			backlightOn = true;
		} else {
			backlightOn = true;

			if (gEeprom.BACKLIGHT_TIME == 7) {
				gBacklightCountdown = 0;
			} else {
				switch (gEeprom.BACKLIGHT_TIME)
				{
					case 1: gBacklightCountdown = 10;  break;
					case 2: gBacklightCountdown = 20;  break;
					case 3: gBacklightCountdown = 40;  break;
					case 4: gBacklightCountdown = 120; break;
					case 5: gBacklightCountdown = 240; break;
					case 6: gBacklightCountdown = 480; break;
					default: gBacklightCountdown = 0; break;
				}
			}
		}

		gUpdateDisplay = true;
		gWasFKeyPressed = false;
		return;
	}

	// === Обычный ввод цифр (короткое нажатие, включая 8) === 
	if (!gWasFKeyPressed)
	{
		bool isFreqMode = IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE);

		if (!isFreqMode)
			gKeyInputCountdown = key_input_timeout_500ms;

		INPUTBOX_Append(Key);
		gRequestDisplayScreen = DISPLAY_MAIN;

		if (!isFreqMode)
		{
			if (gInputBoxIndex == 1 || gInputBoxIndex == 2)
				return;

			if (gInputBoxIndex == 3)
			{
				uint16_t Channel = ((gInputBox[0] * 100) + (gInputBox[1] * 10) + gInputBox[2]) - 1;

				if (!RADIO_CheckValidChannel(Channel, false, 0)) 
				{
					gInputBoxIndex = 0;
					return;
				}

				gEeprom.MrChannel     = Channel;
				gEeprom.ScreenChannel = Channel;
				gRequestSaveVFO       = true;
				gVfoConfigureMode     = VFO_CONFIGURE_RELOAD;
				gInputBoxIndex        = 0;
				return;
			}

			gInputBoxIndex = 0;

			uint16_t Channel = ((gInputBox[0] * 100) + (gInputBox[1] * 10) + gInputBox[2]) - 1;

			if (!RADIO_CheckValidChannel(Channel, false,0)) return;

			gEeprom.MrChannel     = Channel;
			gEeprom.ScreenChannel = Channel;
			gRequestSaveVFO       = true;
			gVfoConfigureMode     = VFO_CONFIGURE_RELOAD;

			return;
		}

		if (IS_FREQ_CHANNEL(gTxVfo->CHANNEL_SAVE))
		{
			uint32_t Frequency;
			bool isGigaF = gTxVfo->pRX->Frequency >= 100000000;
			if (gInputBoxIndex < 6 + isGigaF) return;

			gInputBoxIndex = 0;
			Frequency = StrToUL(INPUTBOX_GetAscii()) * 100;

			SETTINGS_SetVfoFrequency(Frequency);
			
			gRequestSaveChannel = 1;
			return;
		}

		gRequestDisplayScreen = DISPLAY_MAIN;
		return;
	}

	gWasFKeyPressed = false;
	processFKeyFunction(Key, true);
}

static void MAIN_Key_EXIT(bool bKeyPressed, bool bKeyHeld)
{
	(void)bKeyPressed;
	(void)bKeyHeld;

	if (!bKeyHeld && bKeyPressed)
	{	// exit key pressed

		if (!gFmRadioMode)
		{

				if (gInputBoxIndex == 0)
					return;
				gInputBox[--gInputBoxIndex] = 10;

				gKeyInputCountdown = key_input_timeout_500ms;

			gRequestDisplayScreen = DISPLAY_MAIN;
			return;
		}
		ACTION_FM();
		return;
	}

}

static void MAIN_Key_MENU()
{
		const bool bFlag = (gInputBoxIndex == 0);
		gInputBoxIndex   = 0;

		if (bFlag)
		{
			gFlagRefreshSetting = true;
			gRequestDisplayScreen = DISPLAY_MENU;
		}
		else
		{
			gRequestDisplayScreen = DISPLAY_MAIN;
		}
}

//УПРАВЛЕНИЕ SQL 
static void MAIN_Key_UP_DOWN(bool bKeyPressed, bool bKeyHeld, int8_t Direction)
{
	(void)bKeyPressed;
	(void)bKeyHeld;
	(void)Direction;

	uint16_t Channel = gEeprom.ScreenChannel;
	if (gInputBoxIndex > 0) return;

	if (IS_FREQ_CHANNEL(Channel))  // VFO-режим
	{
		static uint8_t repeat_count = 0;

		if (bKeyHeld)
		{
			// Длинное нажатие — запускаем прокрутку после задержки
			repeat_count++;
			if (repeat_count < 6) return;  // задержка перед началом прокрутки (~0.4 сек)
			if (repeat_count > 20) repeat_count = 8;  // ускорение после долгого удержания
		}
		else
		{
			// Кнопка отпущена — короткое нажатие
			if (repeat_count == 0)
			{
				// короткое нажатие — только один шаг
				repeat_count = 1;  // делаем один шаг
			}
			else
			{
				// отпустили после удержания — ничего не делаем (прокрутка уже шла)
				repeat_count = 0;
				return;
			}
		}

		// делаем шаг
		uint32_t frequency = APP_SetFrequencyByStep(gTxVfo, Direction);
		if (RX_freq_check(frequency) == 0xFF) return;

		gTxVfo->freq_config_RX.Frequency = frequency;
		BK4819_SetFrequency(frequency);
		gRequestSaveChannel = 1;
		gUpdateDisplay = true;

		return;
	}
	else  // MR-режим
	{
		uint16_t Next = RADIO_FindNextChannel(Channel + Direction, Direction, false, 0);
		if (Next == 0xFFFF || Next == Channel) return;

		gEeprom.MrChannel     = Next;
		gEeprom.ScreenChannel = Next;
		gRequestSaveVFO       = true;
		gVfoConfigureMode     = VFO_CONFIGURE_RELOAD;
		gPttWasReleased       = true;
		gUpdateDisplay = true;
	}
}
//END SQL	

void MAIN_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	if (Key == KEY_F)
	{
		if (bKeyPressed && !bKeyHeld)
		{
			gWasFKeyPressed = true;
			gKeyInputCountdown = 0;
		}
		return;
	}

	// Включаем подсветку при любом коротком нажатии кнопки (кроме F)
	if (bKeyPressed && !bKeyHeld && Key != KEY_F)
	{
		BACKLIGHT_TurnOn();
	}

	if (gFmRadioMode && Key != KEY_PTT && Key != KEY_EXIT)
	{
		if (!bKeyHeld && bKeyPressed)
			return;
	}
	switch (Key)
	{
		case KEY_0:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
			MAIN_Key_DIGITS(Key, bKeyPressed, bKeyHeld);
			break;
		case KEY_MENU:
    if (bKeyHeld)
    {
        // Длинное MENU — твоя функция, например сброс или всегда в главное меню
        gRequestDisplayScreen = DISPLAY_MAIN;
        gInputBoxIndex = 0;
    }
    else
    {
        MAIN_Key_MENU();  // стандартное короткое
    }
    break;

		case KEY_UP:
		case KEY_DOWN:
			if (gWasFKeyPressed && bKeyPressed && !bKeyHeld)
			{
				if (Key == KEY_UP)
				{
					if (gEeprom.SQUELCH_LEVEL < 9)
						gEeprom.SQUELCH_LEVEL++;
				}
				else
				{
					if (Key == KEY_DOWN && gEeprom.SQUELCH_LEVEL > 0)
						gEeprom.SQUELCH_LEVEL--;
				}

				gRequestSaveSettings = true;
				gUpdateDisplay = true;

				RADIO_ConfigureSquelchAndOutputPower(gTxVfo);
				RADIO_ApplySquelch();
			}
			else
			{
				MAIN_Key_UP_DOWN(bKeyPressed, bKeyHeld, (Key == KEY_UP) ? 1 : -1);
			}
			gWasFKeyPressed = false;
			break;

		case KEY_EXIT:
			MAIN_Key_EXIT(bKeyPressed, bKeyHeld);
			break;

		case KEY_STAR:
			if (gWasFKeyPressed) MAIN_Key_STAR(1);
			else MAIN_Key_STAR(0);
			break;

		case KEY_F:
			GENERIC_Key_F(bKeyPressed, bKeyHeld);
			break;

		case KEY_PTT:
			GENERIC_Key_PTT(bKeyPressed);
			break;

		case KEY_SIDE1:
		case KEY_SIDE2:
		case KEY_INVALID:
			break;

		default:
			break;
	}
}