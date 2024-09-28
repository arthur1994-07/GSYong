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
	* MD5 �� �����ش�
	* \param strSource MD5 ���� ���� ����
	* \param strTarget MD5 ���� ���ϵǴ� ����
	* \param pData md5 �ؽ��� ������ ������
	* \param dwLength pData �� ����	
	* \return true:���� false:����
	*/	
	bool getMd5(const CStringW& strSource, CStringW& strTarget);
	bool getMd5(const CStringA& strSource, CStringA& strTarget);
	bool getMd5(const BYTE* pData, DWORD dwLength, CStringA& strTarget);
    bool getMd5(const BYTE* pData, DWORD dwLength, CStringW& strTarget);
	
	/**
	* Excite Japan ���� ����ϴ� ��ȣŰ�� �����Ѵ�.
	* \param szSecretKey ������ ��ȣŰ
	*/
	void ExciteSetSecretKey( const TCHAR* szSecretKey );
	
	/**
	* Excite Japan ���� ����ϴ� UserID �� LoginTime �� �޾Ƽ� MD5 �� ���Ѵ�.
	* \param szU UserID
	* \param szT Login Time
	* \param szMd5 Client ���� �Ѿ�� MD5 ��
	* \return true:���� false:����
	*/
	bool ExciteGetMd5(
		const TCHAR* szU,
		const TCHAR* szT,
		const TCHAR* szMd5 );
	/**
	* Excite Japan ���� �Ѿ���� LoginTime �� ����ε� ���������� Ȯ���Ѵ�.
	* \param strDate LoginTime ����:20050303020434 ����Ͻú���
	* \return true:���� false:����
	*/
	bool ExciteCheckDate( CString& strDate );

protected:
	/**
	* �ʱ�ȭ
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