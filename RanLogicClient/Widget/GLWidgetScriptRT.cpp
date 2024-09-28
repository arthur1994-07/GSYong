#include "pch.h"
#include "GLWidgetScript.h"

#include "./GLWidgetTextureChar.h"
#include "./GLWidgetTextureCharVehicle.h"
#include "./GLWidgetTextureDice.h"
#include "./GLWidgetTextureItemPreview.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"

void GLWidgetScript::UpdateDiceRT(void)
{
	WidgetTexture::Dice* const _pTexture((WidgetTexture::Dice*)GLWidgetTextureMan::GetInstance().FindTexture(EMWIDGET_TEXTURE_DICE));
	if ( _pTexture == NULL )
		return;

	_pTexture->setManagerMiniGame(GLWidgetScript::GetInstance().m_pGaeaClient->GetMiniGameManager());	
}
void GLWidgetScript::UpdateCharInfoRT()
{
	/*GLWidgetTextureChar* pTexture = 
		(GLWidgetTextureChar*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_CHARINFO );
	if( NULL == pTexture )
		return;

	pTexture->SetSkinChar( GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter() );*/
}

void GLWidgetScript::UpdateCharVehicleInfoRT( WORD wMid, WORD wSid )
{
	/*GLWidgetTextureCharVehicle* pTexture = 
		(GLWidgetTextureCharVehicle*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_CHAR_VEHICLEINFO );
	if( NULL == pTexture )
		return;

	GLCharacter* pCharacter = GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter();
	pTexture->SetSkinChar( pCharacter );

	if( wMid == SNATIVEID::ID_NULL && wSid == SNATIVEID::ID_NULL)
		return;

	GLWidgetTextureCharVehicle::VehicleData vehicleData;
	SNATIVEID sID(wMid,wSid);
	vehicleData.VehicleID = sID;

	if( GetVehicleType(sID) == VEHICLE_TYPE_BOARD )
		pTexture->SetAni(AN_PLACID, AN_SUB_HOVERBOARD );
	else
		pTexture->SetAni(GetMainAniType(sID), AN_SUB_DRIVER_WALK);
		

	SNATIVEID curMyVehicleId = pCharacter->GET_SLOT_ITEM(SLOT_VEHICLE).Id();
	if( vehicleData.VehicleID == curMyVehicleId )
	{
		for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
		{
			vehicleData.Parts[i] = pCharacter->VehiclePutOnItem(VEHICLE_ACCESSORYTYPE(i));
			vehicleData.ColorArray[i].dwMainColor = pCharacter->VehicleColorMain(VEHICLE_ACCESSORYTYPE(i));
			vehicleData.ColorArray[i].dwSubColor = pCharacter->VehicleColorSub(VEHICLE_ACCESSORYTYPE(i));
		}
	}

	pTexture->SetSkinVehicle( vehicleData );*/
}

void GLWidgetScript::UpdateItemPreviewRT()
{
	GLWidgetTextureItemPreview* pTexture = 
		(GLWidgetTextureItemPreview*) GLWidgetTextureMan::GetInstance().FindTexture( EMWIDGET_TEXTURE_ITEM_PREVIEW );
	if( NULL == pTexture )
		return;

	pTexture->SetSkinChar( GLWidgetScript::GetInstance().m_pGaeaClient->GetCharacter() );
}