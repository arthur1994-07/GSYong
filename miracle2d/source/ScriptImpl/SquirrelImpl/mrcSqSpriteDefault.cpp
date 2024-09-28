
#include "mrcPrecompiled.h"

#include "mrcSqSpriteDefault.h"

#include "../../mrcSpriteDefault.h"
#include "mrcSqUtil.h"

namespace mrc
{

	SqSpriteDefault::SqSpriteDefault( WP_LayerImpl wpLayer,SpriteDefault* pSprite )
	{
		mrcScrASSERT_MSG(!wpLayer.expired(),"Layer is invalid" );
		mrcScrASSERT_MSG(mSprite,"Sprite is invalid" );
		mLayerForCheck = wpLayer;
		mSprite = pSprite;

	}

	bool SqSpriteDefault::isValid() const
	{
		return !mLayerForCheck.expired();
	}

	SpriteDefault& SqSpriteDefault::getThis()
	{
		return static_cast<SpriteDefault&>(*mSprite);
	}

	void SqSpriteDefault::enableMessageProcedure( bool enable )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().enableMessageProcedure(enable);
	}

	void SqSpriteDefault::setTexture(const TextureDataID& id)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setTexture(id);
	}

	void SqSpriteDefault::setFilter(bool isFilter)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setFilter(isFilter);
	}

// 	void SqSpriteDefault::setRGB(const SqColor& color)
// 	{
// 		MIRICALE_SCRIPT_TEST();
// 		getThis().setRGB( uint8(color.r),uint8(color.g),uint8(color.b));		
// 	}

// 	void SqSpriteDefault::setAlpha(uint16 a)
// 	{
// 		MIRICALE_SCRIPT_TEST();
// 		getThis().setAlpha( uint8(a) );		
// 	}

	void SqSpriteDefault::setScreenEffect(bool isScreenEffect)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setScreenEffect(isScreenEffect);
	}

	void SqSpriteDefault::setScissorRect(const SqRect& rect)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setScissorRect(rect);
	}

	const TextureDataID& SqSpriteDefault::getTextureDataID()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getTextureDataID();
	}

	bool SqSpriteDefault::isFilter()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().isFilter();
	}

// 	SqColor SqSpriteDefault::getRGB()
// 	{
// 		MIRICALE_SCRIPT_TEST();
// 		uint8 a,r,g,b;
// 		getThis().getRGB(&r,&g,&b);
// 		a = getThis().getAlpha();
// 		return SqColor(a,r,g,b);
// 	}
// 
// 	uint16 SqSpriteDefault::getAlpha()
// 	{
// 		MIRICALE_SCRIPT_TEST();
// 		return getThis().getAlpha();
// 	}

	bool SqSpriteDefault::getScreenEffect()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getScreenEffect();
	}

	SqRect SqSpriteDefault::getScissorRect()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SqRect sqRect(getThis().getScissorRect());
		return sqRect;
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	void SqSpriteDefault::setWidth(real width)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setWidth(width);
	}

	real SqSpriteDefault::getWidth()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getWidth();
	}

	void SqSpriteDefault::setHeight(real height)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setHeight(height);
	}
	
	real SqSpriteDefault::getHeight()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getHeight();
	}

	void SqSpriteDefault::setWidthHeight(real width,real height)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setWidthHeight(width,height);
	}

	//////////////////////////////////////////////////////////////////////////

	void SqSpriteDefault::setUV(uint16 vertexIndex,const SqVector2& uv)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setUV(uint8(vertexIndex),uv);
	}

	SqVector2 SqSpriteDefault::getUV(uint16 vertexIndex)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getUV(uint8(vertexIndex));
	}

	void SqSpriteDefault::setUVPixel(uint16 vertexIndex,const SqPoint& pixelPoint)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setUVPixel((uint8)vertexIndex,pixelPoint);
	}


	void SqSpriteDefault::setUVFlow(const SqVector2& uv)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setUVFlow(uv);
	}

	SqVector2 SqSpriteDefault::getUVFlow()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getUVFlow();
	}

	void SqSpriteDefault::recoverUV()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().recoverUV();
	}

	void SqSpriteDefault::setCallBackMouseEnter( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseEnter(func);
	}

	void SqSpriteDefault::setCallBackMouseLeave( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseLeave(func);
	}

	void SqSpriteDefault::setCallBackMouseLButtonDown( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseLButtonDown(func);
	}

	void SqSpriteDefault::setCallBackMouseLButtonUp( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseLButtonUp(func);
	}

	void SqSpriteDefault::setCallBackMouseLButtonClick( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseLButtonClick(func);
	}

	void SqSpriteDefault::setCallBackMouseRButtonDown( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseRButtonDown(func);
	}

	void SqSpriteDefault::setCallBackMouseRButtonUp( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseRButtonUp(func);
	}

	void SqSpriteDefault::setCallBackMouseRButtonClick( SqFunction func )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		mrcScrASSERT_MSG( func.IsNull() || sq_isclosure(func.GetFunc()) ,_mrcT(" is function?") );
		getThis().setCallBackMouseRButtonClick(func);
	}

	void SqSpriteDefault::resetFrame( int16 frame )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		getThis().resetFrame(frame);
	}

	void SqSpriteDefault::pauseFrame()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		getThis().pauseFrame();
	}

	void SqSpriteDefault::playFrame()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		getThis().playFrame();
	}

	void SqSpriteDefault::setSpeedFrame( real speed )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		getThis().setSpeedFrame(speed);
	}

	void SqSpriteDefault::setLoopFrame( bool isLoop )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		getThis().setLoopFrame(isLoop);
	}


	//////////////////////////////////////////////////////////////////////////
	void SqSpriteDefault::setWidthHeightFrameStyle(bool isFrame)
	{
		SpriteDefault::setWidthHeightFrameStyle(isFrame);
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	SqSpriteButton::SqSpriteButton( WP_LayerImpl wpLayer,SpriteButton* pSprite )
	{
		mLayerForCheck = wpLayer;
		mrcScrASSERT_MSG(isValid(),"Layer is invalid" );
		mSprite = pSprite;
		mrcScrASSERT_MSG(mSprite,"Sprite is invalid" );
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	SqSpriteNumber::SqSpriteNumber( WP_LayerImpl wpLayer, SpriteNumber* pSprite )
	{
		mLayerForCheck = wpLayer;
		mrcScrASSERT_MSG(isValid(),"Layer is invalid" );
		mSprite = pSprite;
		mrcScrASSERT_MSG(mSprite,"Sprite is invalid" );
	}

	SpriteNumber& SqSpriteNumber::getThis()
	{
		return static_cast<SpriteNumber&>(*mSprite);
	}

	void SqSpriteNumber::setNumber( int32 number )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		getThis().setNumber(number);
	}

	int32 SqSpriteNumber::getNumber()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().getNumber();
	}
	
	void SqSpriteNumber::setNumberOption(int32 option)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().setNumberOption((SpriteNumber::NUMBER_OPTION)option);
	}

	void SqSpriteNumber::setNumberAlign(int32 align)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().setNumberAlign((SpriteNumber::NUMBER_ALIGN)align);
	}

	void SqSpriteNumber::setCipher(int32 cipher)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().setCipher(cipher);
	}

	void SqSpriteNumber::setGap(real gap)
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		return getThis().setGap(gap);
	}

	



}




