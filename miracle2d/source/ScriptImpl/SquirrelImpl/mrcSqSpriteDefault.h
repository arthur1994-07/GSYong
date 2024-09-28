



#ifndef _mrcSqSpriteDefault_h_
#define _mrcSqSpriteDefault_h_

#include "mrcConfig.h"

#include "mrcUtil/mrcScriptShape.h"
//#include "mrcSqFunction.h"
typedef Sqrat::Function SqFunction;

namespace mrc
{
	class LayerImpl;
	class ILayer;

	typedef mrcWeakenPtr<LayerImpl> WP_LayerImpl;

	class ISprite;
	class SpriteDefault;
	class SpriteButton;
	class SpriteNumber;

	class SqSpriteDefault : public Sqrat::SqObjectType
	{
	public:
		SqSpriteDefault() { mSprite = NULL; }
		explicit SqSpriteDefault(WP_LayerImpl wpLayer,SpriteDefault* pSprite);

		void enableMessageProcedure(bool enable); 

		void setTexture(const TextureDataID& id);
		void setFilter(bool isFilter);

		// Tweener에서 세팅되는것들은 레이어에서...
//		void setRGB(const SqColor& color);
//		void setAlpha(uint16 a);
		void setScreenEffect(bool isScreenEffect);	
		void setScissorRect(const SqRect& rect);


		const TextureDataID& getTextureDataID();
		bool isFilter();

		// Tweener에서 세팅되는것들은 레이어에서...
//		SqColor getRGB();
//		real    getAlpha();
		bool getScreenEffect();	
		SqRect getScissorRect();

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		void setWidth(real width);
		real getWidth();
		void setHeight(real height);
		real getHeight();
		void setWidthHeight(real width,real height);

		//////////////////////////////////////////////////////////////////////////

		void setUV(uint16 vertexIndex,const SqVector2& uv);
		SqVector2 getUV(uint16 vertexIndex);
		void setUVPixel(uint16 vertexIndex,const SqPoint& pixelPoint);
		void setUVFlow(const SqVector2& uv);
		SqVector2 getUVFlow();
		void recoverUV();


		void setCallBackMouseEnter(SqFunction func);
		void setCallBackMouseLeave(SqFunction func);
		void setCallBackMouseLButtonDown(SqFunction func);
		void setCallBackMouseLButtonUp(SqFunction func);
		void setCallBackMouseLButtonClick(SqFunction func);
		void setCallBackMouseRButtonDown(SqFunction func);
		void setCallBackMouseRButtonUp(SqFunction func);
		void setCallBackMouseRButtonClick(SqFunction func);


		void  resetFrame(int16 frame);
		void  pauseFrame();
		void  playFrame();

		void  setSpeedFrame(real speed);
		void  setLoopFrame(bool isLoop);

		//////////////////////////////////////////////////////////////////////////
		static void setWidthHeightFrameStyle(bool isFrame);

	private:
		SpriteDefault& getThis();
	protected:
		bool isValid() const;

	protected:
		WP_LayerImpl   mLayerForCheck;
		ISprite*	   mSprite;
	};


	class SqSpriteButton : public SqSpriteDefault
	{
	public:
		SqSpriteButton() {}
		SqSpriteButton( WP_LayerImpl wpLayer, SpriteButton* pSprite );
	};

	class SqSpriteNumber : public SqSpriteDefault
	{
	public:
		SqSpriteNumber() {}
		SqSpriteNumber( WP_LayerImpl wpLayer, SpriteNumber* pSprite );

		void  setNumber(int32 number);
		int32 getNumber();
		void  setNumberOption(int32 option);
		void  setNumberAlign(int32 align);
		void  setCipher(int32 cipher);
		void  setGap(real gap);

	private:
		SpriteNumber& getThis();
	};

}











#endif