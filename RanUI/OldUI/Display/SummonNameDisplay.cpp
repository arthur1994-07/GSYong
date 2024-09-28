#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxEx.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"

#include "../../../RanLogic/RANPARAM.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"

#include "../../InnerInterface.h"
#include "SummonNameDisplay.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSummonNameDisplay::CSummonNameDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pNameBox     (NULL)
    , m_pNameLineBox (NULL)

    , m_bUsedMemPool(false)
    , m_bDISP       (true)
    , m_dwCOUNT     (0)
    //, m_pSummon     (NULL)
    , m_dwGlobID    (UINT_MAX)
    , m_bSimple		( FALSE )

    , m_dwCOLOR(NS_UITEXTCOLOR::DEFAULT)
    , m_emPARTY(NODATA)
    , m_dwTYPE(TYPE_NORMAL)
    , m_vPOS(0,0,0)
{

	memset(m_strSummonName, 0, sizeof(char) * (CHAR_SZNAME));
	memset(m_strOwnerName,  0, sizeof(char) * (CHAR_SZNAME));
}

CSummonNameDisplay::~CSummonNameDisplay ()
{
}

void CSummonNameDisplay::INITIALIZE()
{
	m_bDISP = true;

	m_pNameBox->ClearText();
	m_pNameBox->SetLocalPos( m_rcNameBox );
}

void CSummonNameDisplay::CreateSubControl ()
{
	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicLineBoxEx* pLineBox = new CBasicLineBoxEx(m_pEngineDevice);
	pLineBox->CreateSub ( this, "BASIC_LINE_BOX_EX_NAME", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pLineBox->CreateBaseBoxName ( "SUMMON_BASIC_NAME_LINE_BOX" );
	RegisterControl ( pLineBox );
	m_pNameLineBox = pLineBox;

	CBasicProgressBar* pHP = new CBasicProgressBar(m_pEngineDevice);
	pHP->CreateSub ( this, "SUMMON_HP_IMAGE" );
	pHP->CreateOverImage  ( "SUMMON_HP_OVERIMAGE" );
	pHP->SetControlNameEx ( "SUMMON HP 프로그래스바" );
	RegisterControl ( pHP );
	m_pHP = pHP;

	CBasicTextBox* pNameBox = new CBasicTextBox(m_pEngineDevice);
	pNameBox->CreateSub ( this, "SUMMON_NAME_DISPLAY_NAMEBOX", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	pNameBox->SetFont ( pFont8 );
	pNameBox->SetPartInterval ( 1.0f );
	pNameBox->SetLineInterval ( 0.0f );
	pNameBox->SetTextAlign ( TEXT_ALIGN_CENTER_X );
	RegisterControl ( pNameBox );
	m_pNameBox = pNameBox;

	m_rcNameBox = m_pNameBox->GetLocalPos(); // MEMO : 기본 로컬포지션 저장
}

CUIControl*	CSummonNameDisplay::CreateControl ( const char* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl, wAlignFlag );
	RegisterControl ( pControl );
	return pControl;
}

BOOL CSummonNameDisplay::SetSummmon(std::tr1::shared_ptr<GLSummonClient> spSummon)
{
	if (!spSummon) 
		return FALSE;

	m_spSummon = spSummon;

	// Note : 이름표시 높이조절
	m_vPOS    = spSummon->GetPosBodyHeight();

	// Note : 주인의 이름과 소환수의 이름과 기타 정보를 구한다
	const char* pOwnerName = NULL;
	const char* pName      = spSummon->GETNAME();
	{
		m_dwCOLOR = NS_UITEXTCOLOR::DEFAULT;

		std::tr1::shared_ptr<GLCharClient> spChar = m_pGaeaClient->GetChar( spSummon->m_dwOwnerGaeaID );
		if ( spChar )
		{
			pOwnerName = spChar->GetName();

			// CROWREN의 정보를 구한다
			CROWREN TempCrowRen;
			TempCrowRen.SetData(m_pGaeaClient, spChar.get());

			m_emPARTY = TempCrowRen.m_emPARTY;
			m_dwTYPE  = TempCrowRen.m_dwTYPE;
			m_dwCOLOR = TempCrowRen.m_dwCOLOR;
		}
		else
		{
			GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

			if ( pCharacter && pCharacter->GetGaeaID() == spSummon->m_dwOwnerGaeaID )
			{
				pOwnerName = pCharacter->GetName();

				// CROWREN의 정보를 구한다
				CROWREN TempCrowRen;
				TempCrowRen.SetData(m_pGaeaClient, pCharacter);

				m_emPARTY = TempCrowRen.m_emPARTY;
				m_dwTYPE  = TempCrowRen.m_dwTYPE;
				m_dwCOLOR = TempCrowRen.m_dwCOLOR;
			}
		}
	}

	m_pNameBox->ClearText();

	m_bSimple = RANPARAM::bSIMPLE_SUM_NAME;
	if ( m_bSimple )
	{
		m_pHP->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pHP->SetVisibleSingle( TRUE );
	}

	SetOwnerName ( pOwnerName, m_dwCOLOR );
	SetName      ( pName,      m_dwCOLOR );
	
	RESIZE ();

	return TRUE;
}

void CSummonNameDisplay::SetName ( const CString& strName, const D3DCOLOR& dwColor )
{
	if ( strName[0] == NULL )
		return;

	_tcscpy_s( m_strSummonName, CHAR_SZNAME, (const char *)strName ); 

	m_pNameBox->AddTextNoSplit ( strName, dwColor );

	/////////////long lLineSize = m_pNameBox->GetLongestLine ();

	/*
	if ( PARTY_MASTER == emnPartyMark || PARTY_MASTER_TARGET == emnPartyMark || PARTY_MASTER_GENERAL == emnPartyMark )
	{
		m_pPartyMark[emnPartyMark]->SetVisibleSingle ( TRUE );

		if ( m_bClub )
		{
			const CBasicTextBox::STEXTPART& sPART1 = m_pNameBox->GetTextPart ( 0 );
			const CBasicTextBox::STEXTPART& sPART2 = m_pNameBox->GetTextPart ( 1 );

			long Line1Size = sPART1.dequeLine[0].strSize.cx;
			long Line2Size = long(sPART2.dequeLine[0].strSize.cx + fPartyMark);

			if ( Line2Size > Line1Size )
			{
				lLineSize += long ( fPartyMark );
			}
		}
		else
		{
			lLineSize += long ( fPartyMark );
		}	
	}
	*/

	///////////float fGapX = GetControlVec.sizeX - float(lLineSize);
	////////////float fCLUBSIZE = 0.0f;
	
	/* 소환자의 이름이 아닐까 예상 
	if ( m_bClub )
	{
		fCLUBSIZE = m_pNameBox->CalcMaxHEIGHT(0,0) + 2.0f;
	}
	*/

	///////////const UIRECT& rcLocalPosOld = GetLocalPos ();
	///////////UIRECT        rcLocalPosNew( rcLocalPosOld.left, rcLocalPosOld.top, rcLocalPosOld.sizeX - fGapX + 2.0f, rcLocalPosOld.sizeY + fCLUBSIZE );

	///////////AlignSubControl ( rcLocalPosOld, rcLocalPosNew );
	///////////SetLocalPos ( rcLocalPosNew );

	/*
	if ( m_bClub )
	{
		if ( (NODATA != emnPartyMark) && (emnPartyMark < nPARTYMARK) )
		{
			float fRIGHT = 0.0f;

			const UIRECT& rcLocalPos = m_pNameBox->GetLocalPos ();
			const float fHEIGHT = m_pNameBox->CalcMaxHEIGHT ( 0, 2 );

			m_pNameBox->SetGlobalPos ( UIRECT ( rcLocalPos.left, rcLocalPos.top, rcLocalPos.sizeX, fHEIGHT ) );
			m_pNameBox->SetLocalPos ( UIRECT ( rcLocalPos.left, rcLocalPos.top, rcLocalPos.sizeX, fHEIGHT ) );

			if ( m_pNameBox->GetCount () > 1 )
			{
				UIRECT rcPart;
				m_pNameBox->GetPartInfo ( 1, rcPart );
				fRIGHT = rcPart.right;
			}
			else if ( m_pNameBox->GetCount () == 1 )
			{
				UIRECT rcPart;
				m_pNameBox->GetPartInfo ( 0, rcPart );
				fRIGHT = rcPart.right;
			}
			else
			{
				GASSERT ( 0 && "CNameDisplay::SetName(), m_pNameBox->GetCount <= 0" );
				return ;
			}

			const UIRECT& rcClubMark = m_pClubMark->GetLocalPos();
			const UIRECT& rcMark = m_pPartyMark[emnPartyMark]->GetLocalPos();
			m_pPartyMark[emnPartyMark]->SetLocalPos ( D3DXVECTOR2(fRIGHT, rcMark.top + rcClubMark.sizeY + fLineInterval + 1.0f) );
		}
	}
	*/
}


void CSummonNameDisplay::SetOwnerName( const CString& strName, const D3DCOLOR& dwColor )
{
	if ( strName[0] == NULL )
		return;

	_tcscpy_s( m_strOwnerName, CHAR_SZNAME, (const char *)strName ); 

	m_pNameBox->AddTextNoSplit( strName, dwColor );

	//m_bClub = true; // MEMO : 클럽으로 설정
}

HRESULT CSummonNameDisplay::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT( pd3dDevice );

	CUIGroup::Render ( pd3dDevice );

	return S_OK;
}

void CSummonNameDisplay::UPDATE ( DWORD _dwCOUNT )
{
	if (!m_spSummon)
		return;

	m_dwCOUNT = _dwCOUNT;

	if ( !m_bSimple )
	{
		// Note : 체력 조절
		if (m_spSummon->IsValidBody() && (m_spSummon->GETMAXHP() != 0))
		{
			m_pHP->SetPercent ( (FLOAT)m_spSummon->GETHP() / (FLOAT)m_spSummon->GETMAXHP() );
		}
		else
		{
			m_pHP->SetPercent (0.0f);
		}
	}

	// Note : 이름표시 높이조절
	m_vPOS    = m_spSummon->GetPosBodyHeight();
}

void CSummonNameDisplay::RESIZE ()
{
	//const UIRECT& GetControlVec = m_pNameBox->GetLocalPos ();
	//const float&  fLineInterval = m_pNameBox->GetPartInterval ();
	//float         fGapX         = GetControlVec.sizeX - float(lLineSize);
	DWORD         dwLineCount      = (DWORD)m_pNameBox->GetCount ();
	long          lLineWidth       = m_pNameBox->GetLongestLine ();
	long          lHPWidth         = (m_bSimple) ? 0 : (long)m_pHP->GetLocalPos().sizeX;

	// Note : 높이를 가져온다.
	long lHeight = (long) m_pNameBox->CalcMaxHEIGHT(0, dwLineCount);
	
	if ( !m_bSimple )
	{
		lHeight += (long)m_pHP->GetLocalPos().sizeY;
	}

	// Note : 가장 큰 Width를 찾는다.
	long lLongestWidth  = (lLineWidth < lHPWidth) ? lHPWidth : lLineWidth;

	// Note : 텍스트 박스크기로 리사이징
	const UIRECT& rcNameBoxLocalPos = m_pNameBox->GetLocalPos();
	const UIRECT& rcNameBoxNew      = UIRECT ( rcNameBoxLocalPos.left,                 rcNameBoxLocalPos.top,
	                                           rcNameBoxLocalPos.left + lLongestWidth, rcNameBoxLocalPos.top + lHeight + 4.0f );

	AlignSubControl ( rcNameBoxLocalPos, rcNameBoxNew );

	//Note : 현재 컨트롤 리사이징
	const UIRECT& rcLocalPosOld = GetLocalPos ();
/*
	UIRECT        rcLocalPosNew = UIRECT ( rcLocalPosOld.left, rcLocalPosOld.top,
		                                   rcLocalPosOld.sizeX + (lLongestWidth - rcNameBoxLocalPos.sizeX), rcNameBoxLocalPos.top + lHeight );
*/
	SetLocalPos  ( rcLocalPosOld );
	SetGlobalPos ( rcLocalPosOld );

}

bool CSummonNameDisplay::DIFFERENT ( std::tr1::shared_ptr<GLSummonClient> spSummon )
{
	if (!m_spSummon || !spSummon)
		return TRUE;

	if (m_bSimple != RANPARAM::bSIMPLE_SUM_NAME)
		return TRUE;

	CROWREN TempCrowRen;
	TempCrowRen.SetData(m_pGaeaClient, spSummon.get());

	if (m_dwCOLOR != TempCrowRen.m_dwCOLOR)
        return TRUE;
	if (m_emPARTY != TempCrowRen.m_emPARTY)
        return TRUE;
	if (m_dwTYPE  != TempCrowRen.m_dwTYPE)
        return TRUE;

	return FALSE;
}

