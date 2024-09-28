

#ifndef _mrcSprite_h_
#define _mrcSprite_h_

#include "mrcConfig.h"
#include "mrcAllocatedObject.h"
#include "mrcUtil/mrcFactory.h"
#include "mrcUtil/mrcScriptType.h"
#include "mrcUtil/mrcPolymorphFactory.h"
#include "mrcVector2.h"
#include "mrcShape.h"


namespace mrc
{
	class TextureData;
	
	typedef mrcSharedPtr< TextureData > SP_TextureData;
	typedef mrcWeakenPtr< const TextureData > WP_TextureData;


	class Matrix4;
	class AxisAlignedBox;

	struct IAnimationData;
	struct IFrameData;

// 	class SpriteEventListener
// 	{
// 	public:
// 		virtual void  eventMovePoision(int16 x,int16 y) = 0;
// 		virtual const Matrix4& getMatrix() = 0;
// 	};

	class ILayer;

	class ISpriteAnimation : public AllocatedObject
	{
	public:			
		virtual void setData(const TextureData* pTextureData) = 0;
		virtual void  update(real elapsedTime) = 0;
		virtual int16 getCurrentFrame() = 0;
		virtual void  reset(int16 frame) = 0;
		virtual void  pause() = 0;
		virtual void  play() = 0;
		virtual void  setSpeed(real speed) = 0;
		virtual void  setLoop(bool isLoop) = 0;
	};

	class ISprite : public IPolymorphBaseObject , public AllocatedObject
	{
	protected:
		ISprite() {}
	public:
		virtual ~ISprite() {}

		virtual void setLayer(ILayer* pListener) = 0;
		virtual ILayer* getLayer() const = 0;

		virtual void invalidAabb() = 0;
		virtual void makeAabb(const Matrix4& matrix) = 0;
		virtual const AxisAlignedBox& getAabb() = 0;

		virtual void copyTo(ISprite& dstSprite) = 0;

		virtual void setTexture(const TextureDataID& id) = 0;

		virtual void update(real elapsedTime) = 0;
		virtual bool messageProcedure(const Message& msg) = 0;
		virtual void enableMessageProcedure(bool enable) = 0;
		virtual void onMouseLeave() = 0;
// 		virtual bool isMouseEntered() = 0;
// 		virtual bool isMouseDragging() = 0;
		virtual void setUVFlow(const Vector2& uv) {}
		virtual Vector2 getUVFlow() { return Vector2::ZERO; }


		void render(const Matrix4& matrix)
		{
			setMaterial();
			renderVertice(matrix);	
		}


		virtual void setRGB(uint8 r,uint8 g,uint8 b) = 0;
		virtual void setAlpha(uint8 a) = 0;
		virtual void getRGB(uint8* r,uint8* g,uint8* b) = 0;
		virtual uint8 getAlpha() = 0;



		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//  이건 꼭 필요한게 아니라 잠시 다른 UI에 여기 Sprite를 가져다 쓰느라 만든(호환용) 것이다 
		//  ------------           deprecated             -----------------------------------------------
		virtual void setCallBackMouseEnter(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseLeave(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseLButtonDown(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseLButtonUp(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseLButtonClick(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseRButtonDown(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseRButtonUp(const ScriptFunction& func) { mrcASSERT(false); }
		virtual void setCallBackMouseRButtonClick(const ScriptFunction& func) { mrcASSERT(false); }
		//-----------------------------------------------------------------------------------------------
		virtual void enableFirstClickEvent(bool enable) = 0;
//		virtual bool isEnableFirstClickEvent() const = 0;

		static ISprite* getMouseEnterSprite() { return msMouseEnterSprite; }
		static ISprite* getMouseDragSprite()  { return msMouseDragSprite;  }
	protected:
		virtual void setMaterial() = 0;
		virtual void renderVertice(const Matrix4& matrix) = 0;

	
	private:
	protected:
		static ISprite* msMouseEnterSprite;
		static ISprite* msMouseDragSprite;
	};

	typedef mrcAutoPtr<ISprite> AP_ISprite;
	typedef mrcAutoPtr<ISpriteAnimation> AP_ISpriteAnimation;
	


	//////////////////////////////////////////////////////////////////////////

	class SpriteFactory : public Factory<ISprite>
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SpriteFactory);
		SpriteFactory() {}

	public:
		static SpriteFactory& getInstance();
	};

	class SpriteAnimationFactory : public Factory<ISpriteAnimation>
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SpriteAnimationFactory);
		SpriteAnimationFactory() {}

	public:
		static SpriteAnimationFactory& getInstance();
	};

}



#endif