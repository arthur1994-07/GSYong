


#ifndef _mrcLayer_h_
#define _mrcLayer_h_

#include "mrcConfig.h"

#include "mrcVector2.h"
#include "mrcMatrix4.h"
#include "mrcUtil/mrcFactory.h"
#include "mrcUtil/mrcIteratorWrapper.h"

namespace mrc
{

	class ILayer;
	typedef mrcSharedPtr<ILayer>   SP_Layer;
	typedef mrcWeakenPtr<ILayer>   WP_Layer;

	typedef list<SP_Layer>		   LayerList;
	typedef LayerList::iterator	   LayerListIter;

	typedef VectorIterator<LayerList> LayerListIterWrapper;

	class ISprite;

	class IFontDraw 
	{
	public:
		virtual ~IFontDraw() {}
		virtual void renderFont() const = 0;

	};

	class ILayer 
	{	
	protected:
		ILayer() {}
	public:
		// 위치정보
		struct ListInfo
		{
			ListInfo()
			{
				mLayerList = NULL;
				mLayerListIter = LayerListIter();
				mParentLayer = NULL;
			}

			ListInfo(LayerList* pLayerList,LayerListIter layerListIter,ILayer* pParentLayer)
			{
				mLayerList = pLayerList;
				mLayerListIter = layerListIter;
				mParentLayer = pParentLayer;
			}

			void destroy()
			{
				mrcASSERT(mLayerList);
				if( mLayerListIter != mLayerList->end() )
				{
//					LayerListIter destroyedIt = mLayerListIter;
//					mLayerListIter = mLayerList->end();
//					mLayerList->erase(destroyedIt);
					mLayerList->erase(mLayerListIter);
					// 여기서부터 유효화지 않게 되니 아무짓도 하믄 안됨
				}
			}

			LayerList*	  mLayerList;
			LayerListIter mLayerListIter;
			ILayer*       mParentLayer;
		};

		
		virtual ~ILayer() {}

		virtual const uint32 getType() const = 0;
		virtual	const LayerID& getId() const = 0;

		virtual void setPosition(const Vector2& pos) = 0;
		virtual Vector2 getPosition() const = 0;


		virtual bool messageProcedure(const Message& msg) = 0;
		virtual const Matrix4& getMatrix() = 0;

		// 
//		virtual void update(real elapsedTime) = 0;

		// 상태를 변화 시키지 않아야 나중에 랜더링 쓰레드로 뺄수 있다 const가 필수
		virtual void render() const = 0;

	
		virtual void  setLayerListInfo(const ListInfo& listInfo) = 0;
		virtual const ListInfo& getLayerListInfo() const = 0;


		virtual ISprite* getSprite() { return NULL; }
		virtual ILayer* getParentLayer() const = 0;

		virtual void registerFontDraw(const IFontDraw* pFontDraw) const {}

		virtual LayerListIterWrapper getChild() = 0;

		virtual void pushBackChild(const SP_Layer& spLayer) = 0;


		virtual void setVisibility(bool isVisibility) = 0;
		virtual bool isVisible() const = 0;
		virtual void deleteChildAll() = 0;
		virtual void destroy() = 0;

		virtual void enableFirstClickEvent(bool enable) = 0;
//		virtual bool isEnableFirstClickEvent() const = 0;

		virtual SP_Layer getThis() const = 0;

		
		bool isRoot() const { return getParentLayer() == NULL; }

		static void resetFirstEventLayer() { msFirstClickEventLayer = NULL; }
		static void setFirstClickEventLayer(ILayer* pLayer) 
		{
			if( !msFirstClickEventLayer )
				msFirstClickEventLayer = pLayer; 
		}

		static ILayer* getFirstEventSprite() { return msFirstClickEventLayer; }

	public:
		static void		setModalLayer( const WP_Layer& wpLayer ) { msModalLayer = wpLayer; }
		static WP_Layer getModalLayer() { return msModalLayer; }

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		// LayerImpl 용

		virtual void updateStaticLogic(real ) {}
//		virtual bool updateThread(real ) { return false; }
		virtual Matrix4 makeCacheMatrix() { return Matrix4::IDENTITY; }
		virtual void makeAabb() {}

		virtual void invalidCacheMatrix() {}

	private:
		static ILayer*		   msFirstClickEventLayer;

		static WP_Layer		   msModalLayer;
	};




	class LayerImpl;
	class LayerFactory : public Factory<LayerImpl>
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(LayerFactory);
		LayerFactory();
	public:
		static LayerFactory& getInstance();
	};

	template<class LAYER>
	mrcSharedPtr<LAYER> createChildLayer(LAYER* pParent,const LayerID& id)
	{
		mrcAutoPtr<ILayer> apLayer = LayerFactory::getInstance().createObject<LAYER>();
		mrcSharedPtr<LAYER> spLayer = sharedptr_static_cast<LAYER>(mrcSharedPtr<ILayer>(apLayer.release()));
		
		if(spLayer->setId(id) == false)
		{
			printLayers();
			mrcASSERT_MSG( false , "layer id is overloaded" );
		}

		pParent->pushBackChild(spLayer);
		return spLayer;
	}

	template<class LAYER,class SPRITE>
	mrcSharedPtr<LAYER> createChildLayer(LAYER* pParent,const LayerID& id = LayerID() )
	{
		mrcSharedPtr<LAYER> spLayer = createChildLayer<LAYER>(pParent,id);
		spLayer->attach( SpriteFactory::getInstance().createObject<SPRITE>() );
		return spLayer;
	}

	

}


#endif


