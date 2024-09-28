#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#pragma once

#include <Rpc.h>
#pragma comment (lib, "Rpcrt4.lib")

#include <string>
#include <algorithm>
#include <vector>
#include "./StringDefine.h"

namespace sc {
    namespace string {

        //! \ingroup CORE_COMMON
        //! \version 1.0
        //! \author Jgkim
        //! \history
        //!    2006.08.08 Jgkim : Create
        //!    2009-10-15 Jgkim : MD5, SHA1, SHA2, CRC32 �߰�
        //! \par license
        //!    Copyright (c) Min Communications. All rights reserved.
        //! \todo
        //!    C++ ǥ���Լ��θ� ����
        //! \bug 

        //! ��� ���� �������� üũ�Ѵ�.	
        //! Y:\_Study\Unicode\ �� �����ڵ� ǥ�� ����.
        EM_CHAR_NATION checkWCharNation( wchar_t wChar );

        //! ����� ID �� �ùٸ��� üũ�Ѵ�.
        //! 1. ����/���� �θ� �����Ǿ�� �Ѵ�.
        bool goodIdName( const wchar_t* pChar, size_t nLength );

        //! �����̸��� �ùٸ��� �˻��Ѵ�.
        //! �����̸� ��Ģ
        //! 1. ����, ����, _, . ���θ� �����Ǿ�� �Ѵ�.
        //! 2. �ҹ��ڷθ� �����Ǿ�� �Ѵ�.
        //! �Է��� �ҹ��ڷθ� �����Ǿ��ٰ� �����ϰ�
        //! �ҹ��� �˻�� ���Ѵ�.
        bool goodFileName( const wchar_t* pChar, int nLength );

        //! �ѱ��� �´� ĳ���� �̸����� �˻��Ѵ�.
        //! ĳ���� �̸� ��Ģ
        //! 1. �ѱ�+����
        //! 2. ����+����
        bool goodChaNameKorea(const std::string& strName);
        bool goodChaNameKorea(const std::wstring& strName);
        bool goodChaNameKorea(const wchar_t* pChar, int nLength);

        //! �±���
        bool goodChaNameThailand(const std::wstring& strName);
        bool goodChaNameThailand(const wchar_t* pChar, int nLength);

        //! Ư������ . , ���� ���ԵǾ� �ִ°�?
        bool haveSymbolCha( const std::wstring& strName );
        bool haveSymbolCha( const wchar_t* pChar, size_t nLength );

        //! ������ ���� ���ڰ��� �˻�
        //! ����, ���� : 1 ����
        //! �ѱ�, ����, �Ͼ�, �±��� : 2 ����
        size_t lengthForGame( const std::wstring& strName );
        size_t lengthForGame( const wchar_t* pChar, int nLength );

        //! ���ڿ��� ����θ� �����Ǿ� �ִ��� üũ�Ѵ�
        bool checkEnglish( const wchar_t* pChar, size_t nLength );
        bool checkEnglish( const std::wstring& ChkStr );

        //! ���ڿ��� ���ڷθ� �����Ǿ� �ִ��� üũ�Ѵ�.
        bool checkNumber( const wchar_t* pChar, size_t nLength );
        bool checkNumber( const std::wstring& ChkStr );

        //! ���ڿ��� ���ڿ� ����θ� �����Ǿ� �ִ��� üũ�Ѵ�.
        bool checkEnglishOrNumber(const wchar_t* pChar, size_t nLength);
        bool checkEnglishOrNumber(const std::wstring& ChkStr);
        bool checkEnglishOrNumber(const std::string& ChkStr);

        //! ���ڿ��� ���ڿ� �ѱ۷θ� �����Ǿ� �ִ��� üũ�Ѵ�.
        bool checkKoreanOrNumber(const wchar_t* pChar, size_t nLength);
        bool checkKoreanOrNumber(const std::wstring& ChkStr);
        bool checkKoreanOrNumber(const std::string& ChkStr);

        //! �±����� ����ϴ� �������� �˻��Ѵ�.
        bool checkThaiName(const wchar_t* pChar, size_t nLength);
        bool checkThaiName(const std::wstring& ChkStr);

        //! Convert multibyte to Unicode
        //! \param CodePage unicode code page
        std::wstring multiToWide( const char* str, int len,	UINT CodePage );

        //! Convert wstring to string
        //! \param CodePage unicode code page
        std::string wideToMulti( const wchar_t* str, int len, UINT CodePage );

        //! Convert string to wstring	
        std::wstring ansiToUnicode( const char* szAscii, int nLen );

        //! Convert string to wstring
        std::wstring ansiToUnicode( const std::string& StrW );

        //! Convert string to wstring	
        void ansiToUnicode( const char* szAscii, wchar_t* szUnicode, int nLen );

        //! Convert string to wstring	
        std::string unicodeToAnsi( const wchar_t* szUnicode, int nLen );

        //! Convert a Unicode string to an ASCII string
        std::string unicodeToAnsi( const std::wstring& strW );

        //! Convert a Unicode string to an ASCII string	
        void unicodeToAnsi( const wchar_t* szUnicode, char* szAscii, int nLen );	

        //! Convert UTF8 to Unicode	
        std::wstring utf8ToUnicode( const char* szUtf8, int nLen );

        std::wstring utf8ToUnicode( const std::string& Utf8 );

        //! Convert UTF8 to Unicode	
        void utf8ToUnicode( const char* szUtf8, wchar_t* szUnicode, int nLen );

        std::string unicodeToUtf8( const wchar_t* szUnicode, int nLen );

        std::string unicodeToUtf8( const std::wstring& StrW );

        //! Convert Unicode to UTF8	
        void unicodeToUtf8( const wchar_t* szUnicode, char* szUtf8, int nLen );	

		
		//
		//mjeon.utf-8		
		//
		std::string ansi_to_utf8(std::string& ansi);
		std::string utf8_to_ansi(std::string &utf8);
		std::string utf8_to_ansi(char *utf8, int length);	//Overloaded function uses char-array

		

        //! ���ο� UUID string �� ��´�
        int getUUID(char* pszUUID, size_t cchUUID);

        //! ���ο� UUID �� ��´�.
        int getUUID( UUID &_uuid );

        //! UUID �� string ���� ��ȯ�Ѵ�.
        int uuidToString( UUID &_uuid, TCHAR* pszUUID );

        std::string uuidToString(UUID& _uuid);

        //! string �� UUID �� ��ȯ�Ѵ�.
        int stringToUUID(char* pszUUID, UUID &_uuid);

        //! Generate randim string
        //! �� �Լ��� ������� ������. while ���� �ʹ� ����
        //! \param str ������ ���ڿ��� ���� ��
        //! \param Length ���ڿ�����
        //! \param Small 
        //! \param Capital 
        //! \param Numbers 
        //! \return true / false
        bool generateRandomString(
            CString& str,
            int Length,
            int Small,
            int Capital,
            int Numbers );

        //! �ֹι�ȣ�� üũ�ؼ� �������� �ƴ����� �˻�
        //! \param strCheck �ֹε�Ϲ�ȣ���ڸ� 6����
        //! \param nCheckAge ������ �Ǵ� ���� 
        //! \return true / false
        bool checkAdult( CString& strCheck, int nCheckAge=18 );

        //! �ֹε�Ϲ�ȣ ���ڸ��� ���̸� ���Ѵ�.
        //! �˴ٿ��� ������ �ӽ÷� �ۼ�
        //! �ֹε�Ϲ�ȣ ���ڸ� �����δ� ��Ȯ�ϰ� ���� �� ��� �ӽ��ڵ��̴�.
        //! ����� ������� �ֹε�Ϲ�ȣ ���ڸ��� �־�� �Ѵ�.
        WORD SsnHeadToAge(const std::string& SsnHead);

        TCHAR* lower_bound( TCHAR *pFirst, DWORD dwCount, TCHAR cFIND );

        bool binary_search( TCHAR *pFirst, DWORD dwCount, TCHAR cFIND );

        //! ��ü ��θ� �ָ�.
        //! ��ο� ���ϸ�� Ȯ���ڸ��� ����� �����ش�.
        //! strFullPath ��ü���(���ϸ� ����)
        //! strPath ���
        //! strFullFileName �����̸�(Ȯ��������)
        //! strFileExe Ȯ����
        void getFilePathAndName(
            const std::wstring& strFullPath IN,
            std::wstring& strPath OUT,
            std::wstring& strFullFileName OUT,
            std::wstring& strFileExt OUT );

        void getFilePathAndName(
            const std::string& strFullPath IN,
            std::string& strPath OUT,
            std::string& strFullFileName OUT,
            std::string& strFileExt OUT );

        void getFilePathAndName(
            const CString& _strFullPath IN,
            CString& _strPath OUT,
            CString& _strFullFileName OUT,
            CString& _strFileExt OUT );

        //! ��ü ��θ� �ָ�
        //! Ȯ���ڸ� ������ ���� �̸��� �����ش�
        void getFullFileName(
            const std::string& strFullPath IN,
            std::string& strFullFileName OUT );

        //! ��ü ��θ� �ָ�
        //! Ȯ���ڸ� ������ ���� �̸��� �����ش�
        void getFullFileName(
            const CString& strFullPath IN,
            CString& strFullFileName OUT );

        //! ���� Ȯ���ڸ� �����ش�.
        CString getFileExt( const LPCTSTR szFileName );

        //! ���ڿ��� �ҹ��ڷ� ��ȯ�Ѵ�.
        void lowerString( std::wstring& strConvert );
        void lowerString( std::string& strConvert );
        std::wstring lowerString2(const std::wstring& strConvert);

        //! ���ڿ��� �빮�ڷ� ��ȯ�Ѵ�.
        void upperString( std::wstring& strConvert );
        void upperString( std::string& strConvert );

        //! ���ڿ��� �������� �˻��Ѵ�. ��ҹ��� ������
        //! ���ڿ��� ������ true, Ʋ���� false
        bool equal( const std::wstring& strL, const std::wstring& strR );

        //! ���ڿ��� �������� �˻��Ѵ�. ��ҹ��� ���о���
        //! ���ڿ��� ������ true, Ʋ���� false
        bool equalNoCase( const std::wstring& strL, const std::wstring& strR );	

        //! Byte Buffer �� Hex Encoding �Ѵ�.
        std::string  HexEncodeA(BYTE* pBuffer, size_t nLength);
        std::wstring HexEncodeW(BYTE* pBuffer, size_t nLength);

        std::string  HexDecodeA(const std::string& strSrc);
        std::wstring HexDecodeW(const std::wstring& strSrc);

        std::string  Base64EncodeA(BYTE* pBuffer, size_t BufSize);
        std::wstring Base64EncodeW(BYTE* pBuffer, size_t BufSize);

        std::string  Base64DecodeA(BYTE* pBuffer, size_t BufSize);
        std::wstring Base64DecodeW(BYTE* pBuffer, size_t BufSize);

        //! Trim
        void trim( std::wstring& strSrc, const wchar_t* TrimStr = L" " );
        void trimLeft( std::wstring& strSrc, const wchar_t* TrimStr = L" " );
        void trimRight( std::wstring& strSrc, const wchar_t* TrimStr = L" " );

        void trim( std::string& strSrc, const char* TrimStr = " " );
        void trimLeft( std::string& strSrc, const char* TrimStr = " " );
        void trimRight( std::string& strSrc, const char* TrimStr = " " );

        //! Replace
        void replace(std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr);
        void replace(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr);
        void replace_all(std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr);
        void replace_all(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr);

        void replace_first(std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr);
        void replace_first(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr);

        void replace_last(std::wstring& strSrc, const std::wstring& SearchStr, const std::wstring& NewStr);
        void replace_last(std::string& strSrc, const std::string& SearchStr, const std::string& NewStr);

        void replaceForDb(std::wstring& strSrc);        
        void replaceForDb(std::string& strSrc);
        std::string GetCurrentTime();

        //! Clipboard
        //! Unicode string -> Clipboard
        bool stringToClipboard(const std::string& strSrc);
		bool stringToClipboard(const std::wstring& strSrc);

        //! Clipboard -> Unicode string
        bool clipboardToString(HWND hWnd, std::string& strTar);
        bool clipboardToString2(HWND hWnd, std::string& strTar);
		bool clipboardToString(HWND hWnd, std::wstring& strTar);

        //! time ���� string ���� �����.
        std::string DateTimeFormat( const __time64_t& Time, bool bTimeInclude=false, bool bSecInclude=false );

        //std::wstring D3DCOLOR_TO_HTML( DWORD ColorA );

        //void ToHtml( const std::wstring& strTextIn, DWORD dwSize, DWORD dwColor, std::wstring& strTextOut, WORD wAddLine = 0 );

        //! ���ڿ��� html �� ��ȯ, Ư����ȣ ó��
        //! " -> &quot;  & -> &amp; < -> &lt; > -> &gt;
        void StringToHtml( std::wstring& strSrc );

        //! MD5 �� ���Ѵ�.
        std::string  getMd5A(const std::string& strSrc);
        std::wstring getMd5W(const std::wstring& strSrc);

        //! SHA1
        std::string  getSha1A(const std::string& strSrc);
        std::wstring getSha1W(const std::wstring& strSrc);

        //! SHA2
        std::string  getSha2A(const std::string& strSrc);
        std::wstring getSha2W(const std::wstring& strSrc);

        //! SHA256, 
        //! ��ȯ�Ǵ� hash ���� �빮���̴�.
        //! 64 ���� ���ڿ� + 1 (���鹮��) = 65
        //! Hash ���� ����� �Ǿ��ִ����� �Ʒ� URL ���� Ȯ�ΰ����ϴ�.
        //! http://www.xorbin.com/tools/sha256-hash-calculator
        std::string getSha256(const std::string& strSrc);

        //! CRC 32
        std::string  getCrc32A(const std::string& strSrc);
        std::wstring getCrc32W(const std::wstring& strSrc);
        unsigned int getCrc32(void const *buffer, std::size_t byte_count);
        unsigned short getCrc16(void const *buffer, std::size_t byte_count);

        std::wstring getAppPathW();
        std::string getAppPathA();	

        int HexPairValue(const char * code);
        int UrlDecode(const char *source, char *dest);
        int UrlEncode(const char *source, char *dest, unsigned max);  
        std::string  UrlDecodeString(const std::string& encoded);
        std::string  UrlEncodeString(const std::string& decoded);
        std::wstring UrlDecodeStringW(const std::wstring& encoded);
        std::wstring UrlEncodeStringW(const std::wstring& decoded);


		//! strTarget �ȿ� strSource�� ���� �Ǿ� �ִ��� �˻��Ѵ�;
		//! true : �ش� strSource�� index�� ��ȯ;
		//! false : -1 ��ȯ;
		const unsigned int FindWord(const std::string& strTarget IN, const std::string& strSource IN);

        void SqlInjectionCheck(std::string& Sql);

        //! Windows via c/c++ 5th edition 40page
        std::string GetLastError(DWORD ErrorCode);

        //LONGLONG ConvertLongLong(const char* szText);

		// ���ڿ� , �� ����
		void AddCommaToNum(const char *str, char *buf);

        struct PARSE_RESULT
        {
            bool m_Mark;
            std::string m_Data;

            PARSE_RESULT()
                : m_Mark(false)
            {
            }
        };

        void ParseMark(const std::string& OriginalStr, char StartMark, char EndMark, std::vector<PARSE_RESULT>& SplitStr);

    } // namespace string

	class BitField128
	{
	public:
		const sc::BitField128& operator =(const sc::BitField128& _rhs)
		{
			::memcpy(this, &_rhs, sizeof(sc::BitField128));
			return *this;
		}
		const sc::BitField128& operator &=(const sc::BitField128& _rhs)
		{
			for ( unsigned int _index(sc::BitField128::BIT_FLAG_NSIZE); _index--; )
				this->arrayBit[_index] &= _rhs.arrayBit[_index];
			return *this;
		}
		const sc::BitField128& operator |=(const sc::BitField128& _rhs)
		{
			for ( unsigned int _index(sc::BitField128::BIT_FLAG_NSIZE); _index--; )
				this->arrayBit[_index] |= _rhs.arrayBit[_index];
			return *this;
		}
		const sc::BitField128& operator |=(const unsigned int _indexBit)
		{
			if ( _indexBit > (sc::BitField128::BIT_FLAG_NSIZE * 32 - 1) )
				return *this;

			const unsigned int _indexArray(sc::BitField128::BIT_FLAG_NSIZE - (_indexBit >> 5) - 1); // _indexBit / 32;
			const unsigned int _bit((_indexBit & 0x1fL)); // 0x1fL = 31;

			this->arrayBit[_indexArray] |= (0x1L << _bit);

			return *this;
		}

		const sc::BitField128& operator <<=(const unsigned int _bit)
		{
			const unsigned int _sizeUse(this->sizeUse);
			const unsigned int _pass(_bit >> 5); // _bit / 32;
			if ( _pass != 0 )
			{
				unsigned int* const _arrayBitTarget(&this->arrayBit[sc::BitField128::BIT_FLAG_NSIZE - _sizeUse]);
				if ( _pass < _sizeUse )
				{				
					const unsigned int* const _arrayBitSource(&_arrayBitTarget[_pass]);
					::memmove(_arrayBitTarget, _arrayBitSource, sizeof(unsigned int) * (_sizeUse - _pass));
					::memset(&this->arrayBit[sc::BitField128::BIT_FLAG_NSIZE - _pass], 0x0L, _pass << 2);
				}
				else
				{
					::memset(_arrayBitTarget, 0x0L, _sizeUse << 2);
					return *this;
				}
			}

			const unsigned int _shiftLeft(_bit & 0x1fL); // 0x1fL = 31;
			const unsigned int _loopFrom(sc::BitField128::BIT_FLAG_NSIZE - _sizeUse);
			const unsigned int _loopTo(sc::BitField128::BIT_FLAG_NSIZE - _pass - 1);		
			for ( unsigned int _index(_loopFrom); _index < _loopTo; ++_index )
			{
				const unsigned int _bitOver(this->arrayBit[_index + 1] >> (32 - _shiftLeft));
				this->arrayBit[_index] = (this->arrayBit[_index] << _shiftLeft) | _bitOver;			
			}	
			this->arrayBit[_loopTo] <<= _shiftLeft;

			return *this;
		}
		const sc::BitField128& operator >>=(const unsigned int _bit)
		{
			const unsigned int _sizeUse(this->sizeUse);
			const unsigned int _pass(_bit >> 5); // _bit / 32;
			if ( _pass != 0 )
			{
				unsigned int* const _arrayBitSource(&this->arrayBit[sc::BitField128::BIT_FLAG_NSIZE - _sizeUse]);
				if ( _pass < _sizeUse )
				{
					unsigned int* const _arrayBitTarget(&this->arrayBit[sc::BitField128::BIT_FLAG_NSIZE - _sizeUse + _pass]);
					::memmove(_arrayBitTarget, _arrayBitSource, sizeof(unsigned int) * (_sizeUse - _pass));
					::memset(_arrayBitSource, 0x0L, _pass << 2);
				}
				else
				{
					::memset(_arrayBitSource, 0x0L, _sizeUse << 2);
					return *this;
				}
			}

			const unsigned int _shiftRight(_bit & 0x1fL); // 0x1fL = 31;
			const unsigned int _loopFrom(sc::BitField128::BIT_FLAG_NSIZE - 1);
			const unsigned int _loopTo(sc::BitField128::BIT_FLAG_NSIZE - _sizeUse + _pass);
			for ( unsigned int _index(_loopFrom); _index > _loopTo; --_index )
			{
				const unsigned int _bitOver(this->arrayBit[_index - 1] << (32 - _shiftRight));
				this->arrayBit[_index] = (this->arrayBit[_index] >> _shiftRight) | _bitOver;
			}
			this->arrayBit[_loopTo] >>= _shiftRight;

			return *this;
		}
		const bool operator &(const unsigned int _indexBit) const
		{
			if ( _indexBit > ((this->sizeUse << 5) - 1) )
				return false;

			const unsigned int _indexArray(sc::BitField128::BIT_FLAG_NSIZE - (_indexBit >> 5) - 1); // _indexBit / 32;
			const unsigned int _bit((_indexBit & 0x1fL)); // 0x1fL = 31;

			return this->arrayBit[_indexArray] & (0x1L << _bit) ? true : false;
		}

		BitField128(void);
		BitField128(const unsigned int _sizeBitUse, const unsigned int _bitLow4Byte);
		BitField128(const unsigned int _sizeBitUse);
		BitField128(const sc::BitField128& _rhs);

	public:
		enum
		{
			BIT_FLAG_NSIZE = 4,
		};

		const unsigned int sizeUse;
		unsigned int arrayBit[sc::BitField128::BIT_FLAG_NSIZE];
	};
} // namespace sc

//#define  __WFUNCTION__ CORE_COMMON::StringUtil::ansiToUnicode(__FUNCTION__).c_str()
#endif // STRING_UTIL_H
