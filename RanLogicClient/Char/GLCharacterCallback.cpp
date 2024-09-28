#include "../pch.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../Enginelib/DxMeshs/SkinMesh/DxVehicle.h"

#include "../GLGaeaClient.h"
#include "./GLCharacter.h"

#include "../../SigmaCore/DebugInclude.h"

// 머리 색 콜백 (bjju) 
//-------------------------------------------------------------------------------------------------------//
void GLCharacter::CallbackHairColorCreate ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    UI::SCOLORPANNELGROUP_INFO sInfo;

    sInfo.strLABEL    = ID2GAMEINTEXT("HAIR_COLOR_LABEL");
    sInfo.dwNUMPANNEL = 1;
    OutPannelGroupInfoArray.push_back( sInfo );
}

void GLCharacter::CallbackHairColorInit ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    if ( OutPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    int nR = (int)((float)((m_wHairColor & 0x7c00) >> 10)*255.0f/31.0f);
    int nG = (int)((float)((m_wHairColor & 0x3e0)  >> 5 )*255.0f/31.0f);
    int nB = (int)((float)((m_wHairColor & 0x1f)        )*255.0f/31.0f);

    DWORD dwHairColor = 0xFF000000
                        | (nR << 16)
                        | (nG << 8)
                        | (nB);

    OutPannelGroupInfoArray[0].dwCOLOR[0] = dwHairColor;
}

void GLCharacter::CallbackHairColorUpdate ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
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
    HairColorChange( wTemp );
}

void GLCharacter::CallbackHairColorCommit ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
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
    ReqInvenHairColorChange( wTemp );
}

// 코스튬 색 콜백 (bjju) 
//-------------------------------------------------------------------------------------------------------//
void GLCharacter::CallbackCostumColorCreate ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    UI::SCOLORPANNELGROUP_INFO sInfo;

    sInfo.strLABEL    = ID2GAMEINTEXT("COSTUM_COLOR_LABEL");
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );
}

void GLCharacter::CallbackCostumColorInit ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
    if ( OutPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    if ( !GetSkinChar() )
    {
        return;
    }

    EMSLOT emCostumSelectSlot = GetCostumSelectSlot();
    PDXCHARPART pCharPart = GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot ) );

    if( pCharPart )
    {
        DWORD dwMainColor = 0, dwSubColor = 0;
        pCharPart->GetDefaultOverlayColor( dwMainColor , dwSubColor );

        if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
            dwSubColor  != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
        {
            OutPannelGroupInfoArray[0].dwCOLOR[0] = 0xFF000000 | dwMainColor;
            OutPannelGroupInfoArray[0].dwCOLOR[1] = 0xFF000000 | dwSubColor;
        }
    }
}

void GLCharacter::CallbackCostumColorUpdate ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( InPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    if ( !GetSkinChar() )
    {
        return;
    }

    EMSLOT emCostumSelectSlot = GetCostumSelectSlot();
    PDXCHARPART pCharPart = GetSkinChar()->GetPiece( SLOT_2_PIECE( emCostumSelectSlot) );

    if ( pCharPart )
    {
        DWORD dwMainColor = 0, dwSubColor = 0;
        pCharPart->GetDefaultOverlayColor( dwMainColor, dwSubColor );

        if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
        {
            pCharPart->SetHairColor( InPannelGroupInfoArray[0].dwCOLOR[0] );
        }

        if( dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
        {
            pCharPart->SetSubColor( InPannelGroupInfoArray[0].dwCOLOR[1] );
        }
    }
}

void GLCharacter::CallbackCostumColorCommit ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( InPannelGroupInfoArray.size() < 1 )
    {
        return;
    }

    DWORD dwMainColor = InPannelGroupInfoArray[0].dwCOLOR[0];
    DWORD dwSubColor  = InPannelGroupInfoArray[0].dwCOLOR[1];

    ReqInvenCostumColorChange( dwMainColor, dwSubColor );
}

// 탈것 색 콜백 (bjju) 
//-------------------------------------------------------------------------------------------------------//
void GLCharacter::CallbackVehicleColorCreate ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{	
    UI::SCOLORPANNELGROUP_INFO sInfo;

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );

    sInfo.strLABEL    = "";
    sInfo.dwNUMPANNEL = 2;
    OutPannelGroupInfoArray.push_back( sInfo );
}

void GLCharacter::CallbackVehicleColorInit ( UI::SPANNELGROUPINFOARRAY& OutPannelGroupInfoArray )
{
	if( VehicleColorChangeTextInit( OutPannelGroupInfoArray ) == false )
		return;

    if ( OutPannelGroupInfoArray.size() < (ACCE_TYPE_SIZE-ACCE_TYPE_PARTS_A) )
    {
        return;
    }
    DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;

    if( !pVehicle )
        return;

    int nIndex = 0;
    DWORD dwMainColor = 0, dwSubColor = 0;
    for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++, nIndex++ )
    {
        UI::SCOLORPANNELGROUP_INFO& sInfo = OutPannelGroupInfoArray[nIndex];

        PDXCHARPART pSkinPiece = pVehicle->GetVehiclePart( i );
        if( pSkinPiece )
        {
			// [shhan][2014.09.29] Overlay 색을 변경하다가 취소를 할 경우, 기존 셋팅값이 아닌, Piece 셋팅값이 나오는 문제 수정.
			//
			dwMainColor = pSkinPiece->GetHairColor();
			dwSubColor = pSkinPiece->GetSubColor();

			if( dwMainColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) && dwSubColor == D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
			{
				pSkinPiece->GetDefaultOverlayColor( dwMainColor , dwSubColor );
			}


			// 값이 잘 들어가 있으면 셋팅.
			if( dwMainColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) &&
				dwSubColor != D3DCOLOR_RGBA ( 0, 0, 0, 0 ) )
            {
                sInfo.dwCOLOR[0] = 0xFF000000 | dwMainColor;
                sInfo.dwCOLOR[1] = 0xFF000000 | dwSubColor;
                sInfo.bENABLE = TRUE;
            }
            else
            {
                sInfo.bENABLE = FALSE;
            }
        }
        else
        {
            sInfo.bENABLE = FALSE;
        }
    }
}

bool GLCharacter::VehicleColorChangeTextInit( std::vector<UI::SCOLORPANNELGROUP_INFO>&   OutPannelGroupInfoArray )
{
	if( OutPannelGroupInfoArray.empty() )
		return false;

	switch( m_sVehicle.m_emTYPE )
	{
	case VEHICLE_TYPE_BIKE:
		for( unsigned int i=0; i<OutPannelGroupInfoArray.size(); ++i)
			OutPannelGroupInfoArray[i].strLABEL = ID2GAMEWORD("BIKE_COLOR_CHANGE_TEXT", i);
		return true;
	case VEHICLE_TYPE_CAR:
		for( unsigned int i=0; i<OutPannelGroupInfoArray.size(); ++i)
			OutPannelGroupInfoArray[i].strLABEL = ID2GAMEWORD("CAR_COLOR_CHANGE_TEXT", i);
		return true;
	case VEHICLE_TYPE_REARCAR:
		for( unsigned int i=0; i<OutPannelGroupInfoArray.size(); ++i)
			OutPannelGroupInfoArray[i].strLABEL = ID2GAMEWORD("REARCAR_COLOR_CHANGE_TEXT", i);
		return true;
	default:
		return false;
	}

	return false;
}

void GLCharacter::CallbackVehicleColorUpdate ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( InPannelGroupInfoArray.size() < (ACCE_TYPE_SIZE-ACCE_TYPE_PARTS_A) )
    {
        return;
    }

    DxVehicle* pVehicle = m_sVehicle.m_pSkinChar;

    if( !pVehicle )
        return;

    int nIndex = 0;
    DWORD dwMainColor = 0, dwSubColor = 0;
    for ( INT i = ACCE_TYPE_PARTS_A; i < ACCE_TYPE_SIZE; i++, nIndex++ )
    {
        const UI::SCOLORPANNELGROUP_INFO& sInfo = InPannelGroupInfoArray[nIndex];

        if ( !sInfo.bENABLE )
        {
            continue;
        }

        PDXCHARPART pSkinPiece = pVehicle->GetVehiclePart( i );

        DWORD dwMainColor = sInfo.dwCOLOR[0];
        DWORD dwSubColor  = sInfo.dwCOLOR[1];

        if( pSkinPiece )
        {
            if( m_emVehicle == EMVEHICLE_DRIVER )
            {
                pSkinPiece->SetHairColor( dwMainColor, FALSE );
                pSkinPiece->SetSubColor( dwSubColor, TRUE );
            }
        }
    }
}

void GLCharacter::CallbackVehicleColorCommit ( const UI::SPANNELGROUPINFOARRAY& InPannelGroupInfoArray )
{
    if ( InPannelGroupInfoArray.size() < (ACCE_TYPE_SIZE-ACCE_TYPE_PARTS_A) )
    {
        return;
    }

    DWORD dwMainColor [ ACCE_TYPE_SIZE ];
    DWORD dwSubColor  [ ACCE_TYPE_SIZE ];

    memset(dwMainColor, 0, sizeof(dwMainColor));
    memset(dwSubColor,  0, sizeof(dwSubColor));

    int nIndex = 0;
    for( INT i = 1; i < ACCE_TYPE_SIZE; i++, nIndex++ )
    {
        const UI::SCOLORPANNELGROUP_INFO& sInfo = InPannelGroupInfoArray[nIndex];

        if ( sInfo.bENABLE )
        {
            dwMainColor[ i ] = sInfo.dwCOLOR[0];
            dwSubColor [ i ] = sInfo.dwCOLOR[1];
        }
    }
    
    ReqInvenVehicleColorChange( dwMainColor, dwSubColor );
}