#ifndef _CHASH_H
#define _CHASH_H
/**
 * \ingroup EngineLib
 *
 * \date 2006-10-25
 *
 * \author Jgkim
 *
 * \par license
 *
 * http://www.codeproject.com/cpp/chash.asp 의 코드를 약간 수정했다. 
 *
 */

// Choose which algorithms you want
// Put 1s to support algorithms, else 0 to not support
#define        SUPPORT_CRC32          1
#define        SUPPORT_GOSTHASH       1
#define        SUPPORT_MD2            1
#define        SUPPORT_MD4            1
#define        SUPPORT_MD5            1
#define        SUPPORT_SHA1           1
#define        SUPPORT_SHA2           1

namespace HASH
{
	// 간단한 스트림->해쉬키(4byte)의 경우 generateHashKey()를 이용 하도록 하자;
	extern __forceinline const unsigned int __fastcall generateHashKey(const void* const _pStream, const unsigned int _sizeStream);
	
	// Definitions of some kind
	enum emHASH_METHOD
	{
		STRING_HASH = 1, // 스트링을 해쉬할때
		FILE_HASH   = 2, // 파일을 해쉬할때
	};

	enum emHASH_SIZE
	{
		SIZE_OF_BUFFER = 16000,
	};

	// Algorithms
	enum emHASH_ALGORITHM
	{
		CRC32 = 1,
		GOSTHASH = 2,
		MD2 = 3,
		MD4 = 4,
		MD5 = 5,
		SHA1 = 6,
		SHA2 = 7,
	};

	// Formatting options
	// 해쉬된 결과를 문자열로 돌려줄때 포맷을 지정한다.
	enum emHASH_FORMAT
	{
		LOWERCASE_NOSPACES = 1, // eff7d5dba32b4da32d9a67a519434d3f
		LOWERCASE_SPACES   = 2, // ef f7 d5 db a3 2b 4d a3 2d 9a 67 a5 19 43 4d 3f 
		UPPERCASE_NOSPACES = 3, // EFF7D5DBA32B4DA32D9A67A519434D3F
		UPPERCASE_SPACES   = 4, // EF F7 D5 DB A3 2B 4D A3 2D 9A 67 A5 19 43 4D 3F 
	};

	// CHash definitions
	class CHash
	{
		// Construction
	public:
		CHash();   // Standard constructor
		CString DoHash();
		int GetHashAlgorithm();
		CString GetHashFile();
		int GetHashFormat();
		int GetHashOperation();
		CString GetHashString();
		int GetSHA2Strength();
		CString GOSTHash();
		CString MD2Hash();
		CString MD4Hash();
		CString MD5Hash();
		CString SHA1Hash();
		CString SHA2Hash();
		void SetHashAlgorithm(int Algo);
		void SetHashOperation(int Style);
		void SetHashFile(LPCSTR File);
		void SetHashFormat(int Style);
		void SetHashString(LPCSTR Hash);
		void SetSHA2Strength(int Strength);

		// Implementation
	protected:
		// Input string, algorithm, filename, formatting and such
		CString    hashString;
		int        hashAlgo;
		CString    hashFile;
		int        hashFormatting;
		int        hashStyle;
		int        hashOperation;
		int        sha2Strength;

		// Temporary working CString
		CString    tempHash;
	};

	class CHashW
	{
	public:
		CHashW();
		CStringW DoHash();
		int GetHashAlgorithm();
		CStringW GetHashFile();
		int GetHashFormat();
		int GetHashOperation();
		CStringW GetHashString();
		int GetSHA2Strength();
		CStringW GOSTHash();
		CStringW MD2Hash();
		CStringW MD4Hash();
		CStringW MD5Hash();
		CStringW SHA1Hash();
		CStringW SHA2Hash();
		void SetHashAlgorithm(int Algo);
		void SetHashOperation(int Style);
		void SetHashFile( const wchar_t* File);
		void SetHashFormat( int Style );
		void SetHashString( const wchar_t* Hash );
		void SetSHA2Strength( int Strength );

	protected:
		CStringW hashString;
		int      hashAlgo;
		CStringW hashFile;
		int      hashFormatting;
		int      hashStyle;
		int      hashOperation;
		int      sha2Strength;
		CStringW tempHash;
	};

}

#endif // _CHASH_H
