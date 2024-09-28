#include "pch.h"
#include "./s_NetGlobal.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/******************************************************************************
* GameGuard 에러 코드 체크 함수
* \param dwErrCode nProtect GameGuard 에러코드
*/
std::string GetGameGuardErrorString(DWORD dwErrCode)
{
	std::string strError;

	switch (dwErrCode)
	{
	case 1:
		// 메모리 할당 실패
		strError = _T("Memory allocation failed");
		break;
	case 2:
		// ggauth.dll 로드 실패
		strError = _T("ggauth.dll load failed");
		break;
	case 3:
		// ggauth.dll 의 Export 함수 가져오기 실패
		strError = _T("ggauth.dll function export error");
		break;
	case 4:
		// ggauth.dll 이 초기화 되기 전에 Export 함수 호출하였음
		strError = _T("Do not call function before InitGameguardAuth");
		break;
	case 10:
		// 함수 호출 시 invalid parameter 전달
		strError = _T("Invalid parameter function call");
		break;
	case 11: 
		// 인증 Query 에 대한 클라이언트의 응답 없음.
		strError = _T("Client did not response requested query");
		break;
	case 12:
		// 클라이언트의 인증 프로토콜 버전 틀림
		strError = _T("Not correct protocol version of client");
		break;
	case 13:
		// 인증 Query 에 대한 클라이언트의 응답값이 틀림
		strError = _T("Client response is wrong");
		break;
	case 101:
		// Gamemon 버전이 틀림
		strError = _T("Gamemon version is incorrect");
		break;
	case 102:
		// Gamemon 버전 확인위한 코드값이 틀림
		strError = _T("Code is wrong for Gamemon version check");
		break;
	default:
		strError = _T("Unknown Error");
		break;
	}
	return strError;
}


LONGLONG DECRYPTMONEYVER100( const char* szMoneyChar )
{
	char szTempChar[CHAR_TEMPMONEY], szTemp[2];
	LONGLONG lnTempValue = 0;

	::StringCchCopy(szTempChar, CHAR_TEMPMONEY, (const char*) szMoneyChar);

	size_t i;
	for( i = 0; i < strlen(szTempChar); i++ )
	{
		szTempChar[i] -= 20;
		sprintf_s( szTemp, 2, "%c", szTempChar[i] );
		lnTempValue = ( lnTempValue * 10 ) + atoi(szTemp);
	}
	lnTempValue >>= 0x04;

	return lnTempValue;
}

std::string ENCRYPTMONEYVER100(const LONGLONG lnGameMoney)
{
	char szTempMoney[CHAR_TEMPMONEY];
	LONGLONG lnTempMoney = lnGameMoney;
	lnTempMoney <<= 0x04;
	sprintf_s(szTempMoney, CHAR_TEMPMONEY, "%I64d", lnTempMoney);
	size_t i;
	for (i = 0; i < strlen(szTempMoney); ++i)
	{
		szTempMoney[i] += 20;
	}
    return std::string(szTempMoney);
}

LONGLONG DECRYPTMONEYVER101( const char* szMoneyChar )
{
	char szTempChar[CHAR_TEMPMONEY], szTemp[2];
	LONGLONG lnTempValue = 0;

	::StringCchCopy(szTempChar, CHAR_TEMPMONEY, (const char*) szMoneyChar);

	size_t i;
	for( i = 0; i < strlen(szTempChar); i++ )
	{
		szTempChar[i] ^= 'base';
		sprintf_s( szTemp, 2, "%c", szTempChar[i] );
		lnTempValue = ( lnTempValue * 10 ) + atoi(szTemp);
	}
	lnTempValue ^= 0x203321;


	return lnTempValue;
}

std::string ENCRYPTMONEYVER101(const LONGLONG lnGameMoney)
{
	char szTempMoney[64];
	LONGLONG lnTempMoney = lnGameMoney;

	lnTempMoney ^= 0x203321;
	sprintf_s(szTempMoney, 64, "%I64d", lnTempMoney);

	size_t i;
	for (i = 0; i < strlen(szTempMoney); i++)
	{
		szTempMoney[i] ^= 'base';
	}
    return std::string(szTempMoney);
}


LONGLONG DECRYPTMONEY( const char* szMoneyChar, const int ver /* = GAMEMONEY_VER */ )
{
	return DECRYPTMONEYVER100(szMoneyChar);
}

std::string ENCRYPTMONEY(const LONGLONG lnGameMoney, const int ver /* = GAMEMONEY_VER */)
{
	return ENCRYPTMONEYVER100(lnGameMoney);
}

bool NET_MSG_PACK::SetData(EMNET_MSG MsgType, const msgpack::sbuffer& sbuf, bool UseCrc, bool MsgBox)
{
    size_t DataSize = sbuf.size();
    if (DataSize >= BUFFER_SIZE)
    {
        std::string ErrorMsg(
            sc::string::format(
                "NET_MSG_PACK::SetData Type %1% Size %2% > %3%",
                MsgType,
                DataSize,
                BUFFER_SIZE));
        sc::ErrorMessageBox(ErrorMsg, !MsgBox);
        return false;
    }

    memcpy_s(m_Buffer, BUFFER_SIZE, sbuf.data(), DataSize);

    nType = MsgType;
    dwSize = static_cast<DWORD> (HEAD_SIZE + DataSize);
    m_DataSize = DataSize;
    if (UseCrc)
        m_Crc = sc::string::getCrc32(m_Buffer, m_DataSize);
    return true;
}