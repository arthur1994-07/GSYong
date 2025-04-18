#include "StdAfx.h"
#include "../RanLogic/RANPARAM.h"
#include "../enginelib/DxTools/RENDERPARAM.h"
#include "../enginelib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/charset.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/Database/Sqlite/SQLite.h"
#include "../RanLogic/GLUseFeatures.h"
#include <boost/scoped_array.hpp>
#include <Imm.h>
#include "LauncherText.h"
#include "LauncherID.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLauncherTextMan& CLauncherTextMan::GetInstance()
{
	static CLauncherTextMan Instance;
	return Instance;
}

CLauncherTextMan::CLauncherTextMan ()
{
	memset( m_szPath, 0, sizeof( m_szPath ) );
}

CLauncherTextMan::~CLauncherTextMan ()
{
    Clear ();
}

void CLauncherTextMan::Clear ()
{
	NS_TEXT_LOADER::ClearText ( m_LauncherText.mapFlags );
}

BOOL CLauncherTextMan::LoadText ( const char* szFileName,BOOL bXML )
{
	CString strCountry = GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE ? RANPARAM::strOriginalCountry:RANPARAM::strCountry;
	m_emLang = language::StringToProvidelang(strCountry);
	CString strPath(m_szPath);
	strPath += szFileName;

	Clear();
	DWORD dwLoadop = NS_TEXT_LOADER::NS_TEXT_LOADOP_SERVEREXE|NS_TEXT_LOADER::NS_TEXT_LOADOP_ALLLANG;
	dwLoadop |= bXML == TRUE ? NS_TEXT_LOADER::NS_TEXT_LOADOP_REFACT : 0;
	NS_TEXT_LOADER::LoadText( strPath.GetString(), SLAUNCHERTEXT::progVersion, m_LauncherText.mapFlags, strCountry, dwLoadop);
	
	DefaultSetText();

	return TRUE;
}

BOOL CLauncherTextMan::DefaultSetText()
{

	UINT szID[szID_IDS_MESSAGE78+1];
	std::string strID[strID_IDS_MESSAGE+1];

	szID[szID_ABOUT_BOX_ID_OK]			=	ABOUT_BOX_ID_OK;
	szID[szID_DOWNAREA_ID_OK]			=	DOWNAREA_ID_OK;
	szID[szID_GAMESET_ID_CANCEL]		=	GAMESET_ID_CANCEL;
	szID[szID_GAMESET_ID_OK]			=	GAMESET_ID_OK;
	szID[szID_IDC_AREA_STATIC]			=	IDC_AREA_STATIC;
	szID[szID_IDC_ATI_STATIC]			=	IDC_ATI_STATIC;
	szID[szID_IDC_BUTTON_EXIT]			=	IDC_BUTTON_EXIT;
	szID[szID_IDC_BUTTON_OPTION]		=	IDC_BUTTON_OPTION;
	szID[szID_IDC_BUTTON_PATCH]			=	IDC_BUTTON_PATCH;
	szID[szID_IDC_BUTTON_RETRY]			=	IDC_BUTTON_RETRY;
	szID[szID_IDC_BUTTON_START]			=	IDC_BUTTON_START;
	szID[szID_IDC_CHECK_NOCHECK]		=	IDC_CHECK_NOCHECK;
	szID[szID_IDC_CHECK_PASSIVE]		=	IDC_CHECK_PASSIVE;
	szID[szID_IDC_CHECK_USE_HTTP]		=	IDC_CHECK_USE_HTTP;
	szID[szID_IDC_CHECK_WINDOWMODE]		=	IDC_CHECK_WINDOWMODE;
	szID[szID_IDC_DISPLAY_STATIC]		=	IDC_DISPLAY_STATIC;
	szID[szID_IDC_DOWNAREA_STATIC]		=	IDC_DOWNAREA_STATIC;
	szID[szID_IDC_DOWNLOAD_STATIC]		=	IDC_DOWNLOAD_STATIC;
	szID[szID_IDC_NVIDIA_STATIC]		=	IDC_NVIDIA_STATIC;
	szID[szID_IDC_READYTOSTART]			=	IDC_READYTOSTART;
	szID[szID_IDC_STATIC_ATI_LINK]		=	IDC_STATIC_ATI_LINK;
	szID[szID_IDC_STATIC_NVIDIA_LINK]	=	IDC_STATIC_NVIDIA_LINK;
	szID[szID_IDC_TITLE_STATIC]			=	IDC_TITLE_STATIC;
	szID[szID_WARNING_ID_OK]			=	WARNING_ID_OK;
	szID[szID_IDD_GAMESETDLG]			=	IDD_GAMESETDLG;
	szID[szID_IDD_ABOUT_BOX]			=	IDD_ABOUT_BOX;
	szID[szID_IDD_DIALOG_DOWNLOAD_AREA]	=	IDD_DIALOG_DOWNLOAD_AREA;
	szID[szID_IDD_DIALOG_WARNING_WHQL]	=	IDD_DIALOG_WARNING_WHQL;
		
	szID[szID_IDC_VIDEO_STATIC]	=	IDC_VIDEO_STATIC;
	szID[szID_IDC_VER_STATIC]	=	IDC_VER_STATIC;
			   
	szID[szID_IDS_WHQL_CVD]		=	IDS_WHQL_CVD;
	szID[szID_IDS_WHQL_UCVD]	=	IDS_WHQL_UCVD;
	szID[szID_IDS_ABOUTBOX]		=	IDS_ABOUTBOX;

	szID[szID_IDC_STATIC_HOMEPAGE]	=	IDC_STATIC_HOMEPAGE;
	szID[szID_IDC_STATIC_DOWNLOAD]	=	IDC_STATIC_DOWNLOAD;
	szID[szID_IDC_STATIC_DOWNLOAD2]	=	IDC_STATIC_DOWNLOAD2;

			   
	szID[szID_IDC_GRAPHIC_OPTION0]	=	IDC_GRAPHIC_OPTION0;
	szID[szID_IDC_GRAPHIC_OPTION1]	=	IDC_GRAPHIC_OPTION1;
	szID[szID_IDC_GRAPHIC_OPTION2]	=	IDC_GRAPHIC_OPTION2;
	szID[szID_IDC_GRAPHIC_OPTION3]	=	IDC_GRAPHIC_OPTION3;
	szID[szID_IDC_GRAPHIC_OPTION4]	=	IDC_GRAPHIC_OPTION4;
	szID[szID_IDC_GRAPHIC_OPTION5]	=	IDC_GRAPHIC_OPTION5;
	szID[szID_IDC_GRAPHIC_OPTION6]	=	IDC_GRAPHIC_OPTION6;
	szID[szID_IDC_GRAPHIC_OPTION7]	=	IDC_GRAPHIC_OPTION7;
	szID[szID_IDC_GRAPHIC_OPTION8]	=	IDC_GRAPHIC_OPTION8;
	szID[szID_IDC_GRAPHIC_OPTION9]	=	IDC_GRAPHIC_OPTION9;
	szID[szID_IDC_GRAPHIC_OPTION10]	=	IDC_GRAPHIC_OPTION10;
	szID[szID_IDC_GRAPHIC_OPTION11]	=	IDC_GRAPHIC_OPTION11;
	szID[szID_IDC_GRAPHIC_OPTION12]	=	IDC_GRAPHIC_OPTION12;
	szID[szID_IDC_GRAPHIC_OPTION13]	=	IDC_GRAPHIC_OPTION13;
	szID[szID_IDC_GRAPHIC_OPTION14]	=	IDC_GRAPHIC_OPTION14;
	szID[szID_IDC_GRAPHIC_OPTION15]	=	IDC_GRAPHIC_OPTION15;
	szID[szID_IDC_GRAPHIC_OPTION16]	=	IDC_GRAPHIC_OPTION16;
	szID[szID_IDC_GRAPHIC_OPTION17]	=	IDC_GRAPHIC_OPTION17;
			   
	szID[szID_IDC_SOUND_OPTION0]	=	IDC_SOUND_OPTION0;
	szID[szID_IDC_SOUND_OPTION1]	=	IDC_SOUND_OPTION1;
	szID[szID_IDC_SOUND_OPTION2]	=	IDC_SOUND_OPTION2;
	szID[szID_IDC_SOUND_OPTION3]	=	IDC_SOUND_OPTION3;
	szID[szID_IDC_SOUND_OPTION4]	=	IDC_SOUND_OPTION4;
	szID[szID_IDC_SOUND_OPTION5]	=	IDC_SOUND_OPTION5;
			   
	szID[szID_IDS_MESSAGE0]		=	IDS_MESSAGE0;
	szID[szID_IDS_MESSAGE1]		=	IDS_MESSAGE1;
	szID[szID_IDS_MESSAGE2]		=	IDS_MESSAGE2;
	szID[szID_IDS_MESSAGE3]		=	IDS_MESSAGE3;
	szID[szID_IDS_MESSAGE4]		=	IDS_MESSAGE4;
	szID[szID_IDS_MESSAGE5]		=	IDS_MESSAGE5;
	szID[szID_IDS_MESSAGE6]		=	IDS_MESSAGE6;
	szID[szID_IDS_MESSAGE7]		=	IDS_MESSAGE7;
	szID[szID_IDS_MESSAGE8]		=	IDS_MESSAGE8;
	szID[szID_IDS_MESSAGE9]		=	IDS_MESSAGE9;
	szID[szID_IDS_MESSAGE10]	=	IDS_MESSAGE10;
	szID[szID_IDS_MESSAGE11]	=	IDS_MESSAGE11;
	szID[szID_IDS_MESSAGE12]	=	IDS_MESSAGE12;
	szID[szID_IDS_MESSAGE13]	=	IDS_MESSAGE13;
	szID[szID_IDS_MESSAGE14]	=	IDS_MESSAGE14;
	szID[szID_IDS_MESSAGE15]	=	IDS_MESSAGE15;
	szID[szID_IDS_MESSAGE16]	=	IDS_MESSAGE16;
	szID[szID_IDS_MESSAGE17]	=	IDS_MESSAGE17;
	szID[szID_IDS_MESSAGE18]	=	IDS_MESSAGE18;
	szID[szID_IDS_MESSAGE19]	=	IDS_MESSAGE19;
	szID[szID_IDS_MESSAGE20]	=	IDS_MESSAGE20;
	szID[szID_IDS_MESSAGE21]	=	IDS_MESSAGE21;
	szID[szID_IDS_MESSAGE22]	=	IDS_MESSAGE22;
	szID[szID_IDS_MESSAGE23]	=	IDS_MESSAGE23;
	szID[szID_IDS_MESSAGE24]	=	IDS_MESSAGE24;
	szID[szID_IDS_MESSAGE25]	=	IDS_MESSAGE25;
	szID[szID_IDS_MESSAGE26]	=	IDS_MESSAGE26;
	szID[szID_IDS_MESSAGE27]	=	IDS_MESSAGE27;
	szID[szID_IDS_MESSAGE28]	=	IDS_MESSAGE28;
	szID[szID_IDS_MESSAGE29]	=	IDS_MESSAGE29;
	szID[szID_IDS_MESSAGE30]	=	IDS_MESSAGE30;
	szID[szID_IDS_MESSAGE31]	=	IDS_MESSAGE31;
	szID[szID_IDS_MESSAGE32]	=	IDS_MESSAGE32;
	szID[szID_IDS_MESSAGE33]	=	IDS_MESSAGE33;
	szID[szID_IDS_MESSAGE34]	=	IDS_MESSAGE34;
	szID[szID_IDS_MESSAGE35]	=	IDS_MESSAGE35;
	szID[szID_IDS_MESSAGE36]	=	IDS_MESSAGE36;
	szID[szID_IDS_MESSAGE37]	=	IDS_MESSAGE37;
	szID[szID_IDS_MESSAGE38]	=	IDS_MESSAGE38;
	szID[szID_IDS_MESSAGE39]	=	IDS_MESSAGE39;
	szID[szID_IDS_MESSAGE40]	=	IDS_MESSAGE40;
	szID[szID_IDS_MESSAGE41]	=	IDS_MESSAGE41;
	szID[szID_IDS_MESSAGE42]	=	IDS_MESSAGE42;
	szID[szID_IDS_MESSAGE43]	=	IDS_MESSAGE43;
	szID[szID_IDS_MESSAGE44]	=	IDS_MESSAGE44;
	szID[szID_IDS_MESSAGE45]	=	IDS_MESSAGE45;
	szID[szID_IDS_MESSAGE46]	=	IDS_MESSAGE46;
	szID[szID_IDS_MESSAGE47]	=	IDS_MESSAGE47;
	szID[szID_IDS_MESSAGE48]	=	IDS_MESSAGE48;
	szID[szID_IDS_MESSAGE49]	=	IDS_MESSAGE49;
	szID[szID_IDS_MESSAGE50]	=	IDS_MESSAGE50;
	szID[szID_IDS_MESSAGE51]	=	IDS_MESSAGE51;
	szID[szID_IDS_MESSAGE52]	=	IDS_MESSAGE52;
	szID[szID_IDS_MESSAGE53]	=	IDS_MESSAGE53;
	szID[szID_IDS_MESSAGE54]	=	IDS_MESSAGE54;
	szID[szID_IDS_MESSAGE55]	=	IDS_MESSAGE55;
	szID[szID_IDS_MESSAGE56]	=	IDS_MESSAGE56;
	szID[szID_IDS_MESSAGE57]	=	IDS_MESSAGE57;
	szID[szID_IDS_MESSAGE58]	=	IDS_MESSAGE58;
	szID[szID_IDS_MESSAGE59]	=	IDS_MESSAGE59;
	szID[szID_IDS_MESSAGE60]	=	IDS_MESSAGE60;
	szID[szID_IDS_MESSAGE61]	=	IDS_MESSAGE61;
	szID[szID_IDS_MESSAGE62]	=	IDS_MESSAGE62;
	szID[szID_IDS_MESSAGE63]	=	IDS_MESSAGE63;
	szID[szID_IDS_MESSAGE64]	=	IDS_MESSAGE64;
	szID[szID_IDS_MESSAGE65]	=	IDS_MESSAGE65;
	szID[szID_IDS_MESSAGE66]	=	IDS_MESSAGE66;
	szID[szID_IDS_MESSAGE67]	=	IDS_MESSAGE67;
	szID[szID_IDS_MESSAGE68]	=	IDS_MESSAGE68;
	szID[szID_IDS_MESSAGE69]	=	IDS_MESSAGE69;
	szID[szID_IDS_MESSAGE70]	=	IDS_MESSAGE70;
	szID[szID_IDS_MESSAGE71]	=	IDS_MESSAGE71;
	szID[szID_IDS_MESSAGE72]	=	IDS_MESSAGE72;
	szID[szID_IDS_MESSAGE73]	=	IDS_MESSAGE73;
	szID[szID_IDS_MESSAGE74]	=	IDS_MESSAGE74;
	szID[szID_IDS_MESSAGE75]	=	IDS_MESSAGE75;
	szID[szID_IDS_MESSAGE76]	=	IDS_MESSAGE76;
	szID[szID_IDS_MESSAGE77]	=	IDS_MESSAGE77;
	szID[szID_IDS_MESSAGE78]	=	IDS_MESSAGE78;

	strID[strID_ABOUT_BOX_ID_OK]			=	"ABOUT_BOX_ID_OK";
	strID[strID_DOWNAREA_ID_OK]				=	"DOWNAREA_ID_OK";
	strID[strID_GAMESET_ID_CANCEL]			=	"GAMESET_ID_CANCEL";
	strID[strID_GAMESET_ID_OK]				=	"GAMESET_ID_OK";
	strID[strID_IDC_AREA_STATIC]			=	"IDC_AREA_STATIC";
	strID[strID_IDC_ATI_STATIC]				=	"IDC_ATI_STATIC";
	strID[strID_IDC_BUTTON_EXIT]			=	"IDC_BUTTON_EXIT";
	strID[strID_IDC_BUTTON_OPTION]			=	"IDC_BUTTON_OPTION";
	strID[strID_IDC_BUTTON_PATCH]			=	"IDC_BUTTON_PATCH";
	strID[strID_IDC_BUTTON_RETRY]			=	"IDC_BUTTON_RETRY";
	strID[strID_IDC_BUTTON_START]			=	"IDC_BUTTON_START";
	strID[strID_IDC_CHECK_NOCHECK]			=	"IDC_CHECK_NOCHECK";
	strID[strID_IDC_CHECK_PASSIVE]			=	"IDC_CHECK_PASSIVE";
	strID[strID_IDC_CHECK_USE_HTTP]			=	"IDC_CHECK_USE_HTTP";
	strID[strID_IDC_CHECK_WINDOWMODE]		=	"IDC_CHECK_WINDOWMODE";
	strID[strID_IDC_DISPLAY_STATIC]			=	"IDC_DISPLAY_STATIC";
	strID[strID_IDC_DOWNAREA_STATIC]		=	"IDC_DOWNAREA_STATIC";
	strID[strID_IDC_DOWNLOAD_STATIC]		=	"IDC_DOWNLOAD_STATIC";
	strID[strID_IDC_NVIDIA_STATIC]			=	"IDC_NVIDIA_STATIC";
	strID[strID_IDC_READYTOSTART]			=	"IDC_READYTOSTART";
	strID[strID_IDC_STATIC_ATI_LINK]		=	"IDC_STATIC_ATI_LINK";
	strID[strID_IDC_STATIC_NVIDIA_LINK]		=	"IDC_STATIC_NVIDIA_LINK";
	strID[strID_IDC_TITLE_STATIC]			=	"IDC_TITLE_STATIC";
	strID[strID_WARNING_ID_OK]				=	"WARNING_ID_OK";
	strID[strID_IDD_GAMESETDLG]				=	"IDD_GAMESETDLG";
	strID[strID_IDD_ABOUT_BOX]				=	"IDD_ABOUT_BOX";
	strID[strID_IDD_DIALOG_DOWNLOAD_AREA]	=	"IDD_DIALOG_DOWNLOAD_AREA";
	strID[strID_IDD_DIALOG_WARNING_WHQL]	=	"IDD_DIALOG_WARNING_WHQL";
	strID[strID_IDC_VIDEO_STATIC]			=	"IDC_VIDEO_STATIC";
	strID[strID_IDC_VER_STATIC]				=	"IDC_VER_STATIC";
	strID[strID_IDS_WHQL_CVD]				=	"IDS_WHQL_CVD";
	strID[strID_IDS_WHQL_UCVD]				=	"IDS_WHQL_UCVD";
	strID[strID_IDS_ABOUTBOX]				=	"IDS_ABOUTBOX";

	strID[strID_IDC_STATIC_HOMEPAGE]		=	"IDC_STATIC_HOMEPAGE";
	strID[strID_IDC_STATIC_DOWNLOAD]		=	"IDC_STATIC_DOWNLOAD";
	strID[strID_IDC_STATIC_DOWNLOAD2]		=	"IDC_STATIC_DOWNLOAD2";
					  
	strID[strID_IDC_GRAPHIC_OPTION]			=	"IDC_GRAPHIC_OPTION";
	strID[strID_IDC_SOUND_OPTION]			=	"IDC_SOUND_OPTION";
	strID[strID_IDS_MESSAGE]				=	"IDS_MESSAGE";

	CString strBuff;
	CStringArray* strArray;
	int i;
	

	// SENTENSE
	for ( i = szID_ABOUT_BOX_ID_OK; i<= szID_IDC_STATIC_DOWNLOAD2; ++i)
	{
		if ( !CheckText( strID[i] ) )
		{
			strArray = new CStringArray;
			strBuff.LoadString(szID[i]);
			strArray->Add(strBuff);
			m_LauncherText.mapFlags[m_emLang][strID[i]] = strArray;			
		}
	}

	// WORD
	// 弊贰侨 可记
	NS_TEXT_LOADER::STRFLAG_IT pos = m_LauncherText.mapFlags[m_emLang].find( strID[strID_IDC_GRAPHIC_OPTION] );
	NS_TEXT_LOADER::STRFLAG_IT end = m_LauncherText.mapFlags[m_emLang].end();
	if ( pos == end ) strArray = new CStringArray;
	else strArray = (*pos).second;

	for ( i = szID_IDC_GRAPHIC_OPTION0; i<= szID_IDC_GRAPHIC_OPTION17; ++i) 
	{	
		if ( !CheckText( strID[strID_IDC_GRAPHIC_OPTION], i-szID_IDC_GRAPHIC_OPTION0  ) )
		{
			strBuff.LoadString(szID[i]);
			strArray->Add(strBuff);
		}
	}
	m_LauncherText.mapFlags[m_emLang][strID[strID_IDC_GRAPHIC_OPTION]] = strArray;
	
	// 荤款靛 可记
	pos = m_LauncherText.mapFlags[m_emLang].find( strID[strID_IDC_SOUND_OPTION] );
	end = m_LauncherText.mapFlags[m_emLang].end();
	if ( pos == end ) strArray = new CStringArray;
	else strArray = (*pos).second;

	for ( i = szID_IDC_SOUND_OPTION0; i <= szID_IDC_SOUND_OPTION5 ; ++i) 
	{	
		if ( !CheckText( strID[strID_IDC_SOUND_OPTION], i-szID_IDC_SOUND_OPTION0  ) )
		{	
			strBuff.LoadString(szID[i]);		
			strArray->Add(strBuff);
		}
	}
	m_LauncherText.mapFlags[m_emLang][strID[strID_IDC_SOUND_OPTION]] = strArray;


	// IDS_MESSAGE
	pos = m_LauncherText.mapFlags[m_emLang].find( strID[strID_IDS_MESSAGE] );
	end = m_LauncherText.mapFlags[m_emLang].end();
	if ( pos == end ) strArray = new CStringArray;
	else strArray = (*pos).second;

	for ( i = szID_IDS_MESSAGE0 ; i<= szID_IDS_MESSAGE78; ++i) 
	{	
		if ( !CheckText( strID[strID_IDS_MESSAGE], i-szID_IDS_MESSAGE0  ) )
		{
			strBuff.LoadString(szID[i]);			
			strArray->Add(strBuff);
		}
		else
		{
			continue;
		}
	}
	m_LauncherText.mapFlags[m_emLang][strID[strID_IDS_MESSAGE]] = strArray;

	return TRUE;
}

const CString & CLauncherTextMan::GetLauncherText ( std::string strKey, int Index )
{
	return GetText ( strKey, Index);
}

const CString & CLauncherTextMan::GetText ( std::string strKey, int Index)
{
	return NS_TEXT_LOADER::GetText ( strKey, Index, m_LauncherText.mapFlags[m_emLang] );
}

bool CLauncherTextMan::CheckText ( std::string strKey, int Index)
{
	return NS_TEXT_LOADER::CheckText ( strKey, Index, m_LauncherText.mapFlags[m_emLang] );
}

int	CLauncherTextMan::GetNumber ( std::string strKey, int Index)
{
	return NS_TEXT_LOADER::GetNumber ( strKey, Index, m_LauncherText.mapFlags[m_emLang] );
}

const char * ID2LAUNCHERTEXT( const char* szKeyword, int nIndex )
{
	return CLauncherTextMan::GetInstance().GetText( szKeyword, nIndex);
}

bool CLauncherTextMan::LoadHelpCommentString(const std::string& strID
						   , std::vector<std::string>& vecOutList)
{
	NS_TEXT_LOADER::STRFLAG* mapFlags = &m_LauncherText.mapFlags[m_emLang];
	NS_TEXT_LOADER::STRFLAG_IT FlagItr = mapFlags->find(strID.c_str());
	if ( FlagItr == mapFlags->end() )
	{
		CString strTemp;
		strTemp.Format("[GAME TEXT ERROR]Not exist text keyword : %s", strID.c_str());
		AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
	}
	else
	{
		CStringArray *pStrArray = (*FlagItr).second;
		if ( pStrArray->GetCount () == 0 )
		{
			CString strTemp;
			strTemp.Format("[GAME TEXT ERROR]Index Range is 0 : %s", strID.c_str());
			AfxMessageBox( strTemp, MB_OK|MB_ICONSTOP );
		}
		for ( INT j = 0; j < pStrArray->GetCount(); ++j )
		{
			vecOutList.push_back(pStrArray->GetAt(j).GetString());
		}
	}

	return !vecOutList.empty();
}