//#pragma once
#ifndef MP3_MAC_H
#define MP3_MAC_H

#include <string>

class Mp3
{
public:
	Mp3();
	~Mp3();

	bool Load(std::string);
	void Cleanup();

	bool Play();
	bool CirculationPlayCheck();
	bool isPlaying();
	bool Pause();
	bool Stop();

	// Poll this function with msTimeout = 0, so that it return immediately.
	// If the mp3 finished playing, WaitForCompletion will return true;
	bool WaitForCompletion(long msTimeout, long* EvCode);

	// -10000 is lowest volume and 0 is highest volume, positive value > 0 will fail
	bool SetVolume(long vol);

	// -10000 is lowest volume and 0 is highest volume
	long GetVolume();

	// Returns the duration in 1/10 millionth of a second,
	// meaning 10,000,000 == 1 second
	// You have to divide the result by 10,000,000 
	// to get the duration in seconds.
	__int64 GetDuration();

	// Returns the current playing position
	// in 1/10 millionth of a second,
	// meaning 10,000,000 == 1 second
	// You have to divide the result by 10,000,000 
	// to get the duration in seconds.
	__int64 GetCurrentPosition();

	// Seek to position with pCurrent and pStop
	// bAbsolutePositioning specifies absolute or relative positioning.
	// If pCurrent and pStop have the same value, the player will seek to the position
	// and stop playing. Note: Even if pCurrent and pStop have the same value,
	// avoid putting the same pointer into both of them, meaning put different
	// pointers with the same dereferenced value.
	bool SetPositions(__int64* pCurrent, __int64* pStop, bool bAbsolutePositioning);

private:
	bool    ready;
	// Duration of the MP3.
	__int64 duration;
	std::string sMp3Path;
	void * _audioPlayer;

};


#endif
