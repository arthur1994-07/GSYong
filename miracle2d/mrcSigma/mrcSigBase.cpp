
#include "stdafx.h"
#include "mrcSigBase.h"

#include "mrcUtil/mrcVectorOperator.h"

#include "mrcTextureData.h"
#include "mrcTextureMgr.h"
#include "mrcSprite.h"



namespace mrc
{
	class SpriteButton;
}


namespace mrcSig
{
	LPDIRECT3DDEVICEQ UIBase::s_device = NULL;

	UIBase::UIBase()
	{
		mParentLayer = NULL;
		mPosition = Vector2::ZERO;
		mSize     = Vector2::UNIT_SCALE;

		mInvalidCacheMatrix = true;
	}


	void UIBase::setTextureHelp( CUIGroup* pUI,const TextureDataID& id )
	{
		WP_TextureData  wpTextureData =	TextureDataMgr::getInstance().getTextureData(id);
		mrcASSERT_MSG( !wpTextureData.expired() , "not exist texture id : " + toNString(id) );

		const TextureData* pData = wpTextureData.lock().get();
		pUI->SetTexture( pData->getCacheTexture(0) );

		Rect_real frameRect = pData->getFrameRect(0);
		pUI->SetTexturePos(0,D3DXVECTOR2(frameRect.mLeft,  frameRect.mTop) );
		pUI->SetTexturePos(1,D3DXVECTOR2(frameRect.mRight, frameRect.mTop) );
		pUI->SetTexturePos(2,D3DXVECTOR2(frameRect.mRight, frameRect.mBottom) );
		pUI->SetTexturePos(3,D3DXVECTOR2(frameRect.mLeft,  frameRect.mBottom) );

		pUI->SetLocalPos(UIRECT(0,0,pData->getRealWidth(),pData->getRealHeight()));
	}


	bool UIBase::messageProcedure(const Message& msg)
	{
		HWND hWnd = msg.mWnd;
		uint32 msgID = msg.mMsgID;
		WPARAM wParam = msg.mwParam;
		LPARAM lParam = msg.mlParam;

		switch( msgID )
		{
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEMOVE:
			{
				mMsg.mPosX = GET_X_LPARAM(lParam);
				mMsg.mPosY = GET_Y_LPARAM(lParam);
				mMsg.mLB = mMsg.mLB & DXKEY_PRESSED;
				mMsg.mMB = mMsg.mMB & DXKEY_PRESSED;
				mMsg.mRB = mMsg.mRB & DXKEY_PRESSED;
				//					mMsg.mKeys  = GET_KEYSTATE_WPARAM(wParam);
				if( WM_MOUSEWHEEL == msgID )
					mMsg.mScrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);


				if( WM_LBUTTONDBLCLK == msgID )
					mMsg.mLB = DXKEY_DUP;
				else if( WM_LBUTTONDOWN == msgID )
					mMsg.mLB = DXKEY_DOWN | DXKEY_PRESSED;
				else if( WM_LBUTTONUP == msgID )
					mMsg.mLB = DXKEY_UP;
				else if( WM_RBUTTONDBLCLK == msgID )
					mMsg.mRB = DXKEY_DUP;
				else if( WM_RBUTTONDOWN == msgID )
					mMsg.mRB = DXKEY_DOWN | DXKEY_PRESSED;
				else if( WM_RBUTTONUP == msgID )
					mMsg.mRB = DXKEY_UP;
				else if( WM_MBUTTONDBLCLK == msgID )
					mMsg.mMB = DXKEY_DUP;
				else if( WM_MBUTTONDOWN == msgID )
					mMsg.mMB = DXKEY_DOWN | DXKEY_PRESSED;
				else if( WM_MBUTTONUP == msgID )
					mMsg.mMB = DXKEY_UP;

				break;
			}

		}

		return false;
	}

	void UIBase::updateStaticLogic( real elapsedTime )
	{
		_Update(mMsg.mPosX,mMsg.mPosY,mMsg.mLB,mMsg.mMB,mMsg.mRB,mMsg.mScrollDelta,elapsedTime,true);

		// press 빼곤 다른 메세지는 날려버리자
		mMsg.mLB = mMsg.mLB & DXKEY_PRESSED;
		mMsg.mMB = mMsg.mMB & DXKEY_PRESSED;
		mMsg.mRB = mMsg.mRB & DXKEY_PRESSED;

		// 스크롤 역시 메세지 처리후 초기화
		mMsg.mScrollDelta = 0;
	}

	Matrix4 UIBase::makeCacheMatrix()
	{
		if( !mInvalidCacheMatrix )
			return mCacheMatrix; 

		Matrix4 mat = Matrix4::IDENTITY;
		mat.setTrans( toVector3(mPosition) );

		if( mParentLayer )
			mCacheMatrix = mParentLayer->makeCacheMatrix() * mat;

		//////////////////////////////////////////////////////////////////////////
		Vector3 globalPos = mCacheMatrix.getTrans();
		_SetGlobalPos( toVector2(globalPos) );
		//////////////////////////////////////////////////////////////////////////

		mInvalidCacheMatrix = false;
		return mCacheMatrix; 
	}




	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void UIGroup::create( uint32 wndID )
	{
		m_rcLocalPos = UIRECT(0,0,100,100);
		SetGlobalPos( m_rcLocalPos );
		SetWndID( wndID );

		mLayerID = _mrcT("UIGroup_") + toString(GetWndID());
	}

	void UIGroup::setTexture( const TextureDataID& id )
	{
		setTextureHelp(this,id);
	}

	void UIGroup::render() const
	{
		// 구조가 어쩔수 없다 ... const 강제 제거
		const_cast<UIGroup&>(*this).Render(s_device);
	}

	void UIGroup::_SetGlobalPos( const Vector2& vPos )
	{
		SetGlobalPos(D3DXVECTOR2(vPos.x,vPos.y));
	}

	void UIGroup::_Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void BasicTextBox::create( uint32 wndID,int16 width,int16 height,CD3DFontPar* pFont,const mrc::CHAR* text )
	{
		//			CreateSub ( this, controlID, UI_DEFAULT_FLAG );
		m_rcLocalPos = UIRECT(0,0,width,height);
		SetGlobalPos( m_rcLocalPos );
		SetWndID( wndID );
		SetFont( pFont );
		SetOneLineText( text, NS_UITEXTUTIL::WINDOWTITLE );
		SetUseRender(false);

		mLayerID = _mrcT("BasicTextBox_") + toString(GetWndID());
	}

	void BasicTextBox::render() const 
	{
		mParentLayer->registerFontDraw(this);
	}

	void BasicTextBox::renderFont() const 
	{
		// 이놈은 랜더링 캐쉬가 안되므로 플러쉬를 해줘야 레이어 그려지는 순서를 망치지 않는다
		//			IRenderer::getInstance().flush();
		// 구조가 어쩔수 없다 ... const 강제 제거
		const_cast<BasicTextBox&>(*this).RenderText();
	}

	void BasicTextBox::_SetGlobalPos( const Vector2& vPos )
	{
		SetGlobalPos(D3DXVECTOR2(vPos.x,vPos.y));
	}

	void BasicTextBox::_Update( int x, int y, int8 LB, int8 MB, int8 RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
	{
		Update(x,y,LB,MB,RB,nScroll,fElapsedTime,bFirstControl);
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	BasicButton::BasicButton( EngineDeviceMan* pDeviceMan ) : BasicTextBox(pDeviceMan)
	{
	}

	void BasicButton::create( uint32 wndID,int16 width,int16 height,CD3DFontPar* pFont,const mrc::CHAR* text )
	{
		BasicTextBox::create(wndID,width,height,pFont,text);
		mLayerID = LayerID(_mrcT("BasicButton_")) + toString(GetWndID());
		mSpriteButton = SpriteFactory::getInstance().createObject<SpriteButton>();
		mSpriteButton->setLayer(this);
		SetTextAlign(TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y);
	}

	void BasicButton::setTexture( const TextureDataID& id )
	{
		WP_TextureData  wpTextureData =	TextureDataMgr::getInstance().getTextureData(id);
		mrcASSERT_MSG( !wpTextureData.expired() , "not exist texture id : " + toNString(id) );

		const TextureData* pData = wpTextureData.lock().get();
		mSize.x = pData->getRealWidth();
		mSize.y = pData->getRealHeight();

		m_rcLocalPos = UIRECT(0,0,mSize.x,mSize.y);
		SetGlobalPos( m_rcLocalPos );
		SetGlobalPos( D3DXVECTOR2(mPosition.x,mPosition.y) );
	
		mSpriteButton->setTexture( id );
	}

	void BasicButton::render() const 
	{
		mParentLayer->registerFontDraw(this);
		mSpriteButton->render(mCacheMatrix);
	}

	void BasicButton::_SetGlobalPos( const Vector2& pos )
	{

		SetGlobalPos( D3DXVECTOR2(pos.x, pos.y) );		
	}

	void BasicButton::_Update( int , int , int8 , int8 , int8 , int , float , BOOL )
	{
		mSpriteButton->update(0);
	}

	bool BasicButton::messageProcedure(const Message& msg)
	{
		return mSpriteButton->messageProcedure(msg);
	}


}


