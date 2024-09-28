/**
 * \date	2008/06/17
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <string>

class CDepthWithTextToFile
{
private:
	FILE* m_pFile;

	UINT m_nDepth;
	std::string m_strDepth;

	bool m_bCharacterPredeclaredEntity;

public:
	CDepthWithTextToFile();
	virtual ~CDepthWithTextToFile();

public:
	bool Open(
		const std::string& strPath,
		const std::string& strMode,
		const std::string& strDepth,
		bool bCharacterPredeclaredEntity );

	void Close();

public:
	void Begin( const TCHAR* pFormat, ... );
	void Write( const TCHAR* pFormat, ... );
	void End( const TCHAR* pFormat, ... );

public:
	void WriteDepth();
	void CharacterPredeclaredEntity( TCHAR* pText, UINT nLength );

public:
	UINT AddDepth() { return ++m_nDepth; }
	UINT SubDepth() { return --m_nDepth; }
};