
#include "mrcPrecompiled.h"

#include "mrcSpriteDefault.h"
#include "mrcAxisAlignedBox.h"

#include "mrcRenderer.h"
#include "mrcTextureData.h"
#include "mrcSoftwareVertexBuffer.h"

#include "mrcScript.h"
#include "mrcUtil/mrcVectorOperator.h"
#include "mrcUtil/mrcFixVector.h"
#include "mrcLayer.h"
#include "mrcTextureMgr.h"


namespace mrc
{
	namespace
	{
		MIRACLE_REGISTER_FACTORY(SpriteFactory,SpriteDefault);
		MIRACLE_REGISTER_FACTORY(SpriteFactory,SpriteButton);
		MIRACLE_REGISTER_FACTORY(SpriteFactory,SpriteNumber);


		MIRACLE_STATIC_INIT_FUNC
		(
			SpriteDefault::setWidthHeightFrameStyle(true);
		)
			
		void makeVertexUV(real capHorizon,real capVertical,const TextureData* pData,int16 aniFrame,const Vertex* pSrcVertex,
			SoftwareVertexQuad* pVertexQuad,bool* isAddHorizon,bool* isAddVertical)
		{
			const Rect_real& rect = pData->getFrameRect(aniFrame);
			capHorizon  = fmod(capHorizon,rect.getWidth());
 			capVertical = fmod(capVertical,rect.getHeight());

			if( capHorizon == 0 && capVertical == 0 )
			{
 				Vertex* pDscVertex;
 				SoftwareVertexQuad::ScopeLock lock(pVertexQuad[0],&pDscVertex);
 				std::copy(pSrcVertex,pSrcVertex+4,pDscVertex);
				return;
			}

			real uvStartX = rect.mLeft;
			real uvEndX   = rect.mRight;
			real uvStartY = rect.mTop;
			real uvEndY   = rect.mBottom;

			real posStartX = pSrcVertex[0].pos.x;
			real posEndX   = pSrcVertex[1].pos.x;
			real posStartY = pSrcVertex[0].pos.y;
			real posEndY   = pSrcVertex[2].pos.y;

			real width = posEndX - posStartX;
			real height = posEndY - posStartY;

			real rateHorizon = capHorizon / rect.getWidth();
			real middlePosX	= width * rateHorizon;

			real rateVirtical = capVertical / rect.getHeight();
			real middlePosY = height * rateVirtical;

			if( capHorizon != 0 && capVertical != 0 )
			{
				// 일단 나중에 구현 복잡하네 -_-;
				mrcASSERT_MSG(false,"horizon vertical texture uv is not implemented in same time");

				*isAddHorizon = true;
				*isAddVertical = true;
				return;
			}

			if( capHorizon != 0 )
			{
				*isAddHorizon = true;
				SoftwareVertexQuad& leftQuad  = pVertexQuad[0];
				SoftwareVertexQuad& rightQuad = pVertexQuad[1];
	
				Vertex* pLeftQuadVertex;
				Vertex* pRightQuadVertex;
				SoftwareVertexQuad::ScopeLock lock1(leftQuad,&pLeftQuadVertex);
				SoftwareVertexQuad::ScopeLock lock2(rightQuad,&pRightQuadVertex);
				
				if( capHorizon > 0 )
				{	
					pLeftQuadVertex[0].pos.x = posStartX;
					pLeftQuadVertex[0].pos.y = posStartY;

					pLeftQuadVertex[1].pos.x = posStartX + middlePosX;
					pLeftQuadVertex[1].pos.y = posStartY;

					pLeftQuadVertex[2].pos.x = posStartX + middlePosX;
					pLeftQuadVertex[2].pos.y = posEndY;

					pLeftQuadVertex[3].pos.x = posStartX;
					pLeftQuadVertex[3].pos.y = posEndY;
					//////////////////////////////////////////////////////////////////////////
					pRightQuadVertex[0].pos = pLeftQuadVertex[1].pos;

					pRightQuadVertex[1].pos.x = posEndX;
					pRightQuadVertex[1].pos.y = posStartY;

					pRightQuadVertex[2].pos.x = posEndX;
					pRightQuadVertex[2].pos.y = posEndY;

					pRightQuadVertex[3].pos = pLeftQuadVertex[2].pos;
					
					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					pLeftQuadVertex[0].uv.x = uvEndX - capHorizon;
					pLeftQuadVertex[0].uv.y = uvStartY;

					pLeftQuadVertex[1].uv.x = uvEndX;
					pLeftQuadVertex[1].uv.y = uvStartY;

					pLeftQuadVertex[2].uv.x = uvEndX;
					pLeftQuadVertex[2].uv.y = uvEndY;

					pLeftQuadVertex[3].uv.x = uvEndX - capHorizon;
					pLeftQuadVertex[3].uv.y = uvEndY;
					//////////////////////////////////////////////////////////////////////////
					pRightQuadVertex[0].uv.x = uvStartX;
					pRightQuadVertex[0].uv.y = uvStartY;

					pRightQuadVertex[1].uv.x = uvEndX - capHorizon;
					pRightQuadVertex[1].uv.y = uvStartY;

					pRightQuadVertex[2].uv.x = uvEndX - capHorizon;
					pRightQuadVertex[2].uv.y = uvEndY;

					pRightQuadVertex[3].uv.x = uvStartX;
					pRightQuadVertex[3].uv.y = uvEndY;
				}
				else
				{
					pLeftQuadVertex[0].pos.x = posStartX;
					pLeftQuadVertex[0].pos.y = posStartY;

					pLeftQuadVertex[1].pos.x = posEndX + middlePosX;
					pLeftQuadVertex[1].pos.y = posStartY;

					pLeftQuadVertex[2].pos.x = posEndX + middlePosX;
					pLeftQuadVertex[2].pos.y = posEndY;

					pLeftQuadVertex[3].pos.x = posStartX;
					pLeftQuadVertex[3].pos.y = posEndY;
					//////////////////////////////////////////////////////////////////////////
					pRightQuadVertex[0].pos = pLeftQuadVertex[1].pos;

					pRightQuadVertex[1].pos.x = posEndX;
					pRightQuadVertex[1].pos.y = posStartY;

					pRightQuadVertex[2].pos.x = posEndX;
					pRightQuadVertex[2].pos.y = posEndY;

					pRightQuadVertex[3].pos = pLeftQuadVertex[2].pos;

					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					pLeftQuadVertex[0].uv.x = uvStartX - capHorizon;
					pLeftQuadVertex[0].uv.y = uvStartY;

					pLeftQuadVertex[1].uv.x = uvEndX;
					pLeftQuadVertex[1].uv.y = uvStartY;

					pLeftQuadVertex[2].uv.x = uvEndX;
					pLeftQuadVertex[2].uv.y = uvEndY;

					pLeftQuadVertex[3].uv.x = uvStartX - capHorizon;
					pLeftQuadVertex[3].uv.y = uvEndY;
					//////////////////////////////////////////////////////////////////////////
					pRightQuadVertex[0].uv.x = uvStartX;
					pRightQuadVertex[0].uv.y = uvStartY;

					pRightQuadVertex[1].uv.x = uvStartX - capHorizon;
					pRightQuadVertex[1].uv.y = uvStartY;

					pRightQuadVertex[2].uv.x = uvStartX - capHorizon;
					pRightQuadVertex[2].uv.y = uvEndY;

					pRightQuadVertex[3].uv.x = uvStartX;
					pRightQuadVertex[3].uv.y = uvEndY;
				}

				return;
			}

			if( capVertical != 0)
			{
				*isAddVertical = true;
				SoftwareVertexQuad& topQuad  = pVertexQuad[0];
				SoftwareVertexQuad& bottomQuad = pVertexQuad[2];

				Vertex* pTopQuadVertex;
				Vertex* pBottomQuadVertex;
				SoftwareVertexQuad::ScopeLock lock1(topQuad,&pTopQuadVertex);
				SoftwareVertexQuad::ScopeLock lock2(bottomQuad,&pBottomQuadVertex);

				if( capVertical > 0 )
				{	
					pTopQuadVertex[0].pos.x = posStartX;
					pTopQuadVertex[0].pos.y = posStartY;

					pTopQuadVertex[1].pos.x = posEndX;
					pTopQuadVertex[1].pos.y = posStartY;

					pTopQuadVertex[2].pos.x = posEndX;
					pTopQuadVertex[2].pos.y = posStartY + middlePosY;

					pTopQuadVertex[3].pos.x = posStartX;
					pTopQuadVertex[3].pos.y = posStartY + middlePosY;
					//////////////////////////////////////////////////////////////////////////
					pBottomQuadVertex[0].pos = pTopQuadVertex[3].pos;

					pBottomQuadVertex[1].pos = pTopQuadVertex[2].pos;

					pBottomQuadVertex[2].pos.x = posEndX;
					pBottomQuadVertex[2].pos.y = posEndY;

					pBottomQuadVertex[3].pos.x = posStartX;
					pBottomQuadVertex[3].pos.y = posEndY;

					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					pTopQuadVertex[0].uv.x = uvStartX;
					pTopQuadVertex[0].uv.y = uvEndY - capVertical;

					pTopQuadVertex[1].uv.x = uvEndX;
					pTopQuadVertex[1].uv.y = uvEndY - capVertical;

					pTopQuadVertex[2].uv.x = uvEndX;
					pTopQuadVertex[2].uv.y = uvEndY;

					pTopQuadVertex[3].uv.x = uvStartX;
					pTopQuadVertex[3].uv.y = uvEndY;
					//////////////////////////////////////////////////////////////////////////
					pBottomQuadVertex[0].uv.x = uvStartX;
					pBottomQuadVertex[0].uv.y = uvStartY;

					pBottomQuadVertex[1].uv.x = uvEndX;
					pBottomQuadVertex[1].uv.y = uvStartY;

					pBottomQuadVertex[2].uv.x = uvEndX;
					pBottomQuadVertex[2].uv.y = uvEndY - capVertical;

					pBottomQuadVertex[3].uv.x = uvStartX;
					pBottomQuadVertex[3].uv.y = uvEndY - capVertical;
				}
				else
				{
					pTopQuadVertex[0].pos.x = posStartX;
					pTopQuadVertex[0].pos.y = posStartY;

					pTopQuadVertex[1].pos.x = posEndX;
					pTopQuadVertex[1].pos.y = posStartY;

					pTopQuadVertex[2].pos.x = posEndX;
					pTopQuadVertex[2].pos.y = posEndY + middlePosY;

					pTopQuadVertex[3].pos.x = posStartX;
					pTopQuadVertex[3].pos.y = posEndY + middlePosY;
					//////////////////////////////////////////////////////////////////////////
					pBottomQuadVertex[0].pos = pTopQuadVertex[3].pos;

					pBottomQuadVertex[1].pos = pTopQuadVertex[2].pos;

					pBottomQuadVertex[2].pos.x = posEndX;
					pBottomQuadVertex[2].pos.y = posEndY;

					pBottomQuadVertex[3].pos.x = posStartX;
					pBottomQuadVertex[3].pos.y = posEndY;

					//////////////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////
					pTopQuadVertex[0].uv.x = uvStartX;
					pTopQuadVertex[0].uv.y = uvStartY - capVertical;

					pTopQuadVertex[1].uv.x = uvEndX;
					pTopQuadVertex[1].uv.y = uvStartY - capVertical;

					pTopQuadVertex[2].uv.x = uvEndX;
					pTopQuadVertex[2].uv.y = uvEndY;

					pTopQuadVertex[3].uv.x = uvStartX;
					pTopQuadVertex[3].uv.y = uvEndY;
					//////////////////////////////////////////////////////////////////////////
					pBottomQuadVertex[0].uv.x = uvStartX;
					pBottomQuadVertex[0].uv.y = uvStartY;

					pBottomQuadVertex[1].uv.x = uvEndX;
					pBottomQuadVertex[1].uv.y = uvStartY;

					pBottomQuadVertex[2].uv.x = uvEndX;
					pBottomQuadVertex[2].uv.y = uvStartY - capVertical;

					pBottomQuadVertex[3].uv.x = uvStartX;
					pBottomQuadVertex[3].uv.y = uvStartY - capVertical;
				}

				return;
			}


			//////////////////////////////////////////////////////////////////////////
		} // makeVertexUV

	}


	SpriteDefault::RefTextureData::RefTextureData( const String& id )
	{
		WP_TextureData  wpTextureData =	TextureDataMgr::getInstance().getTextureData(id);
		mrcASSERT_MSG( !wpTextureData.expired() , _mrcT("not exist texture id : ") + toNString(id) );

		mID = id;
		mTextureData = wpTextureData; 
		mRefData = &refPtr(mTextureData);
	}

	void SpriteDefault::RefTextureData::update()
	{
		if( mTextureData.expired() )
			mTextureData = TextureDataMgr::getInstance().getTextureData(mID);
		
	}

	//////////////////////////////////////////////////////////////////////////
	SpriteDefault::F_SetWidthHeightStyle SpriteDefault::msFuncSetWidthHeightStyle = NULL;
	//////////////////////////////////////////////////////////////////////////

	SpriteDefault::SpriteDefault()
	{
		mLayer = NULL;
		
		mInvalidWidthHeight = true;
		mInvalidAabb = true;

		mUVFlow = Vector2::ZERO;

		mIsMouseLButtonPressed = false;
		mIsMouseRButtonPressed = false;

		//		mIsMouseDragging = false;
		mMouseDragX = 0;
		mMouseDragY = 0;
//		mIsMouseEntered = false;

		mEnableMouseDrag =  true; //false;
		mEnableMessageProcedure = true;

		mEnableFirstClickEvent  = false;

	}

	SpriteDefault::~SpriteDefault()
	{
		if( msMouseEnterSprite == this )
			msMouseEnterSprite = NULL;

		if( msMouseDragSprite == this )
			msMouseDragSprite = NULL;
	}

	void SpriteDefault::makeAabb(const Matrix4& matrix)
	{
		//////////////////////////////////////////////////////////////////////////
		// 버택스를 직접 조정
		if(mInvalidAabb || mInvalidWidthHeight)
		{
			Vertex* pVertex;
			SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
			
			real minX =	Math::POS_INFINITY;
			real minY =	Math::POS_INFINITY;
			real maxX =	Math::NEG_INFINITY;
			real maxY =	Math::NEG_INFINITY;

			for(int32 i=0; i < 4; ++i)
			{
				minX = std::min(minX,pVertex[i].pos.x);
				minY = std::min(minY,pVertex[i].pos.y);

				maxX = std::max(maxX,pVertex[i].pos.x);
				maxY = std::max(maxY,pVertex[i].pos.x);
			}

			mAabbBox.setMinimum(minX,minY);
			mAabbBox.setMaximum(maxX,maxY);

			mInvalidWidthHeight = false;
		
			//////////////////////////////////////////////////////////////////////////

			mAabbBox.transform(matrix);
			mInvalidAabb = false;
		}
	}

	const AxisAlignedBox& SpriteDefault::getAabb()
	{
		return mAabbBox;
	}

	void SpriteDefault::copyTo(ISprite& dstSprite)
	{ 
		mrcASSERT(false);
		//		SpriteDefault* pDstSprite =	dynamic_cast<SpriteDefault*>(&dstSprite);
		//		mrcASSERT(pDstSprite);
		//		*pDstSprite = *this;
	}


	void SpriteDefault::update( real elapsedTime )
	{
		mRefTextureData.update();

		mrcASSERT(!mRefTextureData.expired()); 
		if( !mSpriteAnim.get() )
			return;

		uint16 beforeFrame = mSpriteAnim->getCurrentFrame();
		mSpriteAnim->update(elapsedTime);
	
		if( mSpriteAnim->getCurrentFrame() != beforeFrame )
		{
			int16 frame = mSpriteAnim->getCurrentFrame();
			mMaterial.mTexture = mRefTextureData.get()->getCacheTexture(frame);
			makeUV(frame);
		}

	}

	bool SpriteDefault::onMouseDown(int16 x,int16 y,const Matrix4& matrix,
		ScriptFunction* pOnMouseDown,ScriptFunction* pOnMouseClick,bool* pButtonPressed)
	{
		int16 mousePosX = x;
		int16 mousePosY = y;

		if (pOnMouseDown || pOnMouseClick || mEnableMouseDrag)
		{
			if( isHitReigion(mousePosX,mousePosY,matrix) )
			{
				*pButtonPressed = true;
							 
				if( mEnableFirstClickEvent )
					ILayer::setFirstClickEventLayer(getLayer());
					
				if( pOnMouseDown )
				{
					SCRIPT_CALL_IF_FAILED_MSG((*pOnMouseDown)() , _mrcT("onMouseDownEventFunc Check") );
				}
				
				// 일단 귀찬아서 부모가 회전 먹은것에 대해선 처리 안했따.
				// 꼭 필요하다면 해주겠지만 왠지 필요 없을듯 함
				if( mEnableMouseDrag )
				{
					if( !msMouseDragSprite )
					{
						msMouseDragSprite = this;
						mMouseDragX = mousePosX;
						mMouseDragY = mousePosY;
					}
				}

				return true;
			}
		}

		return false;
	}

	bool SpriteDefault::onMouseUp(int16 x,int16 y,const Matrix4& matrix,
		ScriptFunction* pOnMouseUp,ScriptFunction* pOnMouseClick,bool* pButtonPressed)
	{
		int16 mousePosX = x;
		int16 mousePosY = y;

//		mIsMouseDragging = false;
		msMouseDragSprite = NULL;
		mMouseDragX = 0;
		mMouseDragY = 0;

		if( *pButtonPressed )
		{
			if( isHitReigion(mousePosX,mousePosY,matrix) )
			{
				if( pOnMouseUp )
					SCRIPT_CALL_IF_FAILED_MSG((*pOnMouseUp)(), _mrcT("onMouseUpEventFunc Check"));

				if( pOnMouseClick )
					SCRIPT_CALL_IF_FAILED_MSG((*pOnMouseClick)(), _mrcT("onMouseClickEventFunc Check"));
			}

			*pButtonPressed = false;
			return true;
		}
		
		return false;
	}

	void SpriteDefault::onMouseLeave()
	{
		if( this == msMouseEnterSprite )
		{
			msMouseEnterSprite = NULL;
			if( mFuncOnMouseLeave.get() )
				SCRIPT_CALL_IF_FAILED_MSG((*mFuncOnMouseLeave)() , _mrcT("onMouseLeaveEventFunc Check") );
		}
	}

	bool SpriteDefault::messageProcedure( const Message& msg )
	{
		if( !isVisible() )
			return false;

		if( !mEnableMessageProcedure )
			return false;

		mrcASSERT(mLayer);
		const Matrix4& matrix = mLayer->getMatrix();

		HWND hWnd = msg.mWnd;
		uint32 msgID = msg.mMsgID;
		WPARAM wParam = msg.mwParam;
		LPARAM lParam = msg.mlParam;
	
 		switch( msgID )
		{
		case WM_LBUTTONDOWN:    
			return onMouseDown(int16(GET_X_LPARAM(lParam)),int16(GET_Y_LPARAM(lParam)),matrix,
				mFuncOnMouseLButtonDown.get(),mFuncOnMouseLButtonClick.get(),&mIsMouseLButtonPressed);
			
		case WM_LBUTTONUP:      
			return onMouseUp(int16(GET_X_LPARAM(lParam)),int16(GET_Y_LPARAM(lParam)),matrix,
				mFuncOnMouseLButtonUp.get(),mFuncOnMouseLButtonClick.get(),&mIsMouseLButtonPressed);
		case WM_RBUTTONDOWN:
			return onMouseDown(int16(GET_X_LPARAM(lParam)),int16(GET_Y_LPARAM(lParam)),matrix,
				mFuncOnMouseRButtonDown.get(),mFuncOnMouseRButtonClick.get(),&mIsMouseRButtonPressed);
		case WM_RBUTTONUP:
			return onMouseUp(int16(GET_X_LPARAM(lParam)),int16(GET_Y_LPARAM(lParam)),matrix,
				mFuncOnMouseRButtonUp.get(),mFuncOnMouseRButtonClick.get(),&mIsMouseRButtonPressed);
		case WM_MOUSEMOVE:      
			{
				int16 mousePosX = int16(LOWORD(lParam));
				int16 mousePosY = int16(HIWORD(lParam));

				//if(mIsMouseDragging)
				if( this == msMouseDragSprite )
				{
					mLayer->setPosition(mLayer->getPosition() + Vector2(real(mousePosX - mMouseDragX),real(mousePosY - mMouseDragY)));	
					mMouseDragX = mousePosX;
					mMouseDragY = mousePosY;
					return true;
				}

				if( isHitReigion(mousePosX,mousePosY,matrix) )
				{
					if( this != msMouseEnterSprite )				
					{
						if( msMouseEnterSprite )
							msMouseEnterSprite->onMouseLeave();

						if( mFuncOnMouseEnter.get() )
							SCRIPT_CALL_IF_FAILED_MSG( (*mFuncOnMouseEnter)() , _mrcT("onMouseEnterEventFunc Check"));

						msMouseEnterSprite = this;
					}
					// 이걸 처리한걸로 하면 드래깅할때  다른 객체를 통해 이동할때 멈짓해버림
					return true;
				}
				else
				{
					onMouseLeave();
				}
				
				
				break;
			}	// case WM_MOUSEMOVE:		

		} // switch( msgID )

		return false;
	}

	void SpriteDefault::setMaterial()
	{
		if( IsInvalidHandle(mMaterial.mTexture) )
			mMaterial.mTexture = IRenderer::getInstance().getEmptyWhiteTexture();

		IRenderer::getInstance().setMaterial(mMaterial);
	}
	
	void SpriteDefault::renderVertice(const Matrix4& matrix)
	{
		const TextureData* pData = mRefTextureData.get();
		mrcASSERT(pData);	
		if( pData->isVertexForTextureUV() )
		{
			// 딱 딱어지는 텍스쳐가 이니기에 ...
			SoftwareVertexQuad vertexQuad[3];
			bool isAddHorizon  = false;
			bool isAddVertical = false;
			
			Vertex* pSrcVertex;
			SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pSrcVertex);
			makeVertexUV(mUVFlow.x,mUVFlow.y,pData,mSpriteAnim->getCurrentFrame(),
				pSrcVertex,vertexQuad,&isAddHorizon,&isAddVertical);
			
			IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,vertexQuad[0].getRawBuffer(),vertexQuad[0].size());
			if(isAddHorizon)
				IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,vertexQuad[1].getRawBuffer(),vertexQuad[1].size());
			if(isAddVertical)	
				IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,vertexQuad[2].getRawBuffer(),vertexQuad[2].size());
		}
		else
		{
			// 실제 크기와 택스쳐 크기가 딱 떨어지면...
			IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,getVertexQuad().getRawBuffer(),getVertexQuad().size());			
		}	
	}


	//////////////////////////////////////////////////////////////////////////
	void SpriteDefault::setTexture( const TextureDataID& id )
	{
		if( !mRefTextureData.expired() )
		{
			if( mRefTextureData.getID() == id )
				return;
		}

		mRefTextureData = RefTextureData(id);

		const TextureData* pData = mRefTextureData.get();
		mSpriteAnim = pData->createSpriteAnim();
		mSpriteAnim->setData(pData);
		mSpriteAnim->reset(0);

		mMaterial.mTexture = pData->getCacheTexture(mSpriteAnim->getCurrentFrame());
		const Rect& regionRect = pData->getRegionRect();
		setWidthHeight(pData->getRealWidth(),pData->getRealHeight());
		
		makeUV(0);
	}

	void SpriteDefault::setRGB(uint8 r,uint8 g,uint8 b)
	{
		uint8 getA,getR,getG,getB;
		getColorARGB(mMaterial.mARGB,&getA,&getR,&getG,&getB);
		mMaterial.mARGB = makeColorARGB(getA,r,g,b);
	}

	void SpriteDefault::setAlpha(uint8 a)
	{
		uint8 getA,getR,getG,getB;
		getColorARGB(mMaterial.mARGB,&getA,&getR,&getG,&getB);
		mMaterial.mARGB = makeColorARGB(a,getR,getG,getB);
	}


	void SpriteDefault::setFilter( bool isFilter )
	{
		mMaterial.mIsFilter = isFilter;
	}

	void SpriteDefault::setScissorRect( const Rect& rect )
	{
		mMaterial.mScissorRect = rect;
	}

	void SpriteDefault::setScreenEffect( bool isScreenEffect )
	{
		mMaterial.mIsScreenEffect = isScreenEffect;
	}

	const TextureDataID& SpriteDefault::getTextureDataID()
	{
		return mRefTextureData.getID();
	}

	bool SpriteDefault::isFilter()
	{
		return mMaterial.mIsFilter;
	}

	void SpriteDefault::getRGB(uint8* r,uint8* g,uint8* b)
	{
		uint8 a;
		getColorARGB(mMaterial.mARGB,&a,r,g,b);
	}

	uint8 SpriteDefault::getAlpha()
	{
		uint8 a,r,g,b;
		getColorARGB(mMaterial.mARGB,&a,&r,&g,&b);
		return a;
	}

	bool SpriteDefault::getScreenEffect()
	{
		return mMaterial.mIsScreenEffect;
	}

	const Rect& SpriteDefault::getScissorRect()
	{
		return mMaterial.mScissorRect;
	}
	//////////////////////////////////////////////////////////////////////////

	void SpriteDefault::setWidth( real width )
	{
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		msFuncSetWidthHeightStyle(pVertex,width,INVALID_SIZE);
		mInvalidWidthHeight = true;
	}


	real SpriteDefault::getWidth()
	{
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		return pVertex[1].pos.x - pVertex[0].pos.x;
	}

	void SpriteDefault::setHeight( real height )
	{
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		msFuncSetWidthHeightStyle(pVertex,INVALID_SIZE,height);
		mInvalidWidthHeight = true;
	}

	real SpriteDefault::getHeight()
	{
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		return pVertex[2].pos.y - pVertex[1].pos.y;
	}

	void SpriteDefault::setWidthHeight( real width,real height )
	{
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		msFuncSetWidthHeightStyle(pVertex,width,height);
		mInvalidWidthHeight = true;
	}
	

	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	void SpriteDefault::setWidthHeightFrameStyle( bool isFrame )
	{
		if(isFrame)
			msFuncSetWidthHeightStyle = &SpriteDefault::setWidthHeightFrameStyle;
		else
			msFuncSetWidthHeightStyle = &SpriteDefault::setWidthHeightCenterStyle;

	}
	//////////////////////////////////////////////////////////////////////////


	void SpriteDefault::setWidthHeightCenterStyle(Vertex* vertexArray,real width,real height)
	{
		if( width != INVALID_SIZE )
		{
			mrcASSERT( width >= 0);
			real halfWidth = real(width)/2;
			vertexArray[0].pos.x = -halfWidth;
			vertexArray[1].pos.x = halfWidth;
			vertexArray[2].pos.x = halfWidth;
			vertexArray[3].pos.x = -halfWidth;
		}

		if( height != INVALID_SIZE)
		{
			mrcASSERT( height >= 0);
			real halfHeight = real(height)/2;
			vertexArray[0].pos.y = -halfHeight;
			vertexArray[1].pos.y = -halfHeight;
			vertexArray[2].pos.y = halfHeight;
			vertexArray[3].pos.y = halfHeight;
		}
	}

	void SpriteDefault::setWidthHeightFrameStyle(Vertex* vertexArray,real width,real height)
	{
		if( width != INVALID_SIZE )
		{
			mrcASSERT( width >= 0);
			vertexArray[0].pos.x = 0;
			vertexArray[1].pos.x = width;
			vertexArray[2].pos.x = width;
			vertexArray[3].pos.x = 0;
		}

		if( height != INVALID_SIZE)
		{
			mrcASSERT( height >= 0);
			vertexArray[0].pos.y = 0;
			vertexArray[1].pos.y = 0;
			vertexArray[2].pos.y = height;
			vertexArray[3].pos.y = height;
		}
	}



	
	//////////////////////////////////////////////////////////////////////////


	void SpriteDefault::setUV( uint8 vertexIndex,const Vector2& uv )
	{
		mrcASSERT(vertexIndex < 4);
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		pVertex[vertexIndex].uv = uv;

		const TextureData* pData = mRefTextureData.get();
		if( pData->isVertexForTextureUV() )
		{
			Rect_real rect = pData->getFrameRect();
			pVertex[vertexIndex].uv.x *= rect.getWidth();
			pVertex[vertexIndex].uv.y *= rect.getHeight();
		}

		//////////////////////////////////////////////////////////////////////////
		// flow는 꺼버림
		mUVFlow = Vector2::ZERO;
	}

	Vector2 SpriteDefault::getUV( uint8 vertexIndex )
	{
		mrcASSERT(vertexIndex < 4);
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		return pVertex[vertexIndex].uv;
	}

	void SpriteDefault::setUVPixel(uint8 vertexIndex,const Point& pixelPoint)
	{
		Vector2 uv;
		const TextureData* pData = mRefTextureData.get();
		uv.x = real(pixelPoint.mX) / real(pData->getRegionRect().getWidth());
		uv.y = real(pixelPoint.mY) / real(pData->getRegionRect().getHeight());

		setUV(vertexIndex,uv);
	}


	void SpriteDefault::setUVFlow(const Vector2& uv)
	{
		mUVFlow = uv;
	}

	Vector2 SpriteDefault::getUVFlow()
	{
		return mUVFlow;
	}

	void SpriteDefault::recoverUV()
	{
		makeUV(mSpriteAnim->getCurrentFrame());
		mUVFlow = Vector2::ZERO;
	}

	void SpriteDefault::setCallBackMouseEnter( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseEnter,func);
	}

	void SpriteDefault::setCallBackMouseLeave( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseLeave,func);
	}

	void SpriteDefault::setCallBackMouseLButtonDown( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseLButtonDown,func);
	}

	void SpriteDefault::setCallBackMouseLButtonUp( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseLButtonUp,func);
	}

	void SpriteDefault::setCallBackMouseLButtonClick( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseLButtonClick,func);
	}

	void SpriteDefault::setCallBackMouseRButtonDown( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseRButtonDown,func);
	}

	void SpriteDefault::setCallBackMouseRButtonUp( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseRButtonUp,func);
	}

	void SpriteDefault::setCallBackMouseRButtonClick( const ScriptFunction& func )
	{
		setCallBackMouseHelper(&mFuncOnMouseRButtonClick,func);
	}


	void SpriteDefault::setCallBackMouseHelper(AP_ScriptFunction* myFunc,ScriptFunction func )
	{
		if( func.IsNull() )
		{
			myFunc->reset();
			return;
		}

		if(!myFunc->get())
			myFunc->reset(mrcNEW ScriptFunction);

		**myFunc = func;
	}

	void  SpriteDefault::resetFrame(int16 frame)
	{
		mSpriteAnim->reset(frame);
	}

	void  SpriteDefault::pauseFrame()
	{
		mSpriteAnim->pause();
	}

	void  SpriteDefault::playFrame()
	{
		mSpriteAnim->play();
	}

	void  SpriteDefault::setSpeedFrame(real speed)
	{
		mSpriteAnim->setSpeed(speed);
	}

	void  SpriteDefault::setLoopFrame(bool isLoop)
	{
		mSpriteAnim->setLoop(isLoop);
	}

	bool SpriteDefault::isHitReigion( int16 x,int16 y,const Matrix4& matrix )
	{
		mrcASSERT( !mRefTextureData.expired() );
		const vector<Triangle>& triangleVec = mRefTextureData.get()->getHitRegion();
		if( !triangleVec.empty() )
		{
			for (size_t i = 0; i < triangleVec.size(); ++i)
			{
				Vector2 v1 = Vector2(real(triangleVec[i].mPoints[0].mX),real(triangleVec[i].mPoints[0].mY));
				Vector2 v2 = Vector2(real(triangleVec[i].mPoints[1].mX),real(triangleVec[i].mPoints[1].mY));
				Vector2 v3 = Vector2(real(triangleVec[i].mPoints[2].mX),real(triangleVec[i].mPoints[2].mY));

				if( Math::pointInTri2D( Vector2(real(x),real(y)),v1,v2,v3) )
					return true;
			}
	
			return false;
		}

		for (size_t i=0; i < mVertexQuadVec.size(); ++i)
		{
			Vertex* pVertex;
			SoftwareVertexQuad::ScopeLock lock(mVertexQuadVec[i],&pVertex);
			Vector3 v1 = matrix * pVertex[0].pos;
			Vector3 v2 = matrix * pVertex[1].pos;
			Vector3 v3 = matrix * pVertex[2].pos;
			Vector3 v4 = matrix * pVertex[3].pos;

			if( Math::pointInTri2D( Vector2(real(x),real(y)), toVector2(v1),toVector2(v2),toVector2(v3) ) ||
				Math::pointInTri2D( Vector2(real(x),real(y)), toVector2(v3),toVector2(v4),toVector2(v1) ) )
				return true;
		}
			
		return false;
	}

	void SpriteDefault::makeUV(int16 frame)
	{
		Rect_real frameRect = mRefTextureData.get()->getFrameRect(frame);
		Vertex* pVertex;
		SoftwareVertexQuad::ScopeLock lock(getVertexQuad(),&pVertex);
		pVertex[0].uv.x  = frameRect.mLeft;
		pVertex[0].uv.y  = frameRect.mTop;

		pVertex[1].uv.x  = frameRect.mRight;
		pVertex[1].uv.y  = frameRect.mTop;

		pVertex[2].uv.x  = frameRect.mRight;
		pVertex[2].uv.y  = frameRect.mBottom;

		pVertex[3].uv.x  = frameRect.mLeft;
		pVertex[3].uv.y  = frameRect.mBottom;
	}

	SoftwareVertexQuad& SpriteDefault::getVertexQuad()
	{
		if(mVertexQuadVec.empty())
			mVertexQuadVec.resize(1);

		return mVertexQuadVec[0];
	}

	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	SpriteButton::SpriteButton()
	{
		mStatesTextureNumber[STATE_DEFAULT]   = 0;
		mStatesTextureNumber[STATE_MOUSEOVER] = 1;
		mStatesTextureNumber[STATE_PRESSED]   = 2;
		mStatesTextureNumber[STATE_DISABLED]  = 3;
	}

	void SpriteButton::update( real  )
	{
		mrcASSERT(!mRefTextureData.expired()); 
		if( !mSpriteAnim.get() )
			return;

		makeState();
	}

	void SpriteButton::makeState()
	{
		int32 state = mStatesTextureNumber[STATE_DEFAULT];
		// 그림 보여줄 우선 순위 반영
		if( mEnableMessageProcedure == false )
			state = mStatesTextureNumber[STATE_DISABLED];
		else if( mIsMouseLButtonPressed )
			state = mStatesTextureNumber[STATE_PRESSED];
		else if( msMouseEnterSprite == this )
			state = mStatesTextureNumber[STATE_MOUSEOVER];

		if( state >= mRefTextureData.get()->getFrameTotalCount())
			state = mStatesTextureNumber[STATE_DEFAULT];

		makeUV((int16)state);
	}

	void SpriteButton::renderVertice(const Matrix4& matrix)
	{
		IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,getVertexQuad().getRawBuffer(),getVertexQuad().size());
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	SpriteNumber::SpriteNumber()
	{
		mIsInvalid = true;
		mNumber = 0;
		mOption = NUMBER_OPTION_EMPTY;
		mAlign  = NUMBER_ALIGN_LEFT;
		mCipher = 10;
		mGapFromNumberToNumber = 0;
	}

	void SpriteNumber::setNumber( int32 number )
	{
		if(mNumber == number)
			return;

		mrcASSERT(mNumber >= -2000000000);
		mrcASSERT(mNumber <= +2000000000);

		mNumber = number;
		mIsInvalid = true;
	}

	void SpriteNumber::setNumberOption( NUMBER_OPTION option )
	{
		if( mOption == option )
			return;

		mrcASSERT( NUMBER_OPTION_EMPTY == option || NUMBER_OPTION_FILL_ZERO == option || NUMBER_OPTION_PLUS_MINUS == option );
		mOption = option;
		mIsInvalid = true;
	}

	void SpriteNumber::setNumberAlign( NUMBER_ALIGN align )
	{
		if( mAlign == align )
			return;

		mrcASSERT( NUMBER_ALIGN_LEFT == align || NUMBER_ALIGN_RIGHT == align );
		mAlign = align;
		mIsInvalid = true;
	}

	void SpriteNumber::setCipher( int32 cipher )
	{
		if( mCipher == cipher )
			return;

		mrcASSERT(cipher <= 10);
		mCipher = cipher;
		mIsInvalid = true;
	}

	void SpriteNumber::setGap( real gap )
	{
		if( mGapFromNumberToNumber == gap )
			return;

		mGapFromNumberToNumber = gap;
		mIsInvalid = true;
	}

	void SpriteNumber::update( real )
	{
		// 아무것도 하지 않음
		mMaterial.mTexture;
	}

	void SpriteNumber::makeNumber()
	{
		NString numberString = toNString(mNumber);
		if( numberString.size() > size_t(mCipher) )
		{
			numberString.erase(numberString.begin(),numberString.begin() + (numberString.size() - size_t(mCipher)) );
			for (size_t i=0; i < numberString.size(); ++i)
				numberString[i] = '9';
		}

		const int32 number = mNumber;
		const TextureData* pData   = mRefTextureData.get();
		const real oneNumberWidth  = pData->getRealWidth();
		const real oneNumberHeight = pData->getRealHeight();
		size_t cipher = mCipher;
		const NUMBER_ALIGN align = mAlign;
		const NUMBER_OPTION option = mOption;
		
		if( NUMBER_OPTION_FILL_ZERO == option )
		{
			if(  numberString.size() < cipher )
			{
				size_t fill_0_size = cipher - numberString.size();;
				numberString.insert(numberString.begin(),fill_0_size,'0');
			}
		}

		fix_vector<int32,16> numberVec;
		int32 signPos = (option == NUMBER_OPTION_PLUS_MINUS ? 1 : 0);
		//if( Number == 0) signPos = 0;

		numberVec.resize( numberString.size() );
		for (size_t i=0; i < numberString.size(); ++i)
		{
			numberVec[i] = int32( numberString[i] - '0' );
		}

		if(signPos)
		{
			numberVec.insert(numberVec.begin(), number >= 0 ? 10 : 11);
		}

		if( cipher <  numberString.size() ) 
			cipher = numberString.size();


		mVertexQuadVec.clear();
		mVertexQuadVec.reserve( numberString.size() );

		const real oneNumberHeightHalf = oneNumberHeight/2.f;
		const real numberTotalSize = real(oneNumberWidth * numberString.size());
		const real numberTotalSizeHalf = numberTotalSize / 2.f;
		const real cipherTotalSize = cipher *  oneNumberWidth;
		const real cipherTotalSizeHalf = cipherTotalSize / 2.f;

		const real numberGap = oneNumberWidth + mGapFromNumberToNumber;
		
		if( align == NUMBER_ALIGN_RIGHT )
		{
			for (int32 i=0; i > -int32(numberString.size()); --i)
			{
				const Rect_real rect = pData->getFrameRect( numberVec[ numberVec.size() - 1 + i ] );
				Vertex* pDscVertex;
				SoftwareVertexQuad vertexQuad;
				{
					SoftwareVertexQuad::ScopeLock lock(vertexQuad,&pDscVertex);
					pDscVertex[0].pos.x = cipherTotalSize - oneNumberWidth + (i*numberGap);// + _FrameStyleGap;
					pDscVertex[0].pos.y = 0;
					pDscVertex[0].uv.x = rect.mLeft; 
					pDscVertex[0].uv.y = rect.mTop;
					
					pDscVertex[1].pos.x = cipherTotalSize + (i*numberGap);// + _FrameStyleGap;
					pDscVertex[1].pos.y = 0;
					pDscVertex[1].uv.x = rect.mRight;
					pDscVertex[1].uv.y = rect.mTop;
					
					pDscVertex[2].pos.x = cipherTotalSize + (i*numberGap); //  + _FrameStyleGap;
					pDscVertex[2].pos.y = +oneNumberHeight;
					pDscVertex[2].uv.x  = rect.mRight;
					pDscVertex[2].uv.y  = rect.mBottom;

					pDscVertex[3].pos.x = cipherTotalSize - oneNumberWidth + (i*numberGap); //  + _FrameStyleGap;
					pDscVertex[3].pos.y = +oneNumberHeight;
					pDscVertex[3].uv.x  = rect.mLeft;
					pDscVertex[3].uv.y  = rect.mBottom;
				}
				
				mVertexQuadVec.push_back(vertexQuad);
			}
		}
		else if( align == NUMBER_ALIGN_LEFT )
		{
			for (int32 i=0; i < int32(numberVec.size()); ++i)
			{
				const Rect_real rect = pData->getFrameRect( numberVec[ i ] );
				Vertex* pDscVertex;
				SoftwareVertexQuad vertexQuad;
				{
					SoftwareVertexQuad::ScopeLock lock(vertexQuad,&pDscVertex);
					pDscVertex[0].pos.x = 0 + (i*numberGap); //  + _FrameStyleGap;
					pDscVertex[0].pos.y = 0;
					pDscVertex[0].uv.x  = rect.mLeft;
					pDscVertex[0].uv.y  = rect.mTop;
				
					pDscVertex[1].pos.x = 0 + oneNumberWidth + (i*numberGap); //  + _FrameStyleGap;
					pDscVertex[1].pos.y = 0;
					pDscVertex[1].uv.x = rect.mRight;
					pDscVertex[1].uv.y = rect.mTop;

					pDscVertex[2].pos.x = 0 + oneNumberWidth + (i*numberGap); //  + _FrameStyleGap;		
					pDscVertex[2].pos.y = +oneNumberHeight;
					pDscVertex[2].uv.x  = rect.mRight;
					pDscVertex[2].uv.y  = rect.mBottom;

					pDscVertex[3].pos.x = 0 + (i*numberGap); //  + _FrameStyleGap;
					pDscVertex[3].pos.y = +oneNumberHeight;
					pDscVertex[3].uv.x  = rect.mLeft;
					pDscVertex[3].uv.y  = rect.mBottom;
				}
				
				mVertexQuadVec.push_back(vertexQuad);
			}
		}
// 		else if( align == NUMBER_ALIGN_CENTER )
// 		{
// 			for (int32 i=0; i < int32(numberVec.size()); ++i)
// 			{
// 				const Rect_real rect = pData->getFrameRect( numberVec[ i ] );
// 				Vertex* pDscVertex;
// 				SoftwareVertexQuad vertexQuad;
// 				{
// 					SoftwareVertexQuad::ScopeLock lock(vertexQuad,&pDscVertex);
// 					pDscVertex[0].pos.x = (-cipherTotalSizeHalf + (cipherTotalSizeHalf - numberTotalSizeHalf ) ) + (i*numberGap); //  + _FrameStyleGap;
// 					pDscVertex[0].pos.y = -oneNumberHeightHalf;
// 					pDscVertex[0].uv.x = rect.mLeft;
// 					pDscVertex[0].uv.y = rect.mTop;
// 
// 					pDscVertex[1].pos.x = (-cipherTotalSizeHalf + (cipherTotalSizeHalf - numberTotalSizeHalf ) ) + oneNumberWidth + (i*numberGap); //  + _FrameStyleGap;
// 					pDscVertex[1].pos.y = -oneNumberHeightHalf;
// 					pDscVertex[1].uv.x  = rect.mRight;
// 					pDscVertex[1].uv.y  = rect.mTop;
// 
// 					pDscVertex[2].pos.x = (-cipherTotalSizeHalf + (cipherTotalSizeHalf - numberTotalSizeHalf ) ) + oneNumberWidth + (i*numberGap); //  + _FrameStyleGap;
// 					pDscVertex[2].pos.y = +oneNumberHeightHalf;
// 					pDscVertex[2].uv.x  = rect.mRight;
// 					pDscVertex[2].uv.y  = rect.mBottom;
// 
// 					pDscVertex[3].pos.x = (-cipherTotalSizeHalf + (cipherTotalSizeHalf - numberTotalSizeHalf ) ) + (i*numberGap); //  + _FrameStyleGap;
// 					pDscVertex[3].pos.y = +oneNumberHeightHalf;
// 					pDscVertex[3].uv.x  = rect.mLeft;
// 					pDscVertex[3].uv.y  = rect.mBottom;
// 				}
// 
// 				mVertexQuadVec.push_back(vertexQuad);
// 			}
// 		}
		else
		{
			mrcASSERT(false);
		}

		mIsInvalid = false;
	}

	void SpriteNumber::renderVertice(const Matrix4& matrix)
	{
		if( mIsInvalid )
			makeNumber();

		// 숫자가 반드시 세팅되어 있어야함
		mrcASSERT_MSG( !mVertexQuadVec.empty() , "SpriteNumber ought setNumber");

		for (size_t i=0; i < mVertexQuadVec.size(); ++i)
		{
			IRenderer::getInstance().draw(PRIMITIVE_QUAD,matrix,mVertexQuadVec[i].getRawBuffer(),mVertexQuadVec[i].size());			
		}
	}


}


