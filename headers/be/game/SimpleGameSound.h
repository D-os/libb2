/*	SimpleGameSound.h	*/
/*******************************************************************************
/
/	File:			SimpleGameSound.h
/
/   Description:    BSimpleGameSound is a class for sound effects that are 
/					short, and consists of non-changing samples in memory.
/
/	Copyright 1999, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

#if !defined(_SIMPLE_GAME_SOUND_H)
#define _SIMPLE_GAME_SOUND_H

#include <GameSound.h>

class BSimpleGameSound : public BGameSound
{
public:
							BSimpleGameSound(
									const entry_ref * inFile,
									BGameSoundDevice * device = NULL);
							BSimpleGameSound(
									const char * inFile,
									BGameSoundDevice * device = NULL);
							BSimpleGameSound(
									const void * inData,
									size_t inFrameCount,
									const gs_audio_format * format,
									BGameSoundDevice * device = NULL);

							BSimpleGameSound(
									const BSimpleGameSound & other);

virtual						~BSimpleGameSound();

virtual	BGameSound *		Clone() const;

virtual	status_t Perform(int32 selector, void * data);

		status_t			SetIsLooping(
									bool looping);				//	whether to go back to beginning
		bool				IsLooping() const;

private:

	/* leave these declarations private unless you plan on actually implementing and using them. */
	BSimpleGameSound();
	BSimpleGameSound& operator=(const BSimpleGameSound&);

	/* fbc data and virtuals */

	uint32 _reserved_BSimpleGameSound_[12];

virtual	status_t _Reserved_BSimpleGameSound_0(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_1(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_2(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_3(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_4(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_5(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_6(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_7(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_8(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_9(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_10(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_11(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_12(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_13(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_14(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_15(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_16(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_17(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_18(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_19(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_20(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_21(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_22(int32 arg, ...);
virtual	status_t _Reserved_BSimpleGameSound_23(int32 arg, ...);

};

#endif	//	_SIMPLE_GAME_SOUND_H
