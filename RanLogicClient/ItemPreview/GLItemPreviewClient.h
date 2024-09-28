#pragma once

#include "../../SigmaCore/Lua/lua_tinker.h"
#include "../RanLogicClient/Widget/GLWidgetTextureItemPreview.h"

class GLGaeaClient;

namespace ItemPreview
{
	class GLItemPreviewClient
	{
	public:
		GLItemPreviewClient(GLGaeaClient* const pGaeaClient);
		~GLItemPreviewClient(void);

	private:
		GLGaeaClient* m_pGaeaClient;

	private:
		// 처음 UI 오픈시 저장;
		SITEMCUSTOM m_sPreItemCustom;

		float	m_fTurnSpeed;
		BOOL    m_bTurnLeft;
		BOOL    m_bTurnRight;

	public :
		float m_fMinViewPosZ;
		float m_fMaxViewPosZ;

	public:
		void OpenItemPreview( const SITEMCUSTOM& sItemCustom );

		void SetTurnLeft( bool bTurnLeft ){ m_bTurnLeft = bTurnLeft; }
		void SetTurnRight( bool bTurnRight ){ m_bTurnRight = bTurnRight; }

		float GetTurnSpeed() { return m_fTurnSpeed;}
		bool IsTurnLeft() { return m_bTurnLeft; }
		bool IsTurnRight() { return m_bTurnRight; }

		void MoveViewPosZ( float fValue );

		void SetPreItem();
		void SetItem( const SITEMCUSTOM& sItemCustom );

		bool IsEnableItemPreview( const SITEM* pItem );
	};
}