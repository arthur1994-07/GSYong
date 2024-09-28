
#include "mrcPrecompiled.h"
#include "mrcSprite.h"
#include "mrcAssert.h"
#include "mrcTextureData.h"
#include "mrcSpriteAnimation.h"

namespace mrc
{

	ISprite* ISprite::msMouseEnterSprite = NULL;
	ISprite* ISprite::msMouseDragSprite = NULL;

	//////////////////////////////////////////////////////////////////////////

	SpriteFactory& SpriteFactory::getInstance()
	{
		typedef Loki::SingletonHolder<SpriteFactory,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieIndependencyMgr > SpriteFactoryS;

		return SpriteFactoryS::Instance();
	}

	class AnimOneIntervalData;
	class AnimMultiIntervalData;
	class AnimNoData;

	class SpriteAnimOneInterval;
	class SpriteAnimMultiInterval;
	class SpriteAnimNo;

	MIRACLE_REGISTER_FACTORY(SpriteAnimationFactory,SpriteAnimOneInterval,AnimOneIntervalData);
	MIRACLE_REGISTER_FACTORY(SpriteAnimationFactory,SpriteAnimMultiInterval,AnimMultiIntervalData);
	MIRACLE_REGISTER_FACTORY(SpriteAnimationFactory,SpriteAnimNo,AnimNoData);

	SpriteAnimationFactory& SpriteAnimationFactory::getInstance()
	{
		typedef Loki::SingletonHolder<SpriteAnimationFactory,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieIndependencyMgr > SpriteAnimationFactoryS;

		return SpriteAnimationFactoryS::Instance();
	}

}