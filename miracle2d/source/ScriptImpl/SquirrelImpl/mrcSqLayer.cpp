


#include "mrcPrecompiled.h"

#include "mrcSqLayer.h"
#include "../../mrcLayerImpl.h"
#include "../../mrcSpriteDefault.h"
#include "mrcSqUtil.h"
#include "mrcUtil/mrcScriptShape.h"


namespace mrc
{
	class SpriteDefault;

	SqLayer::SqLayer( SP_LayerImpl spLayerImpl )
	{
		SP_Layer spLayer = sharedptr_static_cast<ILayer>(spLayerImpl); 
		mLayerForCheck = spLayer;
		mLayerImpl = spLayerImpl.get();
	}

	bool SqLayer::isValid() const
	{
		return !mLayerForCheck.expired();
	}

	LayerID SqLayer::getId()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getId();
	}

	void SqLayer::setPositionv( const SqVector2& pos )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPosition(pos);
	}

	void SqLayer::setPosition( real x,real y)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPosition(Vector2(x,y));
	}

	void SqLayer::setRotation( real rotation )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRotation( rotation );
	}

	void SqLayer::setPivotRotation( real rotation )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivotRotation( rotation );
	}

	void SqLayer::setScalev( const SqVector2& scale )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setScale(scale);
	}

	void SqLayer::setScale( real x, real y)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setScale( Vector2(x,y) );
	}

	void SqLayer::setScaleUnit( real scale )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setScale(scale);
	}

	void SqLayer::setRGBc(const SqColorRGB& color)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRGB(color.r,color.g,color.b);	
	}

	void SqLayer::setRGB(real r, real g,real b)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRGB( r,g,b );	
	}

	void SqLayer::setAlpha(real alpha)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->setAlpha(alpha);
	}

	SqVector2 SqLayer::getPosition() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getPosition();
	}

	real SqLayer::getRotation() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getRotation();
	}

	real SqLayer::getPivotRotation() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getPivotRotation();
	}

	SqVector2 SqLayer::getScale() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getScale();
	}

	SqColorRGB SqLayer::getRGB() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SqColorRGB color;
		mLayerImpl->getRGB(&color.r,&color.g,&color.b);	
		return color;
	}

	real SqLayer::getAlpha() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getAlpha();
	}

	void SqLayer::setPivotAbsv(const SqVector2& absPivot)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivotAbs(absPivot);
	}

	void SqLayer::setPivotv(const SqVector2& pivot)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivot(pivot);		
	}

	void SqLayer::setPivotAbs(real x,real y)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivotAbs(Vector2(x,y));
	}

	void SqLayer::setPivot(real x,real y)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivot(Vector2(x,y));
	}

	SqVector2 SqLayer::getPivot() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getPivot();
	}

	void SqLayer::setUVVelocityv(const SqVector2& uv)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setUVVelocity(uv);
	}
	
	SqVector2 SqLayer::getUVVelocity()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getUVVelocity();
	}

	void SqLayer::setMoveVelocityv(const SqVector2& vel)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setMoveVelocity(vel);
	}

	SqVector2 SqLayer::getMoveVelocity() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getMoveVelocity();

	}

	void SqLayer::setRotationVelocity(real vel)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRotationVelocity(vel);
	}

	real SqLayer::getRotationVelocity() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getRotationVelocity();
	}

	void SqLayer::setPivotRotationVelocity(real vel)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivotRotationVelocity(vel);
	}

	real SqLayer::getPivotRotationVelocity() const
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl->getPivotRotationVelocity();
	}


	SqSpriteDefault SqLayer::getSpriteDefault()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SpriteDefault* pSprite = dynamic_cast<SpriteDefault*>(mLayerImpl->getSprite());
		mrcScrASSERT(pSprite);
		
		SP_Layer spLayer = mLayerImpl->getThis();
		WP_LayerImpl spLayerImpl = sharedptr_static_cast<LayerImpl>(spLayer); 

		return SqSpriteDefault( spLayerImpl , pSprite );
	}

	SqSpriteButton SqLayer::getSpriteButton()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SpriteButton* pSprite = dynamic_cast<SpriteButton*>(mLayerImpl->getSprite());
		mrcScrASSERT(pSprite);

		SP_Layer spLayer = mLayerImpl->getThis();
		WP_LayerImpl spLayerImpl = sharedptr_static_cast<LayerImpl>(spLayer); 

		return SqSpriteButton( spLayerImpl , pSprite);
	}

	SqSpriteNumber SqLayer::getSpriteNumber()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SpriteNumber* pSprite = dynamic_cast<SpriteNumber*>(mLayerImpl->getSprite());
		mrcScrASSERT(pSprite);

		SP_Layer spLayer = mLayerImpl->getThis();
		WP_LayerImpl spLayerImpl = sharedptr_static_cast<LayerImpl>(spLayer); 

		return SqSpriteNumber( spLayerImpl , pSprite);
	}


	SqLayer SqLayer::createChild( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayer = mLayerImpl->createChild(id);
		return SqLayer(spLayer);
	}

	SqLayer SqLayer::createChildWidthSpriteDefault( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayer = mLayerImpl->createChild<SpriteDefault>(id);
		return SqLayer(spLayer);		
	}

	SqLayer SqLayer::createChildWithSpriteButton( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayer = mLayerImpl->createChild<SpriteButton>(id);
		return SqLayer(spLayer);		
	}

	SqLayer SqLayer::createChildWithSpriteNumber( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayer = mLayerImpl->createChild<SpriteNumber>(id);
		return SqLayer(spLayer);		
	}


	void SqLayer::destroy()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->destroy();
		mLayerForCheck = WP_Layer();
		mLayerImpl = NULL;
	}
// 
// 	void SqLayer::tweenPosition( const SqVector2& pos,real time,real delay,int16 easingMethod , int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenPosition(pos,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenPosition( const SqVector2& pos,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenPosition(pos,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}
// 
// 	void SqLayer::tweenRotation( real rotation,real time,real delay,int16 easingMethod, int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenRotation(rotation,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenRotation( real rotation,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenRotation(rotation,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}
// 
// 	void SqLayer::tweenScale( const SqVector2& scale,real time,real delay,int16 easingMethod , int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenScale(scale,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenScale( const SqVector2& scale,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenScale(scale,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}
// 
// 	void SqLayer::tweenPivotRotation( real pivotRotation,real time,real delay,int16 easingMethod , int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenPivotRotation(pivotRotation,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenPivotRotation( real pivotRotation,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenPivotRotation(pivotRotation,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}
// 
// 	void SqLayer::tweenRGB(const SqColorRGB& color,real time,real delay,int16 easingMethod , int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenRGB(color.r,color.g,color.b,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenRGB( const SqColorRGB& color,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenRGB(color.r,color.g,color.b,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}
// 
// 	void SqLayer::tweenAlpha( real alpha,real time,real delay,int16 easingMethod , int16 easingInOut  )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenAlpha(alpha,time,delay,(tween::EASING_METHOD)easingMethod,(tween::EASING_INOUT)easingInOut);
// 	}
// 
// 	void SqLayer::tweenAlpha( real alpha,real time,real delay )
// 	{
// 		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
// 		mLayerImpl->tweenAlpha(alpha,time,delay,tween::LINEAR,tween::EASE_OUT);
// 	}

	LayerImpl* SqLayer::getPointer() const 
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return mLayerImpl;
	}


	void SqLayer::pushLinePositionQ(const SqVector2& pos,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushLinePositionQ(pos,totalTime,(ease::METHOD)method);
	}

	void SqLayer::pushBezierPositionQ(const SqVector2& middle1,const SqVector2& middle2,
		const SqVector2& end,real totalTime)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushBezierPositionQ(middle1,middle2,end,totalTime);
	}

	void SqLayer::setPositionQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPositionQLoop(isLoop);
	}

	void SqLayer::clearPositionQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearPositionQ();
	}

	void SqLayer::pushRotationQ(real rot,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushRotationQ(rot,totalTime,(ease::METHOD)method);
	}

	void SqLayer::setRotationQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRotationQLoop(isLoop);
	}

	void SqLayer::clearRotationQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearRotationQ();
	}

	void SqLayer::pushScaleQ(const SqVector2& scale,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushScaleQ(scale,totalTime,(ease::METHOD)method);
	}

	void SqLayer::setScaleQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setScaleQLoop(isLoop);
	}

	void SqLayer::clearScaleQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearScaleQ();
	}

	void SqLayer::pushPivotRotationQ(real rot,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushPivotRotationQ(rot,totalTime,(ease::METHOD)method);
	}

	void SqLayer::setPivotRotationQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setPivotRotationQLoop(isLoop);
	}

	void SqLayer::clearPivotRotationQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearPivotRotationQ();
	}

	void SqLayer::pushRGBQ(const SqColorRGB& rgbVector,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushRGBQ(SqVector3(rgbVector.r,rgbVector.g,rgbVector.b),totalTime,(ease::METHOD)method);
	}

	void SqLayer::setRGBQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setRGBQLoop(isLoop);
	}

	void SqLayer::clearRGBQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearRGBQ();
	}

	void SqLayer::pushAlphaQ(real alpha,real totalTime,int16 method)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->pushAlphaQ(alpha,totalTime,(ease::METHOD)method);
	}

	void SqLayer::setAlphaQLoop(bool isLoop)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->setAlphaQLoop(isLoop);
	}

	void SqLayer::clearAlphaQ()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		mLayerImpl->clearAlphaQ();
	}

	//////////////////////////////////////////////////////////////////////////
	
} // mrc