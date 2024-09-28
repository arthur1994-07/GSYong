#include "../pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./GLPetClient.h"
#include "../GLGaeaClient.h"

#include "../../SigmaCore/DebugInclude.h"

// Æê Ä®¶ó ÄÝ¹é (bjju)
//-------------------------------------------------------------------------------------------------------//
void GLPetClient::CallbackPetColorCreate ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    UI::SCOLORPANNELGROUP_INFO sInfo;

    sInfo.strLABEL    = ID2GAMEINTEXT("PET_COLOR_LABEL");
    sInfo.dwNUMPANNEL = 1;
    OutPannelGroupInfoArray.push_back( sInfo );
}

void GLPetClient::CallbackPetColorInit ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    if ( OutPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    int nR = (int)((float)((m_wColor & 0x7c00) >> 10)*255.0f/31.0f);
    int nG = (int)((float)((m_wColor & 0x3e0)  >> 5 )*255.0f/31.0f);
    int nB = (int)((float)((m_wColor & 0x1f)        )*255.0f/31.0f);

    DWORD dwColor = 0xFF000000
        | (nR << 16)
        | (nG << 8)
        | (nB);

    OutPannelGroupInfoArray[0].dwCOLOR[0] = dwColor;
}

void GLPetClient::CallbackPetColorUpdate ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( InPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    DWORD dwColor = InPannelGroupInfoArray[0].dwCOLOR[0];

    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    nR = (int)((float)nR/255.0f * 31.0f);
    nG = (int)((float)nG/255.0f * 31.0f);
    nB = (int)((float)nB/255.0f * 31.0f);

    WORD wTemp = nR << 10 | nG << 5 | nB;
    ColorChange( wTemp );
}

void GLPetClient::CallbackPetColorCommit ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( !m_pOwner )
    {
        return;
    }

    if ( InPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    DWORD dwColor = InPannelGroupInfoArray[0].dwCOLOR[0];

    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    nR = (int)((float)nR/255.0f * 31.0f);
    nG = (int)((float)nG/255.0f * 31.0f);
    nB = (int)((float)nB/255.0f * 31.0f);

    WORD wTemp = nR << 10 | nG << 5 | nB;
    ReqChangeColor( wTemp );
}
