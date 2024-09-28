#include "stdafx.h"

#include "./BaseString.h"
#include "./IniLoader.h"

CIniLoader::CIniLoader( const STRING& sep, bool bToolMode )
: m_fieldsep( sep )
, m_bZipFile( FALSE )
, m_bUseErrorMsgBox( TRUE )
, m_bToolMode( bToolMode )
{
}

CIniLoader::~CIniLoader()
{
	for( MAPSECTION_ITER s_pos = m_mapSection.begin(); s_pos != m_mapSection.end(); ++s_pos )
	{
		MAPKEY * pMapKey(NULL);
		pMapKey = s_pos->second;
		if( !pMapKey ) continue;

		for( MAPKEY_ITER k_pos = pMapKey->begin(); k_pos != pMapKey->end(); ++k_pos )
		{
			delete k_pos->second;
			k_pos->second = NULL;
		}

		pMapKey->clear();
		delete pMapKey;
		pMapKey = NULL;
	}

	m_mapSection.clear();
}

CIniLoader::MAPKEY* CIniLoader::FindSection( const STRING& section )
{
	MAPSECTION_ITER iter_sec = m_mapSection.find( section );
	if( iter_sec != m_mapSection.end() )
	{
		return iter_sec->second;
	}

	return NULL;
}

CIniLoader::VECSTRING* CIniLoader::FindKey( const STRING& section, const std::string& key )
{
	MAPSECTION_ITER iter_sec = m_mapSection.find( section );
	if( iter_sec != m_mapSection.end() )
	{
		MAPKEY * pMapKey = iter_sec->second;

		MAPKEY_ITER iter_key = pMapKey->find( key );
		if( iter_key != pMapKey->end() )
		{
			return iter_key->second;
		}
		else
		{
			return NULL;
		}
	}

	return NULL;
}

CIniLoader::VECSTRING* CIniLoader::validflag(const STRING& section, const STRING& key, const int nIDX, const int nSIZE)
{
	VECSTRING* pVecStr = FindKey(section, key);
	if (!pVecStr)
		return NULL;

	if (nSIZE != pVecStr->size())
	{
		return NULL;
	}

	return pVecStr;
}

CIniLoader::VECSTRING* CIniLoader::validflag( const DWORD dwITER, const STRING& section, const STRING& key, const int nIDX, const int nSIZE )
{
	MAPKEY* pMapKey = FindSection(section);
	if (!pMapKey)
		return NULL;

	MAPKEY_PAIR pair = pMapKey->equal_range(key);
	if (pair.first == pair.second)
	{
		return NULL;
	}

	MAPKEY_ITER iter = pair.first;
	for (DWORD i=0; i<dwITER; ++i)
		++iter;

	VECSTRING* pFOUND_VEC = iter->second;

	DWORD dwPARAMNUM = (DWORD) pFOUND_VEC->size();
	if (dwPARAMNUM != nSIZE)
	{
		return NULL;
	}

	return pFOUND_VEC;
}

bool CIniLoader::IsNotes( const CString & line )
{
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == _T(' ') || line[i] == _T('\t') ) continue;
		if( line[i] == _T(';') )
			return true;
		else
			return false;
	}

	return false;
}

int CIniLoader::GetKeyData( const CString & line, STRING & key, VECSTRING * pVec )
{
	CString strTemp( line );
	CString resToken;
	int curPos = 0;

	resToken = strTemp.Tokenize( "=", curPos );
	if( resToken == "" ) // Notes : Ű���� �����Ѵ�.
	{
		strTemp.Trim( _T(" \t") );
		key = strTemp;

		return 0;
	}

	int nTokenLen = resToken.GetLength();
	CString strData( strTemp.Right( strTemp.GetLength() - nTokenLen - 1) );
	strData.Trim( _T(" \t") );

	resToken.Trim( _T(" \t") );
	key = resToken;

	int nRet = split( STRING(strData), pVec );

	return nRet;
}

bool CIniLoader::IsSection( const CString & line, STRING & sec )
{
	int nStart(0);
	int nLen = line.GetLength();

	for( int i = 0; i < nLen; ++i )
	{
		if( line[i] == ' ' || line[i] == '\t' ) continue;
		if( line[i] != '[' ) return false;
		else
		{
			nStart = i;
			break;
		}
	}

	int nEnd = nLen - 1;

	for( int i = nEnd; i > 0; --i )
	{
		if( line[i] == _T(' ') || line[i] == '\t' ) continue;
		if( line[i] != ']' ) return false;
		else
		{
			nEnd = i;
			break;
		}
	}

	if( nStart > nEnd ) return false;

	sec = line.Mid( ++nStart, --nEnd );

	return true;
}

bool CIniLoader::open(const STRING& strFile, bool bDEC, BOOL bPack)
{
	m_strFileName = strFile;

	char fname[_MAX_FNAME]={0};
	char ext[_MAX_EXT]={0};
	_splitpath( m_strFileName.c_str(), NULL, NULL, fname, ext );
	StringCchCat( fname, _MAX_FNAME, ext );

	std::auto_ptr<sc::CBaseString> pBString(
		sc::openfile_basestring(
		m_strZipFileName, 
		m_strFileName.c_str(), 
		fname, 
		bDEC,
		bPack));

	if( !pBString.get() )
	{
		//CDebugSet::ToLogFile ( "ERROR : CIniLoader::open(), File Open %s", fname );
		return false;
	}

	sc::CBaseString &cBString = *pBString;

	m_bZipFile = FALSE;

	MAPKEY * pMapKey(NULL);
	STRING sectionback;

	CString strLine;
	while( cBString.GetNextLine(strLine) )
	{
		if( !IsNotes( strLine ) ) // Notes : �ּ�ó��
		{
			if( pMapKey == NULL )
				pMapKey = new MAPKEY;

			STRING section;

			if( !IsSection( strLine, section ) ) // Notes : Ű�� ������ ó��, ó���� ������ ������ ���;� �Ѵ�.
			{
				VECSTRING * pVecStr = new VECSTRING;

				// Notes : �� ������ Ű�� �����ͷ� �����Ѵ�. Ű�� ��Ʈ��, ���� ��Ƽ�� ���Ϳ� �����Ѵ�.
				STRING key;
				GetKeyData( strLine, key, pVecStr ); 

				//std::pair< MAPKEY_ITER, bool > pr;
				//pr = pMapKey->insert( std::make_pair( key, pVecStr ) );

				pMapKey->insert( std::make_pair( key, pVecStr ) );

				//if( pr.second != true )   
				//{
				//	ErrorMegBox( section.c_str(), key.c_str(), "The Key already exists." );
				//}
			}
			else
			{
				// Note : ����ó��
				STRING strTemp(section);

				if( sectionback.empty() )
				{
					sectionback = section;
				}

				if( !sectionback.empty() && !pMapKey->empty() )
				{
					std::pair< MAPSECTION_ITER, bool > pr;
					pr = m_mapSection.insert( std::make_pair( sectionback, pMapKey ) );
					if( pr.second != true )   
					{
					}

					sectionback = strTemp;
					pMapKey = NULL;
				}
			}
		}
	}

	if( !sectionback.empty() && !pMapKey->empty() )
	{
		std::pair< MAPSECTION_ITER, bool > pr;
		pr = m_mapSection.insert( std::make_pair( sectionback, pMapKey ) );
		if( pr.second != true )   
		{
		}
	}

	return true;
}

int CIniLoader::split( const STRING & line, VECSTRING * pVec )
{
	STRING fld;
	UINT i(0),j(0);
	UINT nfield(0);

	if( (UINT)line.length() == 0 )
		return 0;

	do
	{
		if( i < (UINT)line.length() && line[i] == _T('"') )
			j = advquoted( line, fld, ++i); // Notes : �ο��ȣ ó�� ����
		else
			j = advplain( line, fld, i );

		if( j > i )
		{
			if( nfield >= (UINT)pVec->size() )
				pVec->push_back(fld);
			else
				(*pVec)[nfield] = fld;

			nfield++;
		}

		i = j + 1;
	} while( j < (UINT)line.length() );

	return nfield;
}

// Notes : �ο��ȣ�� �ѷ� ���� �ʵ�
int CIniLoader::advquoted(const STRING & s, STRING & fld, UINT i)
{
	UINT j(0);
	fld = _T("");

	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( j = i; j < (unsigned int)s.length(); j++ )
	{
		if( s[j] == _T('"') && s[++j] != _T('"') )
		{
			indexCh = findfirstof( s, m_fieldsep, j );

			if( indexCh == npos ) // Notes : �����ڰ� ���� ���
				indexCh = s.length();

			for( indexCh -= j; indexCh-- > 0; )
			{
				fld += s[j++];
			}

			break;
		}

		fld += s[j];
	}

	return j;
}

// Notes : �ο��ȣ�� �ѷ������� ���� �ʵ�
int CIniLoader::advplain(const STRING & s, STRING & fld, UINT i ) 
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	indexCh = findfirstof( s, m_fieldsep, i );

	if( indexCh == npos ) // Notes : �����ڰ� ���� ���
		indexCh = s.length();

	fld = STRING( s, i, indexCh-i);

	return (int)indexCh;
}

UINT CIniLoader::GetKeyDataSize( const STRING& section, const STRING& key )
{
	VECSTRING * pVecStr = FindKey( section, key );
	if( !pVecStr ) return 0;

	return (UINT)pVecStr->size();
}

DWORD CIniLoader::GetKeySize( const STRING& section, const STRING& key )
{
	MAPKEY * pMapKey = FindSection( section );
	if( !pMapKey ) return 0;

	MAPKEY_PAIR pair = pMapKey->equal_range( key );
	if( pair.first == pair.second )
	{
		// ErrorMegBox( section.c_str(), key.c_str(), "A Key which does not exist." );
		return 0;
	}

	return (DWORD)std::distance( pair.first, pair.second );
}

int CIniLoader::findfirstof( const STRING & line, const STRING & sep, UINT i )
{
	STRING::size_type indexCh;
	static const STRING::size_type npos = -1;

	for( int index=i; index<(int)line.length(); )
	{
		if( CHARSET::IsLeadChar( line[index] ) )
		{
			index += 2;
			continue;
		}

		indexCh = sep.find( line[index] );
		if( indexCh != npos )
			return index;

		++index;
	}

	return npos;
}