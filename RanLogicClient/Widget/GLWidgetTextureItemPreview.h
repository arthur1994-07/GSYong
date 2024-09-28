#pragma once

#include "../../enginelib/DxMeshs/DxAniKeys.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Item/GLItemDef.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "./GLWidgetTextureChar.h"



class DxSkinChar;
struct GLCHARLOGIC;

namespace ItemPreview
{
	class GLItemPreviewClient;
}

// 캐릭터 미리보기
class GLWidgetTextureItemPreview : public GLWidgetTextureChar
{
public :
	GLWidgetTextureItemPreview( LPDIRECT3DDEVICEQ pDevice, ItemPreview::GLItemPreviewClient* pGaeaClient );
	virtual ~GLWidgetTextureItemPreview(void);

private :
	ItemPreview::GLItemPreviewClient* m_pItemPreviewClient;

	SITEMCUSTOM	m_sItemCustomArray[GLWidgetTextureChar::EMPREVIEWSLOT_SIZE];
	float	m_fTurnSpeed;

public :
	virtual bool OnCreate();
	virtual void OnFrameMove( float fElapsedTime );

protected :
	const EMSLOT Item_2_Slot ( const SITEMCUSTOM& sItemCustom );
	const EMSLOT Item_2_Slot( const SITEM* pItem );
	GLWidgetTextureChar::EMPREVIEWSLOT Slot_2_PreviewSlot ( const EMSLOT emSlot );

	void TurnLeft( float fElapsedTime );
	void TurnRight( float fElapsedTime );

	void SetSlotItem( const GLWidgetTextureChar::EMPREVIEWSLOT emSlot, const SITEMCUSTOM& sItemCustom );

public :
	void SetSkinChar( GLCHARLOGIC* pChar );
	void SetPreviewItem( const SITEMCUSTOM& sItemCustom );
	void ResetPreviewItem();

	void MoveViewPosZ( float fValue );
};