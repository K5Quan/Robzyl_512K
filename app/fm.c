/* Original work Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 *   zylka(c)
 * https://k5.2je.eu/index.php?topic=119
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


#include "app/action.h"
#include "app/fm.h"
#include "app/generic.h"

#include "driver/bk1080.h"
#include "misc.h"
#include "settings.h"
#include "ui/ui.h"
#include "driver/gpio.h"
#include "driver/si473x.h"
#include <string.h>
#include "external/printf/printf.h"
//#if defined ENABLE_4732
//#endif


const uint16_t FM_RADIO_MAX_FREQ = 10800; // 108  Mhz
const uint16_t FM_RADIO_MIN_FREQ = 8750;  // was 87.5 Mhz

bool              gFmRadioMode;
uint8_t           gFmRadioCountdown_500ms;
volatile uint16_t gFmPlayCountdown_10ms;
volatile int8_t   gFM_ScanState;
uint16_t          gFM_RestoreCountdown_10ms;
//#if defined(ENABLE_4732)
uint16_t          gFmCurrentFrequency;
// FM settings variables
uint8_t           gFmGain;
int8_t            gFmVolume;
bool              gAgcEnabled;
uint8_t           gFmMuteState;
//#endif

void FM_TurnOff(void)
{
	gFmRadioMode              = false;
	gFM_ScanState             = FM_SCAN_OFF;
	gFM_RestoreCountdown_10ms = 0;

	AUDIO_AudioPathOff();

	gEnableSpeaker = false;

#if defined ENABLE_4732
		// Power down the radio chip
	SI47XX_PowerDown();
#endif
	BK1080_Init(0, false);
	

	gUpdateStatus  = true;
}

static void Key_EXIT()
{
	ACTION_FM();
	SETTINGS_SaveVfoIndices();
	return;
}

void FM_ProcessKeys(KEY_Code_t Key, bool bKeyPressed, bool bKeyHeld)
{
	// Handle frequency change on initial press and hold
	if (bKeyPressed || bKeyHeld) {
		//gFmCurrentFrequency=gFmCurrentFrequency*10;
		switch (Key) {
			case KEY_UP:
				if (gFmCurrentFrequency < FM_RADIO_MAX_FREQ) {
					gFmCurrentFrequency += 10; // 0.1 MHz
				} else {
					gFmCurrentFrequency = FM_RADIO_MIN_FREQ;
				}
#if defined ENABLE_4732
				SI47XX_SetFreq(gFmCurrentFrequency);
#else
	BK1080_SetFrequency(gFmCurrentFrequency/10);
#endif			
				gEeprom.FM_FrequencyPlaying = gFmCurrentFrequency;
				return; // Exit after handling
			case KEY_DOWN:
				if (gFmCurrentFrequency > FM_RADIO_MIN_FREQ) {
					gFmCurrentFrequency -= 10; // 0.1 MHz
				} else {
					gFmCurrentFrequency = FM_RADIO_MAX_FREQ;
				}

#if defined ENABLE_4732
				SI47XX_SetFreq(gFmCurrentFrequency);
#else
	BK1080_SetFrequency(gFmCurrentFrequency/10);
#endif
				gEeprom.FM_FrequencyPlaying = gFmCurrentFrequency;
				return; // Exit after handling
			default:
				break;
		}
	}
			
			
				//gEeprom.FM_FrequencyPlaying = gFmCurrentFrequency ;
	// save
	//gRequestSaveSettings = true;
			
	//return; // Exit after handling
							
	//-BK1080_TuneNext(direction);
	//--gEeprom.FM_FrequencyPlaying = BK1080_GetFrequency();
	
		// Handle other keys on release (short press)
	if (!bKeyPressed && !bKeyHeld) {
		switch (Key)
		{
			case KEY_1: // Scan Up
				if (gFM_ScanState == FM_SCAN_OFF) {

gFM_ScanState = FM_SCAN_UP;
#if defined ENABLE_4732
SI47XX_Seek(true, true);				
#else
BK1080_TuneNext(true);
gEeprom.FM_FrequencyPlaying = BK1080_GetFrequency()/10;	
#endif
					
				}
				break;
			case KEY_7: // Scan Down
				if (gFM_ScanState == FM_SCAN_OFF) {

					gFM_ScanState = FM_SCAN_DOWN;
#if defined ENABLE_4732
SI47XX_Seek(false, true);				
#else
BK1080_TuneNext(false);
gEeprom.FM_FrequencyPlaying = BK1080_GetFrequency()/10;	
#endif
					
				}
				break;
			case KEY_0: // Toggle AGC
				gAgcEnabled = !gAgcEnabled;
#if defined ENABLE_4732
				// When toggling, re-apply gain setting. AGCDIS is 1 to disable.
				SI47XX_SetAutomaticGainControl(!gAgcEnabled, gFmGain);
#endif
				break;
			case KEY_2: // Volume Up
				if (gFmVolume < 63) {
					gFmVolume++;
#if defined ENABLE_4732
					setVolume(gFmVolume);
#endif
				}
				break;
			case KEY_8: // Volume Down
				if (gFmVolume > 0) {
					gFmVolume--;
#if defined ENABLE_4732
					setVolume(gFmVolume);
#endif
				}
				break;
			case KEY_3: // Manual Gain Up
				if (!gAgcEnabled && gFmGain < 27) { // Gain is manually adjustable only if AGC is OFF. Max 27 for FM.
					gFmGain++;
#if defined ENABLE_4732
					SI47XX_SetAutomaticGainControl(true, gFmGain); // AGC Disabled
#endif
			
				}
				break;
			case KEY_9: // Manual Gain Down
				if (!gAgcEnabled && gFmGain > 0) { // Gain is manually adjustable only if AGC is OFF
					gFmGain--;
#if defined ENABLE_4732
					SI47XX_SetAutomaticGainControl(true, gFmGain); // AGC Disabled
#endif
				}
				break;
			case KEY_5: // Cycle Mute State
#if defined ENABLE_4732
				gFmMuteState = (gFmMuteState + 1) % 4; // Cycle 0, 1, 2, 3
				sendProperty(PROP_RX_HARD_MUTE, gFmMuteState);				
#else
	gFmMuteState = (gFmMuteState + 1) % 2; // Cycle 0, 1, 
				BK1080_Mute(gFmMuteState);	
#endif
				break;
			case KEY_EXIT:
				Key_EXIT();
				break;
			default:
				
				break;
		}
	}


}

void FM_CheckScan(void)
{
	// Called periodically to check for seek completion
	if (gFM_ScanState != FM_SCAN_OFF) {
		bool valid;
#if defined ENABLE_4732
		uint16_t new_freq = SI47XX_getFrequency(&valid);
#else
		uint16_t new_freq = BK1080_GetFrequency()*10;
		valid = true;	
#endif
		if (valid) {
			// Seek/scan is complete, a new station was found
			gFM_ScanState = FM_SCAN_OFF;
			gFmCurrentFrequency = new_freq;
			gEeprom.FM_FrequencyPlaying = new_freq;
			
			// Force re-tune to the new frequency to stabilize the chip
			// This fixes the desynchronization issue after scanning.
#if defined ENABLE_4732
			SI47XX_SetFreq(gFmCurrentFrequency);
#endif

			// Force a GUI redraw
			gUpdateDisplay = true;
		}
	}
}

void FM_Start(void)
{
	gFmRadioMode              = true;
	gFM_ScanState             = FM_SCAN_OFF;
	gFM_RestoreCountdown_10ms = 0;

	gFmPlayCountdown_10ms = 0;
	gScheduleFM           = false;
	gAskToSave            = false;
	
		// Initialize FM settings
	gFmGain                   = 4;   // !max gain
	gFmVolume                 = 55;   // Set start volume to 55
	gAgcEnabled               = true; // AGC enabled by default
	gFmMuteState              = 0;    // Mute off by default



	
#if defined ENABLE_4732
		// Power up the SI473x chip in FM mode
	SI47XX_PowerUp();
	
	// Set initial volume
	setVolume(gFmVolume);

	// Set De-emphasis to 50us for Europe
	sendProperty(PROP_FM_DEEMPHASIS, 0x0001);

	// Enable Automatic Gain Control for better reception
	SI47XX_SetAutomaticGainControl(!gAgcEnabled, gFmGain); // Set initial AGC state	
	AUDIO_AudioPathOn();	
	gFmCurrentFrequency = gEeprom.FM_FrequencyPlaying;
	SI47XX_SetFreq(gFmCurrentFrequency);
	if(gFmCurrentFrequency>FM_RADIO_MAX_FREQ) gFmCurrentFrequency=gFmCurrentFrequency*10;
#else
	BK1080_Init(gEeprom.FM_FrequencyPlaying/10, true);
	AUDIO_AudioPathOn();
	gFmCurrentFrequency = gEeprom.FM_FrequencyPlaying;
#endif

	//gEeprom.FM_FrequencyPlaying=gFmCurrentFrequency*10;
	// let the user see DW is not active
	gDualWatchActive     = false;

	gEnableSpeaker       = true;
	gUpdateStatus        = true;
	GUI_SelectNextDisplay(DISPLAY_FM);

}

