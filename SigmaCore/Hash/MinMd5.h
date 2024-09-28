#ifndef MINMD5_H_
#define MINMD5_H_

#pragma once

#include <Wincrypt.h> // Crypt API, Advapi32.lib

// Auto Link Library Files
#pragma comment (lib, "Advapi32.lib")

namespace sc
{	
/**
 * \ingroup SigmaCore
 *
 * \par requirements
 * win98 or later\n
 * win2k or later\n
 * MFC\n
 *
 * \version 1.0
 * first version
 *
 * \date 2005-07-04
 *
 * \author Jgkim
 *
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 * 
 * \todo 
 *
 * \bug 
 *
 */
class MinMd5
{
public:
	/**
	*
	* \param void 
	* \return 
	*/
	MinMd5(void);

	/**
	*
	* \param void 
	* \return 
	*/
	~MinMd5(void);

	/**
	* MD5 를 구해준다
	* \param strSource MD5 값을 구할 원본
	* \param strTarget MD5 값이 리턴되는 버퍼
	* \param pData md5 해쉬할 데이터 포인터
	* \param dwLength pData 의 길이	
	* \return true:성공 false:실패
	*/	
	bool getMd5(const CStringW& strSource, CStringW& strTarget);
	bool getMd5(const CStringA& strSource, CStringA& strTarget);
	bool getMd5(const BYTE* pData, DWORD dwLength, CStringA& strTarget);
    bool getMd5(const BYTE* pData, DWORD dwLength, CStringW& strTarget);
	
	/**
	* Excite Japan 에서 사용하는 암호키를 설정한다.
	* \param szSecretKey 설정할 암호키
	*/
	void ExciteSetSecretKey( const TCHAR* szSecretKey );
	
	/**
	* Excite Japan 에서 사용하는 UserID 와 LoginTime 을 받아서 MD5 를 구한다.
	* \param szU UserID
	* \param szT Login Time
	* \param szMd5 Client 에서 넘어온 MD5 값
	* \return true:성공 false:실패
	*/
	bool ExciteGetMd5(
		const TCHAR* szU,
		const TCHAR* szT,
		const TCHAR* szMd5 );
	/**
	* Excite Japan 에서 넘어오는 LoginTime 이 제대로된 데이터인지 확인한다.
	* \param strDate LoginTime 형식:20050303020434 년월일시분초
	* \return true:성공 false:실패
	*/
	bool ExciteCheckDate( CString& strDate );

protected:
	/**
	* 초기화
	* \return 
	*/
	bool init();	

protected:
	HCRYPTPROV m_hCryptProv;	
	DWORD m_dwHashLen;
	CString m_strSecretKey;	
};

} //

#endif // MINMD5_H_