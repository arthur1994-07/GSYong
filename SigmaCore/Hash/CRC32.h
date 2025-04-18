#ifndef _CRC32STATIC_H_
#define _CRC32STATIC_H_

#include "HashCommon.h"

namespace HASH
{
extern DWORD s_arrdwCrc32Table[256];

class CCrc32Static
{
public:
	CCrc32Static();
	virtual ~CCrc32Static();

	static DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32);
	static DWORD FileCrc32Streams(LPCTSTR szFilename, DWORD &dwCrc32);

protected:
	static bool GetFileSizeQW(const HANDLE hFile, QWORD &qwSize);
	static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);
};

class CCrc32StaticW
{
public:
    CCrc32StaticW();
    virtual ~CCrc32StaticW();

    static DWORD StringCrc32(const wchar_t* szString, DWORD &dwCrc32);
    static DWORD FileCrc32Streams(const wchar_t* szFilename, DWORD &dwCrc32);

protected:
    static bool GetFileSizeQW(const HANDLE hFile, QWORD& qwSize);
    static inline void CalcCrc32(const BYTE byte, DWORD& dwCrc32);
};

//#include "crc32.cpp"

}; // End of namespace HASH

#endif // _CRC32STATIC_H_

