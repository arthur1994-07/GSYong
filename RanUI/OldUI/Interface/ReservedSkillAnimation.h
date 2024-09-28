#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include "../../InnerInterface.h"

//-----------------------------------------------------------------------------------------------//

#include <string>
#include <list>

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CSkillImage;

class CReservedSkillAnimation : public CUIGroup
{
public:
	struct SKILLOBJECT : public CUIGroup
	{
		static float fMAX_REMAIN_TIME;
		static float fMAX_EFFECT_TIME;
		static float fMAX_MOVING_TIME;
		static DWORD dwDEFAULT_COLOR;


		SNATIVEID	 sID;
		WORD		 wSLOT;

		CSkillImage* pIMAGE;
		CSkillImage* pIMAGE_EFFECT;
		bool		 bENABLE;

		float		 fANITIME;
		float		 fANITIMEEFFECT;

		GLGaeaClient*	 pGaeaClient;
		CInnerInterface* pInterface;
		EngineDeviceMan* pEngineDevice;

		UIRECT rcBeginRect;
		UIRECT rcEffectRect;
		UIRECT rcEndRect;

		SKILLOBJECT(GLGaeaClient* _pGaeaClient, CInnerInterface* _pInterface, EngineDeviceMan* _pEngineDevice)
			: CUIGroup(_pEngineDevice)
			, wSLOT(0)
			, pIMAGE(NULL)
			, pIMAGE_EFFECT(NULL)
			, bENABLE(false)
			, fANITIME(0.0f)
			, fANITIMEEFFECT(0.0f)
			, pGaeaClient(_pGaeaClient)
			, pInterface(_pInterface)
			, pEngineDevice(_pEngineDevice)
		{
		}

		void  CreateSubControl ();
		void  Reset( const SNATIVEID& sSKillID );
		void  SetEnable( const bool bEnable );
		const float GetDefaultPosX( const LONG lResolution );
		const float GetDefaultPosY( const LONG lResolution );

		virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	};

	typedef std::list<SKILLOBJECT*> LIST_SKILLOBJ;
	typedef LIST_SKILLOBJ::iterator LIST_SKILLOBJ_ITER;

public:
	CReservedSkillAnimation(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CReservedSkillAnimation ();

public:
	void                        CreateSubControl ();

public:
	void						Show ( const SNATIVEID& sSkillID );
	void						HideAll();
//protected:
//	virtual void                SetVisibleSingle( BOOL bVisible );
//
//public:
//	void                        Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText );
//	void                        Begin ();
//	void                        Hide ();
//	void                        Update ();
//
//private:
//	void                        ResizeControl ( const UI::EMPROGRESSDISPLAY_TYPE emType );
//	void                        SetPositionAlign ( const std::string& strControl );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	LIST_SKILLOBJ				m_listSkillObject;

private:
	LPDIRECT3DDEVICEQ			m_pd3dDevice;

protected:
	CInnerInterface*            m_pInterface;
	GLGaeaClient*               m_pGaeaClient;
};

//----------------------------------------------------------------------------------------------------//

class MyReservedSkillAnimation : public IReservedSkillAnimation, private CReservedSkillAnimation
{
public:
	MyReservedSkillAnimation( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyReservedSkillAnimation()    {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void Show ( const SNATIVEID& sSkillID );
	virtual void HideAll () ;

};
