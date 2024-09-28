#include "../pch.h"

#include "./GLBuffManagerAG.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/gassert.h"

#include "../../RanLogic/GLUseFeatures.h"

#include "../TriggerManager/GLTriggerManagerAG.h"

#include "../AgentServer/GLAgentServer.h"
#include "../Character/GLCharAG.h"

#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

using namespace buffmanager;
using namespace Sqrat;

// ----------------------------------------------------------------------------

GLBuffManagerAG::GLBuffManagerAG( GLAgentServer* pAgentServer )
: m_pAgentServer( pAgentServer )
, m_nKey_CTF_End(0)
{
    //Blank
}

GLBuffManagerAG::~GLBuffManagerAG()
{
    using namespace triggermanager;

    GLTriggerManagerAG::GetInstance()
        .Get_Trigger_CTF_End().Erase( m_nKey_CTF_End );
}

void GLBuffManagerAG::ToDo_BindTrigger()
{
    using namespace triggermanager;

    m_nKey_CTF_End = GLTriggerManagerAG::GetInstance()
        .Get_Trigger_CTF_End().Register( boost::bind(&GLBuffManagerAG::Trigger_CTF_End, this, _1) );
}

void GLBuffManagerAG::Trigger_CTF_End( ENUM_SCHOOL emVictorySchool )
{
    if ( !GLUseFeatures::GetInstance().IsUsingCaptureTheField() )
    {
        return;
    }

    if ( emVictorySchool == SCHOOL_NONE )
    {
        m_pAgentServer->ClearCTFRewardBuff();
        return;
    }

    //! 점령전 승리 보상 버프 리스트를 가져온다.
    VECBUFF vecCTFBuffReward;
    Sqrat::Array arrCTFBuffReward = m_fnSQGetCTFBuffReward.Evaluate<Sqrat::Array>();
    Sqrat::GetVectorFromArray( arrCTFBuffReward, &vecCTFBuffReward );

    //! CTF에 세팅한다.
    m_pAgentServer->ClearCTFRewardBuff();

    for ( VECBUFF_ITER iter=vecCTFBuffReward.begin(); iter!=vecCTFBuffReward.end(); ++iter )
    {
        SQBuff& sqBuff = (*iter);
        m_pAgentServer->AddCTFRewardBuff( sqBuff.sID, sqBuff.wLEVEL );
    }  
}
