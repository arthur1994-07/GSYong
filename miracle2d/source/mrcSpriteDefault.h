

#ifndef _mrcSpriteDefault_h_
#define _mrcSpriteDefault_h_

#include "mrcConfig.h"
#include "mrcSprite.h"
#include "mrcAxisAlignedBox.h"
#include "mrcSoftwareVertexBuffer.h"

#include "mrcUtil/mrcScriptType.h"
#include "mrcRenderer.h"


namespace mrc
{

	class ILayer;

	class SpriteDefault : public ISprite
	{
	protected:
		MIRACLE_FRIEND_FACTORY;
		SpriteDefault();
		virtual ~SpriteDefault();
		
		class RefTextureData
		{
		public:
			RefTextureData() { mRefData = NULL;	}
			RefTextureData(const String& id);

			void update();

			const String& getID() const { return mID; }
			bool  expired() const { return mTextureData.expired(); }
			const TextureData* get() const { return mRefData; }

		private:
			TextureDataID		 mID;
			const TextureData*   mRefData;
			WP_TextureData		 mTextureData;
		};

	public:
		virtual void setLayer(ILayer* pLayer) { mLayer = pLayer; }
		virtual ILayer* getLayer() const { return mLayer; }

		virtual void invalidAabb() mrcOVERRIDE { mInvalidAabb = true;}
		virtual void makeAabb(const Matrix4& matrix) mrcOVERRIDE;
		virtual const AxisAlignedBox& getAabb() mrcOVERRIDE;

		virtual void copyTo(ISprite& dstSprite) mrcOVERRIDE;

		virtual void update(real elapsedTime) mrcOVERRIDE;
		virtual bool messageProcedure(const Message& msg) mrcOVERRIDE;
// 		virtual bool isMouseEntered() { return mIsMouseEntered; }
// 		virtual bool isMouseDragging() { return mIsMouseDragging; }

		//////////////////////////////////////////////////////////////////////////
		virtual void enableMessageProcedure(bool enable) mrcOVERRIDE { mEnableMessageProcedure = enable; }
		void setEnableDrag();
		

		bool isVisible() { return getAlpha() > 0; }
		//////////////////////////////////////////////////////////////////////////
		virtual void setTexture(const TextureDataID& id);
		void setFilter(bool isFilter);
		virtual void setRGB(uint8 r,uint8 g,uint8 b) mrcOVERRIDE;
		virtual void setAlpha(uint8 a) mrcOVERRIDE;
		void setScreenEffect(bool isScreenEffect);	
		void setScissorRect(const Rect& rect);


		const TextureDataID& getTextureDataID();
		bool isFilter();
		virtual void getRGB(uint8* r,uint8* g,uint8* b) mrcOVERRIDE;
		virtual uint8 getAlpha() mrcOVERRIDE;
		bool getScreenEffect();	
		const Rect& getScissorRect();

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		void setWidth(real width);
		real getWidth();
		void setHeight(real height);
		real getHeight();
		void setWidthHeight(real width,real height);

		//////////////////////////////////////////////////////////////////////////

		void setUV(uint8 vertexIndex,const Vector2& uv);
		Vector2 getUV(uint8 vertexIndex);

		// 픽셀값으로 직접 지정-> UV로 변환됨
		void setUVPixel(uint8 vertexIndex,const Point& pixelPoint);

		virtual void setUVFlow(const Vector2& uv);
		virtual Vector2 getUVFlow();

		void recoverUV();
		//////////////////////////////////////////////////////////////////////////
		static void setWidthHeightFrameStyle(bool isFrame);
		//////////////////////////////////////////////////////////////////////////
	
		virtual void setCallBackMouseEnter(const ScriptFunction& func) mrcOVERRIDE;
		virtual void setCallBackMouseLeave(const ScriptFunction& func) mrcOVERRIDE;
		virtual void setCallBackMouseLButtonDown(const ScriptFunction& func)mrcOVERRIDE;
		virtual void setCallBackMouseLButtonUp(const ScriptFunction& func)mrcOVERRIDE;
		virtual void setCallBackMouseLButtonClick(const ScriptFunction& func)mrcOVERRIDE;
		virtual void setCallBackMouseRButtonDown(const ScriptFunction& func)mrcOVERRIDE;
		virtual void setCallBackMouseRButtonUp(const ScriptFunction& func)mrcOVERRIDE;
		virtual void setCallBackMouseRButtonClick(const ScriptFunction& func)mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////////////

		void  resetFrame(int16 frame);
		void  pauseFrame();
		void  playFrame();

		void  setSpeedFrame(real speed);
		void  setLoopFrame(bool isLoop);

		//////////////////////////////////////////////////////////////////////////
		
		virtual void enableFirstClickEvent(bool enable) mrcOVERRIDE { mEnableFirstClickEvent = enable; }
//		virtual bool isEnableFirstClickEvent() const mrcOVERRIDE { return mEnableFirstClickEvent; }

	protected:
		void makeUV(int16 frame);
	private:

		virtual void setMaterial() mrcOVERRIDE;
		virtual void renderVertice(const Matrix4& matrix) mrcOVERRIDE;

		void setCallBackMouseHelper(AP_ScriptFunction* myFunc,ScriptFunction func);
		bool isHitReigion(int16 x,int16 y,const Matrix4& matrix);


		bool onMouseDown(int16 x,int16 y,const Matrix4& matrix,ScriptFunction* pOnMouseDown,ScriptFunction* pOnMouseClick,bool* pButtonPressed);
		bool onMouseUp(int16 x,int16 y,const Matrix4& matrix,ScriptFunction* pOnMouseUp,ScriptFunction* pOnMouseClick,bool* pButtonPressed);	
		void onMouseLeave();

	protected:
		// 프레임 스타일과 일반 중앙 스타일을 구분한다
		static void setWidthHeightFrameStyle(Vertex* vertexArray,real width,real height);
		static void setWidthHeightCenterStyle(Vertex* vertexArray,real width,real height);

		SoftwareVertexQuad& getVertexQuad();
	protected:
		typedef void (*F_SetWidthHeightStyle)(Vertex* ,real ,real);
		static F_SetWidthHeightStyle msFuncSetWidthHeightStyle;

		ILayer* mLayer;

		RefTextureData mRefTextureData;
		AP_ISpriteAnimation mSpriteAnim;

		Material mMaterial;

		bool mInvalidWidthHeight;
		bool mInvalidAabb;
		AxisAlignedBox mAabbBox;
		Vector2 mUVFlow;
		vector<SoftwareVertexQuad> mVertexQuadVec;


		//////////////////////////////////////////////////////////////////////////
		AP_ScriptFunction mFuncOnMouseEnter;
		AP_ScriptFunction mFuncOnMouseLeave;
		
		AP_ScriptFunction mFuncOnMouseLButtonDown;
		AP_ScriptFunction mFuncOnMouseLButtonUp;
		AP_ScriptFunction mFuncOnMouseLButtonClick;
		
		AP_ScriptFunction mFuncOnMouseRButtonDown;
		AP_ScriptFunction mFuncOnMouseRButtonUp;
		AP_ScriptFunction mFuncOnMouseRButtonClick;


		bool mIsMouseLButtonPressed;
		bool mIsMouseRButtonPressed;

//		bool mIsMouseDragging;
		int16 mMouseDragX;
		int16 mMouseDragY;

//		bool mIsMouseEntered;
		bool mEnableMessageProcedure;
		bool mEnableMouseDrag;

		bool mEnableFirstClickEvent;

	};



	class SpriteButton : public SpriteDefault
	{
		MIRACLE_FRIEND_FACTORY;
		SpriteButton();

		enum STATE{STATE_DEFAULT,STATE_MOUSEOVER,STATE_PRESSED,STATE_DISABLED,STATE_COUNT};

	private:
		virtual void update( real elapsedTime ) mrcOVERRIDE;
		virtual void renderVertice(const Matrix4& matrix) mrcOVERRIDE;

		void makeState();
	private:
		int32 mStatesTextureNumber[STATE_COUNT];
	};

	class SpriteNumber : public SpriteDefault
	{
		MIRACLE_FRIEND_FACTORY;
		SpriteNumber();

	public:
		enum NUMBER_ALIGN{NUMBER_ALIGN_LEFT,NUMBER_ALIGN_RIGHT};
		enum NUMBER_OPTION{NUMBER_OPTION_EMPTY,NUMBER_OPTION_FILL_ZERO,NUMBER_OPTION_PLUS_MINUS};

	public:

		void  setNumber(int32 number);
		int32 getNumber() { return mNumber; }
		
		void setNumberOption(NUMBER_OPTION option);
		void setNumberAlign(NUMBER_ALIGN align);
		void setCipher(int32 cipher);
		void setGap(real gap);

	private:
		virtual void update( real elapsedTime ) mrcOVERRIDE;
		virtual void renderVertice(const Matrix4& matrix) mrcOVERRIDE;
		void makeNumber();
	private:
		bool		  mIsInvalid;
		int32		  mNumber;
		NUMBER_OPTION mOption;
		NUMBER_ALIGN  mAlign;
		int32		  mCipher;
		real		  mGapFromNumberToNumber;

	};

	// 판넬의 기본 컨셉
	// 4모서리는 나두고  위아래 양옆 가운데는 크기를 늘리면 늘어난다
	// -----------------------------------------------------
	//       l		  l
	//    ---l--------l----
	//    |  l        l  |
	//ㅡㅡ|ㅡlㅡㅡㅡㅡlㅡ|ㅡㅡㅡㅡ
	//    |	 l		  l	 |
	//ㅡㅡ|ㅡlㅡㅡㅡㅡlㅡ|ㅡㅡㅡㅡ	
	//	  |	 l 		  l	 |
	//	  ---l--------l----
	//		 l		  l
	//-----------------------------------------------------
	class SpritePanel : public ISprite
	{
	public:

	};

}





#endif
