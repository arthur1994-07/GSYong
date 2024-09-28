#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "./CrowAction.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SCROWACTION::SCROWACTION()
    : m_dwActFlag( NULL )
    , m_bAfterFall_NoBody( FALSE )
    , m_bRun( FALSE )
    , m_emMoveType( MOVE_LAND )
    , m_fDriftHeight( 0.0f )
    , m_wBodyRadius( 4 )
    , m_fWalkVelo( 12.0f )
    , m_fRunVelo( 34.0f )
    , m_fLiveTime( 0.0f )
    , m_bMobLink( FALSE )
    , m_sMobLinkID( NATIVEID_NULL() )
    , m_fMobLinkScale( 0.0f )
    , m_fMobLinkDelay( 0.0f )
{
}

SCROWACTION::SCROWACTION(const SCROWACTION& value)
{
    operator = (value);
}

SCROWACTION& SCROWACTION::operator= (const SCROWACTION_100 &OldData)
{
    m_wBodyRadius			= OldData.m_wBodyRadius;
    m_strSkinObj			= OldData.m_szSkinObj;
    m_strTalkFile			= OldData.m_szTalkFile;

    m_strSaleFile[0]		= OldData.m_szSaleFile;

    m_bAfterFall_NoBody		= OldData.m_bAfterFall_NoBody;

    m_strFallingEffect		= OldData.m_szFallingEffect;
    m_strBlowEffect			= OldData.m_szBlowEffect;

    m_emMoveType			= OldData.m_emMoveType;
    m_fDriftHeight			= OldData.m_fDriftHeight;

    m_fWalkVelo				= OldData.m_fWalkVelo;
    m_bRun					= OldData.m_bRun;
    m_fRunVelo				= OldData.m_fRunVelo;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_101 &OldData )
{
    m_wBodyRadius			= OldData.m_wBodyRadius;
    m_strSkinObj			= OldData.m_szSkinObj;
    m_strTalkFile			= OldData.m_szTalkFile;

    m_dwActFlag				= OldData.m_dwActFlag;
    m_strSaleFile[0]		= OldData.m_szSaleFile;
    //m_szSaleFile2;
    //m_szSaleFile3;

    m_bAfterFall_NoBody		= OldData.m_bAfterFall_NoBody;
    m_strBirthEffect		= OldData.m_szBirthEffect;
    m_strFallingEffect		= OldData.m_szFallingEffect;
    m_strBlowEffect			= OldData.m_szBlowEffect;

    m_emMoveType			= OldData.m_emMoveType;
    m_fDriftHeight			= OldData.m_fDriftHeight;

    m_fWalkVelo				= OldData.m_fWalkVelo;
    m_bRun					= OldData.m_bRun;
    m_fRunVelo				= OldData.m_fRunVelo;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_102 &OldData )
{
    m_wBodyRadius		 = OldData.m_wBodyRadius;
    m_strSkinObj		 = OldData.m_szSkinObj;
    m_strTalkFile		 = OldData.m_szTalkFile;

    m_dwActFlag			 = OldData.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = OldData.m_szSaleFile[i];

    m_bAfterFall_NoBody  = OldData.m_bAfterFall_NoBody;
    m_strBirthEffect	 = OldData.m_szBirthEffect;
    m_strFallingEffect   = OldData.m_szFallingEffect;
    m_strBlowEffect		 = OldData.m_szBlowEffect;

    m_emMoveType		 = OldData.m_emMoveType;
    m_fDriftHeight		 = OldData.m_fDriftHeight;

    m_fWalkVelo			 = OldData.m_fWalkVelo;
    m_bRun				 = OldData.m_bRun;
    m_fRunVelo			 = OldData.m_fRunVelo;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_103 &OldData )
{
    m_wBodyRadius		= OldData.m_wBodyRadius;
    m_strSkinObj		= OldData.m_strSkinObj;
    m_strTalkFile		= OldData.m_strTalkFile;

    m_dwActFlag = OldData.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = OldData.m_strSaleFile[i];

    m_bAfterFall_NoBody = OldData.m_bAfterFall_NoBody;
    m_strBirthEffect	= OldData.m_strBirthEffect;
    m_strFallingEffect	= OldData.m_strFallingEffect;
    m_strBlowEffect		= OldData.m_strBlowEffect;

    m_emMoveType		= OldData.m_emMoveType;
    m_fDriftHeight		= OldData.m_fDriftHeight;

    m_fWalkVelo			= OldData.m_fWalkVelo;
    m_bRun				= OldData.m_bRun;
    m_fRunVelo			= OldData.m_fRunVelo;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_104 &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_105 &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= FALSE;
    m_sMobLinkID		= NATIVEID_NULL();

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_106 &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= value.m_bMobLink;
    m_sMobLinkID		= value.m_sMobLinkID;

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_107 &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= value.m_bMobLink;
    m_sMobLinkID		= value.m_sMobLinkID;

    m_vecPatternList	= value.m_vecPatternList;

    return *this;
}


SCROWACTION& SCROWACTION::operator= ( const SCROWACTION_108 &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= value.m_bMobLink;
    m_sMobLinkID		= value.m_sMobLinkID;
    m_fMobLinkScale		= value.m_fMobLinkScale;
    m_fMobLinkDelay		= value.m_fMobLinkDelay;

    m_vecPatternList	= value.m_vecPatternList;

    return *this;
}

SCROWACTION& SCROWACTION::operator= ( const SCROWACTION &value )
{
    m_wBodyRadius		 = value.m_wBodyRadius;
    m_strSkinObj		 = value.m_strSkinObj;
    m_strTalkFile		 = value.m_strTalkFile;

    m_dwActFlag = value.m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        m_strSaleFile[i] = value.m_strSaleFile[i];

    m_bAfterFall_NoBody = value.m_bAfterFall_NoBody;
    m_strBirthEffect    = value.m_strBirthEffect;
    m_strFallingEffect  = value.m_strFallingEffect;
    m_strBlowEffect		= value.m_strBlowEffect;

    m_emMoveType		= value.m_emMoveType;
    m_fDriftHeight		= value.m_fDriftHeight;

    m_fWalkVelo			= value.m_fWalkVelo;
    m_bRun				= value.m_bRun;
    m_fRunVelo			= value.m_fRunVelo;
    m_fLiveTime			= value.m_fLiveTime;

    m_bMobLink			= value.m_bMobLink;
    m_sMobLinkID		= value.m_sMobLinkID;
    m_fMobLinkScale		= value.m_fMobLinkScale;
    m_fMobLinkDelay		= value.m_fMobLinkDelay;

    m_vecPatternList	= value.m_vecPatternList;

    return *this;
}

bool SCROWACTION::LOAD103(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i=0; i<SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA;
    SCROWPATTERN sCrowTemp;
    sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
    sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
    sCrowTemp.m_fPatternDNRate = 90;
    m_vecPatternList.push_back( sCrowTemp );

    SFile >> dwDATA; 


    SFile >> dwDATA; m_emMoveType = (EMMOVETYPE) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;

    return true;
}

bool SCROWACTION::LOAD104(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA;
    SCROWPATTERN sCrowTemp;
    sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
    sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
    sCrowTemp.m_fPatternDNRate = 90;
    m_vecPatternList.push_back( sCrowTemp );

    SFile >> dwDATA; 

    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;
    SFile >> m_fLiveTime;

    return true;
}

bool SCROWACTION::LOAD105(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA;
    SCROWPATTERN sCrowTemp;
    sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
    sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
    sCrowTemp.m_fPatternDNRate = 90;
    m_vecPatternList.push_back( sCrowTemp );

    SFile >> dwDATA; 

    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;
    SFile >> m_fLiveTime;

    float fActionDNRate;	//	사용하지 않는 변수

    SFile >> fActionDNRate;

    return true;
}

bool SCROWACTION::LOAD106(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA;
    SCROWPATTERN sCrowTemp;
    sCrowTemp.m_emActPattern = ( EMCROWACT_UP ) dwDATA;
    sCrowTemp.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
    sCrowTemp.m_fPatternDNRate = 90;
    m_vecPatternList.push_back( sCrowTemp );

    SFile >> dwDATA; 

    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;
    SFile >> m_fLiveTime;

    float fActionDNRate;	//	사용하지 않는 변수

    SFile >> fActionDNRate;

    SFile >> m_bMobLink;
    SFile >> m_sMobLinkID.dwID;

    return true;
}

bool SCROWACTION::LOAD107(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;
    SFile >> m_fLiveTime;

    float fActionDNRate;	//	사용하지 않는 변수

    SFile >> fActionDNRate;

    SFile >> m_bMobLink;
    SFile >> m_sMobLinkID.dwID;

    SCROWPATTERN sPattern;
    DWORD dwNUM(0);
    SFile >> dwNUM;
    for( DWORD i = 0; i < dwNUM; i++ )
    {
        SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
        //	예전 랜덤셋
        if( sPattern.m_dwPatternAttackSet == 3 )	sPattern.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
        m_vecPatternList.push_back ( sPattern );
    }

    return true;
}

bool SCROWACTION::LOAD108(sc::BaseStream& SFile)
{
    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_dwActFlag;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    DWORD dwDATA;
    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;
    SFile >> m_fLiveTime;

    float fActionDNRate;	//	사용하지 않는 변수

    SFile >> fActionDNRate;

    SFile >> m_bMobLink;
    SFile >> m_sMobLinkID.dwID;
    SFile >> m_fMobLinkScale;
    SFile >> m_fMobLinkDelay;

    SCROWPATTERN sPattern;
    DWORD dwNUM(0);
    SFile >> dwNUM;
    for( DWORD i = 0; i < dwNUM; i++ )
    {
        SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
        if( sPattern.m_dwPatternAttackSet == 3 )	sPattern.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
        m_vecPatternList.push_back ( sPattern );
    }

    return true;
}


bool SCROWACTION::LOAD109(sc::BaseStream& SFile)
{

    SFile >> m_dwActFlag;

    DWORD dwDATA;
    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;

    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_fLiveTime;

    SFile >> m_bMobLink;
    SFile >> m_sMobLinkID.dwID;
    SFile >> m_fMobLinkDelay;
    SFile >> m_fMobLinkScale;

    SCROWPATTERN sPattern;
    DWORD dwNUM(0);
    SFile >> dwNUM;
    for( DWORD i = 0; i < dwNUM; i++ )
    {
        SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
        if( sPattern.m_dwPatternAttackSet == 3 )	sPattern.m_dwPatternAttackSet = SCROWPATTERN::RANDOMPATTERNNUM;
        m_vecPatternList.push_back ( sPattern );
    }

    return true;
}

bool SCROWACTION::LOAD(sc::BaseStream& SFile)
{

    SFile >> m_dwActFlag;

    DWORD dwDATA;
    SFile >> dwDATA; m_emMoveType = ( EMMOVETYPE ) dwDATA;
    SFile >> m_fDriftHeight;

    SFile >> m_fWalkVelo;
    SFile >> m_bRun;
    SFile >> m_fRunVelo;

    SFile >> m_wBodyRadius;
    SFile >> m_strSkinObj;
    SFile >> m_strTalkFile;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    SFile >> m_bAfterFall_NoBody;
    SFile >> m_strBirthEffect;
    SFile >> m_strFallingEffect;
    SFile >> m_strBlowEffect;

    for ( int i = 0; i < SALENUM; ++i )
        SFile >> m_strSaleFile[i];

    SFile >> m_fLiveTime;

    SFile >> m_bMobLink;
    SFile >> m_sMobLinkID.dwID;
    SFile >> m_fMobLinkDelay;
    SFile >> m_fMobLinkScale;

    SCROWPATTERN sPattern;
    DWORD dwNUM(0);
    SFile >> dwNUM;
    for( DWORD i = 0; i < dwNUM; i++ )
    {
        SFile.ReadBuffer ( &sPattern, sizeof( SCROWPATTERN ) );
        m_vecPatternList.push_back ( sPattern );
    }

    return true;
}

bool SCROWACTION::SAVE(sc::SerialFile& SFile)
{
    SFile << m_dwActFlag;

    SFile << m_emMoveType;
    SFile << m_fDriftHeight;

    SFile << m_fWalkVelo;
    SFile << m_bRun;
    SFile << m_fRunVelo;

    SFile << m_wBodyRadius;
    SFile << m_strSkinObj;
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.
    SFile << m_strTalkFile;

    SFile << m_bAfterFall_NoBody;
    SFile << m_strBirthEffect;
    SFile << m_strFallingEffect;
    SFile << m_strBlowEffect;

    for ( int i = 0; i < SALENUM; ++i )
        SFile << m_strSaleFile[i];

    SFile << m_fLiveTime;

    SFile << m_bMobLink;
    SFile << m_sMobLinkID.dwID;
    SFile << m_fMobLinkDelay;
    SFile << m_fMobLinkScale;

    SFile << static_cast<int>(m_vecPatternList.size());
    if ( m_vecPatternList.size() > 0 )
    {
        SFile.WriteBuffer ( &(m_vecPatternList[0]), DWORD( sizeof( SCROWPATTERN ) * m_vecPatternList.size() ) );
    }

    return true;
}

VOID SCROWACTION::SaveCsvHead ( std::fstream &SFile )
{
    SFile << "wBodyRadius" << ",";

    SFile << "strSkinObj" << ",";
    SFile << "strTalkFile" << ",";

    SFile << "dwActFlag" << ",";
    for( int i=0; i<SALENUM; ++i )
        SFile << "strSaleFile" << i << ",";

    SFile << "bAfterFall_NoBody" << ",";

    SFile << "strBirthEffect" << ",";
    SFile << "strFallingEffect" << ",";
    SFile << "strBlowEffect" << ",";

    SFile << "emMoveType" << ",";
    SFile << "fDriftHeight" << ",";

    SFile << "fWalkVelo" << ",";
    SFile << "bRun" << ",";
    SFile << "fRunVelo" << ",";
    SFile << "m_fLiveTime" << ",";

    SFile << "m_bMobLink" << ",";
    SFile << "m_sMobLinkID wMainID" << ",";
    SFile << "m_sMobLinkID wSubID" << ",";
    SFile << "m_fMobLinkScale" << ",";
    SFile << "m_fMobLinkDelay" << ",";

    for( int i = 0; i < PATTERNNUM; i++ )
    {
        SFile << "m_fPatternDNRate" << i << ",";
        SFile << "m_emPatternDN" << i << ",";
        SFile << "m_dwPatternAttackSet" << i << ",";
    }
}

VOID SCROWACTION::SaveCsv ( std::fstream &SFile )
{
    SFile << m_wBodyRadius << ",";

    STRUTIL::OutputStrCsv( SFile, m_strSkinObj );
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.
    STRUTIL::OutputStrCsv( SFile, m_strTalkFile );

    SFile << m_dwActFlag << ",";
    for( int i=0; i<SALENUM; ++i )
        STRUTIL::OutputStrCsv( SFile, m_strSaleFile[i] );

    SFile << m_bAfterFall_NoBody << ",";

    STRUTIL::OutputStrCsv( SFile, m_strBirthEffect );
    STRUTIL::OutputStrCsv( SFile, m_strFallingEffect );
    STRUTIL::OutputStrCsv( SFile, m_strBlowEffect );

    SFile << m_emMoveType << ",";
    SFile << m_fDriftHeight << ",";

    SFile << m_fWalkVelo << ",";
    SFile << m_bRun << ",";
    SFile << m_fRunVelo << ",";
    SFile << m_fLiveTime << ",";

    SFile << m_bMobLink << ",";
    SFile << m_sMobLinkID.wMainID << ",";
    SFile << m_sMobLinkID.wSubID << ",";
    SFile << m_fMobLinkScale << ",";
    SFile << m_fMobLinkDelay << ",";

    const int iMaxSize = ( int ) PATTERNNUM;
    int iSize = ( int ) m_vecPatternList.size();

    if( iSize > iMaxSize )
    {
        iSize = iMaxSize;
        MessageBox( NULL, _T("몬스터 AI는 최대 10개까지만 저장합니다."), _T("ERROR"), MB_OK );
    }

    for( int i = 0; i < iMaxSize; i++ )
    {
        if( i < iSize )
        {
            SFile << m_vecPatternList[ i ].m_fPatternDNRate << ",";
            SFile << m_vecPatternList[ i ].m_emActPattern << ",";
            SFile << m_vecPatternList[ i ].m_dwPatternAttackSet << ",";
        }
        else
        {
            SFile << "0" << ",";
            SFile << "0" << ",";
            SFile << "0" << ",";
        }
    }
}

VOID SCROWACTION::LoadCsv(CStringArray& StrArray, int& iCsvCur)
{
    m_wBodyRadius = ( WORD ) atoi(StrArray[ iCsvCur++ ]);

    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strSkinObj );
    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strTalkFile );
    sc::string::lowerString(m_strTalkFile); // 소문자로 바꾼다.

    m_dwActFlag = ( DWORD ) atol( StrArray[ iCsvCur++ ] );
    for( int i = 0; i < SALENUM; ++i )
        STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strSaleFile[i] );

    m_bAfterFall_NoBody = ( BOOL ) atol( StrArray[ iCsvCur++ ] );

    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strBirthEffect );
    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strFallingEffect );
    STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], m_strBlowEffect );

    m_emMoveType = ( EMMOVETYPE ) atoi( StrArray[ iCsvCur++ ] );
    m_fDriftHeight = ( float ) atof( StrArray[ iCsvCur++ ] );

    m_fWalkVelo = ( float ) atof( StrArray[ iCsvCur++ ] );
    m_bRun = ( BOOL ) atol( StrArray[ iCsvCur++ ] );
    m_fRunVelo = ( float ) atof( StrArray[ iCsvCur++ ] );
    m_fLiveTime = ( float ) atof( StrArray[ iCsvCur++ ] );

    m_bMobLink = ( BOOL ) atol( StrArray[ iCsvCur++ ] );
    m_sMobLinkID.wMainID = ( WORD ) atoi( StrArray[ iCsvCur++ ] );
    m_sMobLinkID.wSubID = ( WORD ) atoi( StrArray[ iCsvCur++ ] );
    m_fMobLinkScale = ( float ) atof( StrArray[ iCsvCur++ ] );
    m_fMobLinkDelay = ( float ) atof( StrArray[ iCsvCur++ ] );

    const int iMaxSize = ( int ) PATTERNNUM;

    m_vecPatternList.clear();

    for( int i = 0; i < iMaxSize; i++ )
    {
        SCROWPATTERN sPattern;
        sPattern.m_fPatternDNRate = ( float ) atof( StrArray[ iCsvCur++ ] );
        sPattern.m_emActPattern = ( EMCROWACT_UP ) atoi( StrArray[ iCsvCur++ ] );
        sPattern.m_dwPatternAttackSet = atoi( StrArray[ iCsvCur++ ] );

        if( sPattern.m_fPatternDNRate == 0.0f &&
            sPattern.m_emActPattern == EMCROWACT_UP_IDLE &&
            sPattern.m_dwPatternAttackSet == 0 )
            continue;

        m_vecPatternList.push_back( sPattern );
    }
}
