

#ifndef _mrcSigBase_h_
#define _mrcSigBase_h_

#include "mrcLayer.h"

#include "../enginelib/DxTools/dxFontMan.h"
#include "../enginelib/DxTools/d3dfont.h"

#include "../enginelib/GUInterface/BasicTextBox.h"
#include "../enginelib/GUInterface/BasicButton.h"


namespace mrcSig
{
	using namespace mrc;

	class UIBase : public ILayer
	{
	protected:
		static LPDIRECT3DDEVICEQ s_device;

		struct MSG
		{
			MSG() 
			{
				mPosX = 0;
				mPosY = 0;
				mLB = 0;
				mMB = 0;
				mRB = 0;
				mScrollDelta = 0;
			}

			int32 mPosX;
			int32 mPosY;
			int8  mLB;
			int8  mMB;
			int8  mRB;
			//			int16 mKeys;
			int16 mScrollDelta;
		};

	public:
		UIBase();

		static void setDevice(LPDIRECT3DDEVICEQ device) { s_device = device; }
		static void setTextureHelp(CUIGroup* pUI,const TextureDataID& id);


		virtual	const LayerID& getId() const mrcOVERRIDE { return mLayerID; }
		virtual Vector2 getPosition() const mrcOVERRIDE { return mPosition; }
		virtual void movePoision(int16 x,int16 y) mrcOVERRIDE { setPosition(getPosition() + Vector2(x,y)); }
		void setPosition(int16 x,int16 y) { setPosition( Vector2(x,y) ); }
		void setPosition(const Vector2& pos) { mPosition = pos; invalidCacheMatrix(); }

		virtual bool messageProcedure(const Message& msg) mrcOVERRIDE;

		virtual void updateStaticLogic(real elapsedTime) mrcOVERRIDE;

		virtual const Matrix4& getMatrix() mrcOVERRIDE { return mCacheMatrix; }

		virtual void  setLayerListInfo(const ListInfo& listInfo) mrcOVERRIDE { mInfo = listInfo; }
		virtual const ListInfo& getLayerListInfo() const mrcOVERRIDE { return mInfo; }

		virtual ISprite* getSprite() mrcOVERRIDE { return NULL; }
		virtual ILayer* getParentLayer() const mrcOVERRIDE { return mParentLayer; }
		virtual void setParentLayer(ILayer* pLayer) mrcOVERRIDE { mParentLayer = pLayer; }

	private:
		virtual void _Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) = 0;
		virtual void _SetGlobalPos(const Vector2& vPos) = 0;

		virtual void invalidCacheMatrix() mrcOVERRIDE { mInvalidCacheMatrix = true; }
		virtual Matrix4 makeCacheMatrix() mrcOVERRIDE;
	protected:
		ILayer*  mParentLayer;
		ListInfo mInfo;
		LayerID  mLayerID;

		Vector2  mPosition;
		Vector2  mSize;

		bool     mInvalidCacheMatrix;
		Matrix4  mCacheMatrix;

		MSG      mMsg;
	};

	enum LAYER_TYPE
	{
		TYPE_UI_CONTROL= TYPE_SCOPE_TRAFT, 
		TYPE_UI_BASIC_TEXT,
		TYPE_UI_BUTTON,
	};

	class UIGroup : public UIBase , private CUIGroup
	{
	public:
		UIGroup(EngineDeviceMan* pDeviceMan) : CUIGroup(pDeviceMan) {}

		void create(uint32 wndID);

		//////////////////////////////////////////////////////////////////////////
		void setTexture(const TextureDataID& id);

		virtual const uint32 getType() const mrcOVERRIDE { return TYPE_UI_CONTROL; }

		// 상태를 변화 시키지 않아야 나중에 랜더링 쓰레드로 뺄수 있다 const가 필수
		virtual void render() const mrcOVERRIDE;

	private:
		virtual void _SetGlobalPos(const Vector2& vPos);
		virtual void _Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	};


	class BasicTextBox : public UIBase , public CBasicTextBox , public IFontDraw
	{
	public:
		//		BasicTextBox() : CBasicTextBox(NULL) {}
		BasicTextBox(EngineDeviceMan* pDeviceMan) : CBasicTextBox(pDeviceMan) {}

		virtual void create(uint32 wndID,int16 width,int16 height,CD3DFontPar* pFont,const mrc::CHAR* text);

		//////////////////////////////////////////////////////////////////////////
		virtual const uint32 getType() const mrcOVERRIDE { return TYPE_UI_BASIC_TEXT; }

		virtual void render() const mrcOVERRIDE;

		virtual void renderFont() const mrcOVERRIDE;

	private:
		virtual void _SetGlobalPos(const Vector2& vPos);
		virtual void _Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	};

	class BasicButton : public BasicTextBox
	{
	public:
		BasicButton(EngineDeviceMan* pDeviceMan);

		//////////////////////////////////////////////////////////////////////////
		virtual void create(uint32 wndID,int16 width,int16 height,CD3DFontPar* pFont,const mrc::CHAR* text);

		void setTexture(const TextureDataID& id);

		virtual const uint32 getType() const mrcOVERRIDE { return TYPE_UI_BUTTON; }

		// 상태를 변화 시키지 않아야 나중에 랜더링 쓰레드로 뺄수 있다 const가 필수
		virtual void render() const mrcOVERRIDE;
		virtual bool messageProcedure(const Message& msg) mrcOVERRIDE;

	private:
		virtual void _SetGlobalPos(const Vector2& vPos);
		virtual void _Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	private:
		mrcAutoPtr<ISprite> mSpriteButton; 
	};


}






#endif



