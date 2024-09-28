


#include "mrcPrecompiled.h"

#include "mrcSpriteAnimation.h"


#include "mrcTextureData.h"

namespace mrc
{
	static const real sRealCompensate = real(0.0001); 



	void SpriteAnimOneInterval::setData( const TextureData* pTextureData)
	{
		mData = pTextureData;
		mStaticFrame = mData->getAnimTime(0);
		mrcASSERT(mStaticFrame > 0);
		mCurrentFrame = 0;
		mTime = 0;
		mPlay = true;
		mLoop = true;
		mSpeed = 1;
	}

	void SpriteAnimOneInterval::update( real elapsedTime )
	{
		if(!mPlay)
			return;

		if( mData->getFrameTotalCount() == 1 )
		{
			mCurrentFrame = 0;
			return;
		}


		elapsedTime *= mSpeed;
		mTime += elapsedTime;

		real totalTime = mData->getFrameTotalCount() * mStaticFrame;
		if( mTime > totalTime && !mLoop )
		{
			mPlay = false;
			reset(mData->getFrameTotalCount()-1);
			return;
		}

		while( mTime > totalTime )	{ mTime -= totalTime; }
		int16 curFrame = int16(mTime / mStaticFrame);
		mCurrentFrame =  curFrame %	mData->getFrameTotalCount();
		mrcASSERT(mCurrentFrame >= 0);
	}

	int16 SpriteAnimOneInterval::getCurrentFrame()
	{
		return mCurrentFrame;
	}

	void SpriteAnimOneInterval::reset( int16 frame )
	{
		mrcASSERT( frame < mData->getFrameTotalCount() );
		// sRealCompensate은 혹시 모를 오차를 대비해서
		mTime = mStaticFrame * frame + sRealCompensate;
		mCurrentFrame = frame;
	}

	//////////////////////////////////////////////////////////////////////////
	void SpriteAnimMultiInterval::setData( const TextureData* pTextureData)
	{
		mData = pTextureData;
		mTime = 0;
		mCurrentFrame = 0;
		mPlay = true;
		mLoop = true;
		mSpeed = 1;

		mFrameVec.clear();
		const int16 aniTotalCount = mData->getFrameTotalCount();
		mFrameVec.reserve(aniTotalCount);
		real accumulation = 0;
		for (int16 i=0; i < aniTotalCount; ++i) 
		{
			accumulation += mData->getAnimTime(i);
			mFrameVec.push_back(accumulation);
		}

		mrcASSERT(mFrameVec.size() > 0);
	}

	void SpriteAnimMultiInterval::update( real elapsedTime )
	{
		if(!mPlay)
			return;

		elapsedTime *= mSpeed;
		mTime += elapsedTime;
		real lastTime = mFrameVec[mFrameVec.size()-1];

		if( mTime > lastTime && !mLoop )
		{
			mPlay = false;
			reset(mData->getFrameTotalCount()-1);
			return;
		}

		while( mTime > lastTime )
		{
			mTime -= lastTime;
			// 이미 끝을 돌았으므로 0부터 검색해야함
			mCurrentFrame = 0;
		}

		while( mTime >= mFrameVec[  mCurrentFrame % (int16)mFrameVec.size() ] )
		{ ++mCurrentFrame; }

		mrcASSERT(mCurrentFrame >= 0);
	}

	int16 SpriteAnimMultiInterval::getCurrentFrame()
	{
		return mCurrentFrame;
	}

	void SpriteAnimMultiInterval::reset( int16 frame )
	{
		mrcASSERT(frame < (int16)mFrameVec.size());
		mCurrentFrame = frame;
		if( frame == 0)
		{
			mTime = 0;
		}
		else
		{
			// 마지막 전 프레임
			mTime = mFrameVec[frame-1];
			mTime += sRealCompensate;
		}

	}

	
}







