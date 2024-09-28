#include "pch.h"
#include "GLLandMan.h"

/*LandPoolField* const LandPoolField::getInstance(void)
{
    static LandPoolField _instance;
    return &_instance;
}*/
LandPoolField::LandPoolField(void)
	: landPool(_T("GLLandMan"))
	, landManagerPool(_T("GLLandManager"))
{
	::InitializeCriticalSection(&this->csObjectPoolLand);
	::InitializeCriticalSection(&this->csObjectPoolLandManager);
}

LandPoolField::~LandPoolField(void)
{
    ::DeleteCriticalSection(&this->csObjectPoolLand);
	::DeleteCriticalSection(&this->csObjectPoolLandManager);
}

GLLandMan* const LandPoolField::newLand(GLGaeaServer* const _pGaeaServer, GLLandManager* const _pLandManager, const SNATIVEID& _baseMapID, const SNATIVEID& _instanceMapID)
{	
    LandPoolField::_lockLand();	
    GLLandMan* const _pNewLand(this->landPool.construct(_pGaeaServer, _pLandManager, _baseMapID));
    if ( _pNewLand != 0  )
    {
		_pNewLand->SetGaeaMapID(_instanceMapID);
        LandPoolField::_unlockLand();

		//sc::writeLogInfo(
		//	sc::string::format("LandPoolField::newLand MapID : %1%/%2%, InstanceMapID : %3%", _baseMapID.Mid(), _baseMapID.Sid(), _instanceMapID.Id() ) );

        return _pNewLand;
    }

    this->landPool.set_next_size(32);
    GLLandMan* const _pReallocLand(this->landPool.construct(_pGaeaServer, _pLandManager, _baseMapID));
	_pReallocLand->SetGaeaMapID(_instanceMapID);
    LandPoolField::_unlockLand();

	//sc::writeLogInfo(
	//	sc::string::format("LandPoolField::newLand(Realloc) MapID : %1%/%2%, InstanceMapID : %3%", _baseMapID.Mid(), _baseMapID.Sid(), _instanceMapID.Id() ) );

    return _pReallocLand;
}
GLLandMan* const LandPoolField::newLand(const GLLandMan* const _pSourceLand, GLLandManager* const _pLandManager, const SNATIVEID& _instanceMapID)
{
    LandPoolField::_lockLand();	
    GLLandMan* const _pNewLand(this->landPool.construct(*_pSourceLand, _pLandManager));
    if ( _pNewLand != 0  )
    {		
		_pNewLand->SetGaeaMapID(_instanceMapID);
        LandPoolField::_unlockLand();

		//sc::writeLogInfo(
		//	sc::string::format("LandPoolField::newLand InstanceMapID : %1%", _instanceMapID.Id() ) );

        return _pNewLand;
    }

    this->landPool.set_next_size(32);
    GLLandMan* const _pReallocLand(this->landPool.construct(*_pSourceLand, _pLandManager));	
	_pReallocLand->SetGaeaMapID(_instanceMapID);
    LandPoolField::_unlockLand();

	//sc::writeLogInfo(
	//	sc::string::format("LandPoolField::newLand(Realloc) InstanceMapID : %1%", _instanceMapID.Id() ) );

    return _pReallocLand;
}
GLLandManager* const LandPoolField::newLandManager(GLGaeaServer* const _pGaeaServer, const unsigned int nInstanceMapID, const unsigned int _nBaseMapID)
{
    LandPoolField::_lockLandManager();	
    GLLandManager* const _pLandManager(this->landManagerPool.construct(_pGaeaServer, nInstanceMapID, _nBaseMapID));
    if ( _pLandManager != 0  )
    {
        LandPoolField::_unlockLandManager();

		//sc::writeLogInfo(
		//	sc::string::format("LandPoolField::newLandManager MapID : %1%, InstanceMapID : %2%", _nBaseMapID, nInstanceMapID ) );

        return _pLandManager;
    }

    this->landPool.set_next_size(32);
    GLLandManager* const _pReallocLandManager(this->landManagerPool.construct(_pGaeaServer, nInstanceMapID, _nBaseMapID));
    LandPoolField::_unlockLandManager();

	//sc::writeLogInfo(
	//	sc::string::format("LandPoolField::newLandManager(Realloc) MapID : %1%, InstanceMapID : %2%", _nBaseMapID, nInstanceMapID ) );

    return _pReallocLandManager;
}

void LandPoolField::release(GLLandMan* const _pLand)
{
    if ( _pLand == 0 )
        return;

	SNATIVEID baseMapID = _pLand->GetBaseMapID();
	SNATIVEID instanceMapID = _pLand->GetGaeaMapID();

    LandPoolField::_lockLand();
    this->landPool.destroy(_pLand);
    LandPoolField::_unlockLand();

	//sc::writeLogInfo(
	//	sc::string::format("LandPoolField::release(Land) MapID : %1%/%2%, InstanceMapID : %3%/%4%", baseMapID.Mid(), baseMapID.Sid(), instanceMapID.Mid(), instanceMapID.Sid() ) );
}
void LandPoolField::release(GLLandManager* const _pLandManager)
{
    if ( _pLandManager == 0 )
        return;

	unsigned int mapID = _pLandManager->getBaseMapMainID();
	unsigned int instanceMapID = _pLandManager->getInstanceMapMainID();

    LandPoolField::_lockLandManager();
    this->landManagerPool.destroy(_pLandManager);
    LandPoolField::_unlockLandManager();

	//sc::writeLogInfo(
	//	sc::string::format("LandPoolField::release(LandManager) MapID : %1%, InstanceMapID : %2%", mapID, instanceMapID ) );
}
