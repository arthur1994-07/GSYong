


#ifndef _mrcSpriteAnimation_h_ 
#define _mrcSpriteAnimation_h_

#include "mrcConfig.h"
#include "mrcSprite.h"

namespace mrc
{
	class TextureData;
	
	class SpriteAnimOneInterval : public ISpriteAnimation
	{
		virtual void  setData(const TextureData* pTextureData);
		virtual void  update(real elapsedTime);
		virtual int16 getCurrentFrame();
		virtual void  reset(int16 frame);
		virtual void  pause() { mPlay = false; }
		virtual void  play()  { mPlay = true; }

		virtual void  setSpeed(real speed) { mSpeed = speed; }
		virtual void  setLoop(bool isLoop) { mLoop = isLoop; }
	private:
		const TextureData* mData;
		real  mStaticFrame;
		real  mTime;
		int16 mCurrentFrame;
		bool  mPlay;
		bool  mLoop;
		real  mSpeed;
	};

	class SpriteAnimMultiInterval : public ISpriteAnimation
	{
		virtual void  setData(const TextureData* pTextureData);
		virtual void  update(real elapsedTime);
		virtual int16 getCurrentFrame();
		virtual void  reset(int16 frame);
		virtual void  pause() { mPlay = false; }
		virtual void  play()  { mPlay = true; }
		virtual void  setSpeed(real speed) { mSpeed = speed; }
		virtual void  setLoop(bool isLoop) { mLoop = isLoop; }

	private:
		const TextureData* mData;
		real  mTime;
		int16 mCurrentFrame;
		vector<real> mFrameVec;
		bool  mPlay;
		bool  mLoop;
		real  mSpeed;
	};

	class SpriteAnimNo : public ISpriteAnimation
	{
		virtual void  setData(const TextureData* ) {}
		virtual void  update(real ) {}
		virtual int16 getCurrentFrame() { return 0; }
		virtual void  reset(int16 ) {}
		virtual void  pause() {}
		virtual void  play()  {}
		virtual void  setSpeed(real ) {  }
		virtual void  setLoop(bool ) {}
	};


	
}




#endif