

#include "mrcPrecompiled.h"
#include "mrcSqLayerMgr.h"

#include "mrcUtil/mrcSingleton.h"
#include "mrcLayerMgr.h"
#include "mrcSqLayer.h"
#include "../../mrcLayerImpl.h"

namespace mrc
{
	// MIRICALE_SCRIPT_TEST() 메크로 패스를 위한 --; 
	static inline bool isValid() { return true; }

	SqLayerMgr& SqLayerMgr::getInstance()
	{
		typedef Loki::SingletonHolder<SqLayerMgr,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieDependencyMgr > SqLayerMgrS;

		return SqLayerMgrS::Instance();
	}


	mrc::SqLayer SqLayerMgr::createLayer( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayerImpl = sharedptr_static_cast<LayerImpl>(LayerMgr::getInstance().createLayer(id)); 
		return SqLayer(spLayerImpl);
	}

	SqLayer SqLayerMgr::createLayerWithSpriteDefault( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayerImpl = LayerMgr::getInstance().createLayer<SpriteDefault>(id);
		return SqLayer(spLayerImpl);
	}

	SqLayer SqLayerMgr::createLayerWithSpriteButton( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayerImpl = LayerMgr::getInstance().createLayer<SpriteButton>(id);
		return SqLayer(spLayerImpl);
	}

	SqLayer SqLayerMgr::createLayerWithSpriteNumber( const LayerID& id )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		SP_LayerImpl spLayerImpl = LayerMgr::getInstance().createLayer<SpriteNumber>(id);
		return SqLayer(spLayerImpl);
	}

	void SqLayerMgr::moveBeforeLayer( const SqLayer& targetLayer,const SqLayer& moveLayer )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		LayerMgr::getInstance().moveBeforeLayer(targetLayer.getPointer(),moveLayer.getPointer());
	}

	void SqLayerMgr::moveAfterLayer( const SqLayer& targetLayer,const SqLayer& moveLayer )
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();	
		LayerMgr::getInstance().moveAfterLayer(targetLayer.getPointer(),moveLayer.getPointer());
	}

	void SqLayerMgr::clear()
	{
		MIRICALE_SCRIPT_TEST_CLASS_FUNC();
		LayerMgr::getInstance().clear();
	}

	
}

