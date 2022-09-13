#include "MP3_mac.h"
#include <AVFoundation/AVFoundation.h>

struct Mp3::Imp
{
    AVAudioPlayer * _audioPlayer = nullptr;
};

Mp3::Mp3():mImp(new Imp())
{
    mImp->_audioPlayer = NULL;
	ready = false;
	duration = 0;
}

Mp3::~Mp3()
{
	Cleanup();
    if(mImp)
    {
        delete mImp;
        mImp = nullptr;
    }
}

void Mp3::Cleanup()
{
	if (mImp->_audioPlayer) {
		Stop();
		//free(mImp->_audioPlayer);
		mImp->_audioPlayer = NULL;
	}
	ready = false;
}

bool Mp3::Load(std::string mp3Path)
{
	Cleanup();
	ready = false;
	NSError * generateAudioPlayerError;
	mImp->_audioPlayer = [[AVAudioPlayer alloc]initWithContentsOfURL:[NSURL fileURLWithPath:
																[NSString stringWithUTF8String:mp3Path.c_str()]] error:&generateAudioPlayerError];
	if (generateAudioPlayerError) {
		NSLog(@"创建AVAudioPlayer对象失败----error::%@",generateAudioPlayerError);
		ready = false;
	}else{
		((AVAudioPlayer*)mImp->_audioPlayer).numberOfLoops = -1; // loop
		((AVAudioPlayer*)mImp->_audioPlayer).enableRate = YES;
		ready = [(AVAudioPlayer*)mImp->_audioPlayer prepareToPlay];
	}
	return ready;
}

bool Mp3::Play()
{
	((AVAudioPlayer*)mImp->_audioPlayer).currentTime = 0;
    return [(AVAudioPlayer*)mImp->_audioPlayer play];
	
}

bool Mp3::CirculationPlayCheck()
{

	return false;
}

bool Mp3::Pause()
{

	[(AVAudioPlayer*)mImp->_audioPlayer pause];
	return true;
}
bool Mp3::isPlaying()
{
	return [(AVAudioPlayer*)mImp->_audioPlayer isPlaying];
}

bool Mp3::Stop()
{

	[(AVAudioPlayer*)mImp->_audioPlayer stop];
	return true;
}

bool Mp3::WaitForCompletion(long msTimeout, long* EvCode)
{
	return false;
}

bool Mp3::SetVolume(long vol)
{

	((AVAudioPlayer*)mImp->_audioPlayer).volume = vol;
	return true;
}

long Mp3::GetVolume()
{

	return ((AVAudioPlayer*)mImp->_audioPlayer).volume;
}

__int64 Mp3::GetDuration()
{
	return ((AVAudioPlayer*)mImp->_audioPlayer).duration;
}

__int64 Mp3::GetCurrentPosition()
{
	return ((AVAudioPlayer*)mImp->_audioPlayer).currentTime * 10000000;
}

bool Mp3::SetPositions(__int64* pCurrent, __int64* pStop, bool bAbsolutePositioning)
{
	return false;
}
