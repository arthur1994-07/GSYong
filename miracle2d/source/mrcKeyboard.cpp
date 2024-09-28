
#include "mrcPrecompiled.h"
#include "mrcKeyboard.h"

namespace mrc
{
	Keyboard& mrc::Keyboard::getInstance()
	{
		typedef Loki::SingletonHolder<Keyboard,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieDependencyMgr > KeyboardS;

		return KeyboardS::Instance();
	}

}
