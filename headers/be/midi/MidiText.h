/*******************************************************************************
/
/	File:		MidiText.h
/
/	Description:	MIDI debugging tool.
/
/	Copyright 1993-98, Be Incorporated, All Rights Reserved.
/
*******************************************************************************/

#ifndef _MIDI_TEXT_H
#define _MIDI_TEXT_H

#ifndef _BE_BUILD_H
#include <BeBuild.h>
#endif
#include <Midi.h>
#include <stdio.h>

/*------------------------------------------------------------*/

class BMidiText : public BMidi {
public:
				BMidiText();
virtual			~BMidiText();

virtual	void	NoteOff(uchar channel, 
						uchar note, 
						uchar velocity,
						uint32 time = B_NOW);

virtual	void	NoteOn(uchar channel, 
					   uchar note, 
					   uchar velocity,
			    	   uint32 time = B_NOW);

virtual	void	KeyPressure(uchar channel, 
							uchar note, 
							uchar pressure,
							uint32 time = B_NOW);

virtual	void	ControlChange(uchar channel, 
							  uchar controlNumber,
							  uchar controlValue, 
							  uint32 time = B_NOW);

virtual	void	ProgramChange(uchar channel, 
								uchar programNumber,
							  	uint32 time = B_NOW);

virtual	void	ChannelPressure(uchar channel, 
								uchar pressure, 
								uint32 time = B_NOW);

virtual	void	PitchBend(uchar channel, 
						  uchar lsb, 
						  uchar msb,
			    		  uint32 time = B_NOW);

virtual	void	SystemExclusive(void* data, 
								size_t dataLength, 
								uint32 time = B_NOW);

virtual	void	SystemCommon(uchar statusByte, 
							 uchar data1, 
							 uchar data2,
							 uint32 time = B_NOW);

virtual	void	SystemRealTime(uchar statusByte, uint32 time = B_NOW);

	void	ResetTimer(bool start=false);

private:

virtual	void		_ReservedMidiText1();
virtual	void		_ReservedMidiText2();
virtual	void		_ReservedMidiText3();

virtual	void	Run();
	int32	fStartTime;
	uint32	_reserved[4];
};

/*------------------------------------------------------------*/

#endif
