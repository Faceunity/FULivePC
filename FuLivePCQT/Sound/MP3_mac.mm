#include "MP3_mac.h"
#include <AVFoundation/AVFoundation.h>
Mp3::Mp3()
{
	_audioPlayer = NULL;
	ready = false;
	duration = 0;
}

Mp3::~Mp3()
{
	Cleanup();
}

void Mp3::Cleanup()
{
	if (_audioPlayer) {
		Stop();
		free(_audioPlayer);
		_audioPlayer = NULL;
	}
	ready = false;
}

bool Mp3::Load(std::string mp3Path)
{
	Cleanup();
	ready = false;
	NSError * generateAudioPlayerError;
	_audioPlayer = [[AVAudioPlayer alloc]initWithContentsOfURL:[NSURL fileURLWithPath:
																[NSString stringWithUTF8String:mp3Path.c_str()]] error:&generateAudioPlayerError];
	if (generateAudioPlayerError) {
		NSLog(@"创建AVAudioPlayer对象失败----error::%@",generateAudioPlayerError);
		ready = false;
	}else{
		((AVAudioPlayer*)_audioPlayer).numberOfLoops = -1; // loop
		((AVAudioPlayer*)_audioPlayer).enableRate = YES;
		ready = [(AVAudioPlayer*)_audioPlayer prepareToPlay];
	}
	return ready;
}

bool Mp3::Play()
{
	((AVAudioPlayer*)_audioPlayer).currentTime = 0;
    return [(AVAudioPlayer*)_audioPlayer play];
	
}

bool Mp3::CirculationPlayCheck()
{

	return false;
}

bool Mp3::Pause()
{

	[(AVAudioPlayer*)_audioPlayer pause];
	return true;
}
bool Mp3::isPlaying()
{
	return [(AVAudioPlayer*)_audioPlayer isPlaying];
}

bool Mp3::Stop()
{

	[(AVAudioPlayer*)_audioPlayer stop];
	return true;
}

bool Mp3::WaitForCompletion(long msTimeout, long* EvCode)
{
	return false;
}

bool Mp3::SetVolume(long vol)
{

	((AVAudioPlayer*)_audioPlayer).volume = vol;
	return true;
}

long Mp3::GetVolume()
{

	return ((AVAudioPlayer*)_audioPlayer).volume;
}

__int64 Mp3::GetDuration()
{
	return ((AVAudioPlayer*)_audioPlayer).duration;
}

__int64 Mp3::GetCurrentPosition()
{
	return ((AVAudioPlayer*)_audioPlayer).currentTime * 10000000;
}

bool Mp3::SetPositions(__int64* pCurrent, __int64* pStop, bool bAbsolutePositioning)
{
	return false;
}
