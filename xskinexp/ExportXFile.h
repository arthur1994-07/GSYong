#pragma once

#define POWER_DEFAULT   0.0

#define EXPD_FRAME		0x0001
#define EXPD_ANIMATION	0x0002
#define EXPD_KEY_ANI	0x0004

struct SDialogOptions
{
	BOOL m_bProceedWithExport;
	DXFILEFORMAT m_xFormat;
	DWORD m_cMaxBonesPerVertex;
	DWORD m_cMaxBonesPerFace;

	DWORD m_ExportDataOption;

	BOOL m_bSavePatchData;
	BOOL m_bLoopingAnimationData;
	DWORD m_iAnimSamplingRate;
	DWORD m_iAnimStart;
	DWORD m_iAnimEnd;

	BOOL m_bLeftHandAxis;
	BOOL m_bHideBiped;
	BOOL m_bTexEncrypt;

    SDialogOptions()
        : m_bProceedWithExport( FALSE )
        , m_xFormat( DXFILEFORMAT_BINARY )
        , m_cMaxBonesPerVertex( 0 )
	    , m_cMaxBonesPerFace( 0 )

	    , m_ExportDataOption( EXPD_FRAME | EXPD_ANIMATION )

	    , m_bSavePatchData( FALSE )
	    , m_bLoopingAnimationData( TRUE )
	    , m_iAnimSamplingRate( 30 )
		, m_iAnimStart( 0 )
		, m_iAnimEnd( 0 )

	    , m_bLeftHandAxis( TRUE )
	    , m_bHideBiped( TRUE )
	    , m_bTexEncrypt( FALSE )
    {
    }
};

struct SBoneInfo
{
    INode* m_pBoneNode;
    DWORD m_cVertices;

    SBoneInfo()
        : m_pBoneNode( NULL )
        , m_cVertices( 0 )
    {
    }
};

struct SSkinMap
{
    INode* m_pMeshNode;

    SBoneInfo* m_rgbiBones;
    DWORD m_cbiBones;
    DWORD m_cbiBonesMax;

    SSkinMap()
        : m_pMeshNode( NULL )
        , m_rgbiBones( NULL )
        , m_cbiBones( 0 )
        , m_cbiBonesMax( 0 )
    {
    }

    ~SSkinMap()
    {
        delete[] m_rgbiBones;
    }

    SBoneInfo *FindBone(INode *pBoneNode)
    {
        SBoneInfo *pbi = NULL;
        DWORD iBone;

        for (iBone = 0; iBone < m_cbiBones; iBone++)
        {
            if (pBoneNode == m_rgbiBones[iBone].m_pBoneNode)
            {
                pbi = &m_rgbiBones[iBone];
                break;
            }
        }

        return pbi;
    }

    HRESULT AddBone(INode *pBoneNode, SBoneInfo **ppbiBoneInfo)
    {
        HRESULT hr = S_OK;
        SBoneInfo *rgbiTemp;

        // reallocate if neccessary
        if (m_cbiBones == m_cbiBonesMax)
        {
            m_cbiBonesMax = max(1, m_cbiBonesMax);
            m_cbiBonesMax *= 2;

            rgbiTemp = m_rgbiBones;
            m_rgbiBones = new SBoneInfo[m_cbiBonesMax];
            if (m_rgbiBones == NULL)
            {
                m_rgbiBones = rgbiTemp;
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }

            if (m_cbiBones > 0)
            {
                memcpy(m_rgbiBones, rgbiTemp, m_cbiBones * sizeof(SBoneInfo));
            }

            delete[] rgbiTemp;
        }

        // not initialize the next bone in the array and return a pointer to it

        m_rgbiBones[m_cbiBones].m_cVertices = 0;
        m_rgbiBones[m_cbiBones].m_pBoneNode = pBoneNode;

        *ppbiBoneInfo = &m_rgbiBones[m_cbiBones];

        m_cbiBones += 1;

e_Exit:
        return hr;
    }
};

struct SPreprocessContext
{
    BOOL m_bSaveSelection;

    SSkinMap** m_rgpsmSkinMaps;
    DWORD m_cpsmSkinMaps;
    DWORD m_cpsmSkinMapsMax;

    DWORD m_cMaxWeightsPerVertex;
    DWORD m_cMaxWeightsPerFace;

    DWORD m_cNodes;

    SPreprocessContext()
        : m_bSaveSelection( FALSE )

        , m_rgpsmSkinMaps( NULL )
        , m_cpsmSkinMaps( 0 )
        , m_cpsmSkinMapsMax( 0 )

        , m_cMaxWeightsPerVertex( 0 )
        , m_cMaxWeightsPerFace( 0 )

        , m_cNodes( 0 )
    {
    }

    ~SPreprocessContext()
    {
        delete[] m_rgpsmSkinMaps;
    }
};

const int x_cbStringBufferMax = 4088;

struct SStringBlock
{
	SStringBlock* m_psbNext;
	DWORD m_cbData;

	TCHAR szData[ x_cbStringBufferMax ];

    SStringBlock()
		: m_psbNext( NULL )
        , m_cbData( 0 ) 
    {
    }

    ~SStringBlock()
	{
		delete m_psbNext;
	}
};

class CStringTable
{
private:
	SStringBlock* m_psbHead;

public:
	CStringTable()
		: m_psbHead( NULL )
    {
    }

	~CStringTable()
	{
		delete m_psbHead;
	}

	// allocate a string out of the data blocks to be free'd later, and make it a valid
	//   x-file name at the same time
	TCHAR *CreateNiceString(TCHAR *szString)
    {
        TCHAR* szNewString = NULL;
		BOOL bFirstCharIsDigit;
		DWORD cbLength;
		SStringBlock *psbNew;

        if (szString == NULL)
            return NULL;

        cbLength = (DWORD)_tcslen(szString) + 1;

        bFirstCharIsDigit = _istdigit(*szString);
        if (bFirstCharIsDigit)
        {
            cbLength += 1;
        }

		// if no string blocks or the current doesn't have enough space, then allocate one
		if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
		{
			psbNew = new SStringBlock();
			if (psbNew == NULL)
				return NULL;

			psbNew->m_psbNext = m_psbHead;
			m_psbHead = psbNew;
		}

		// allocate a string out of the data block
        szNewString = m_psbHead->szData + m_psbHead->m_cbData;
		m_psbHead->m_cbData += cbLength;

		// deal with the fact that the string can't start with digits
        *szNewString = _T('\0');
        if( bFirstCharIsDigit ) 
        {
            _tcscat(szNewString, _T("_"));
        }

        _tcscat(szNewString, szString);

        TCHAR* pchCur = szNewString;
        while( NULL != *pchCur )
        {
            if( *pchCur != _T('_') && !_istalnum(*pchCur) )
            {
                *pchCur = _T('_');
            }
            pchCur++;
        }
        return szNewString;
    }

	// Allocate a new string with '\\' in place of '\' characters
	TCHAR* CreateNiceFilename(TCHAR *szString)
	{
		TCHAR* szNewString = NULL;
		DWORD cbNameLength;
		DWORD cbLength;
		TCHAR* pchCur;
		TCHAR* pchOrig;
		SStringBlock *psbNew;

		if( NULL == szString )
		{
			return NULL;
		}

		cbNameLength = (DWORD)_tcslen(szString);
		cbLength = cbNameLength*2 + 1;

		// if no string blocks or the current doesn't have enough space, then allocate one
		if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
		{
			psbNew = new SStringBlock();
			if (psbNew == NULL)
				return NULL;

			psbNew->m_psbNext = m_psbHead;
			m_psbHead = psbNew;
		}

		// allocate a string out of the data block
        szNewString = m_psbHead->szData + m_psbHead->m_cbData;
		m_psbHead->m_cbData += cbLength;

		pchCur = szNewString;
		pchOrig = szString;
		while (NULL != *pchOrig)
		{
			if( _T('\\') == *pchOrig )
			{
				*(pchCur++) = _T('\\');
				*(pchCur++) = _T('\\');
			}
			else
			{
				*(pchCur++) = *pchOrig;
			}
			pchOrig++;
		}
		*pchCur = _T('\0');

		return szNewString;
	}

	// Allocate a new string without fiddling with the '\' characters
	TCHAR* CreateNormalFilename(TCHAR *szString)
	{
		TCHAR* szNewString = NULL;
		DWORD cbNameLength;
		DWORD cbLength;
		SStringBlock *psbNew;

		if( NULL == szString )
		{
			return NULL;
		}

		cbNameLength = (DWORD)_tcslen(szString);
		cbLength = cbNameLength + 1;

		// if no string blocks or the current doesn't have enough space, then allocate one
		if ((m_psbHead == NULL) || ((x_cbStringBufferMax - m_psbHead->m_cbData) < cbLength))
		{
			psbNew = new SStringBlock();
			if (psbNew == NULL)
				return NULL;

			psbNew->m_psbNext = m_psbHead;
			m_psbHead = psbNew;
		}

		// allocate a string out of the data block
        szNewString = m_psbHead->szData + m_psbHead->m_cbData;
		m_psbHead->m_cbData += cbLength;

        memcpy(szNewString, szString, cbLength);

		return szNewString;
	}
};

struct SSaveContext
{
    LPDIRECTXFILESAVEOBJECT m_pxofsave;
    DXFILEFORMAT m_xFormat;
    DWORD m_iTime;
    BOOL m_bSaveSelection;
    BOOL m_bSavePatchData;
    
	DWORD m_ExportDataOption;
    
	BOOL m_bLoopingAnimationData;
    DWORD m_iAnimSamplingRate;
	DWORD m_iAnimStart;
	DWORD m_iAnimEnd;
    DWORD m_cMaxWeightsPerVertex;
    DWORD m_cMaxWeightsPerFace;

    SSkinMap** m_rgpsmSkinMaps;
    DWORD m_cpsmSkinMaps;

    LPDIRECTXFILEDATA m_pAnimationSet;
    Interface* m_pInterface;

    DWORD m_cNodes;
    DWORD m_cNodesCur;
    INode** m_rgpnNodes;

	CStringTable m_stStrings;

	BOOL m_bLeftHandAxis;
	BOOL m_bHideBiped;
	BOOL m_bTexEncrypt;

    SSaveContext()
        : m_pxofsave( NULL )
        , m_xFormat( DXFILEFORMAT_BINARY )
        , m_iTime( 0 )
        , m_bSaveSelection( FALSE )
        , m_bSavePatchData( FALSE )
        
	    , m_ExportDataOption( EXPD_FRAME | EXPD_ANIMATION )
        
	    , m_bLoopingAnimationData( TRUE )
        , m_iAnimSamplingRate( 0 )
		, m_iAnimStart( 0 )
		, m_iAnimEnd( 0 )
        , m_cMaxWeightsPerVertex( 0 )
        , m_cMaxWeightsPerFace( 0 )

        , m_rgpsmSkinMaps( NULL )
        , m_cpsmSkinMaps( 0 )

        , m_pAnimationSet( NULL )
        , m_pInterface( NULL )

        , m_cNodes( 0 )
        , m_cNodesCur( 0 )
        , m_rgpnNodes( NULL )

	    , m_bLeftHandAxis( TRUE )
	    , m_bHideBiped( TRUE )
	    , m_bTexEncrypt( FALSE )
    {
    }

    ~SSaveContext()
    {
        delete[] m_rgpsmSkinMaps;
    }

    SSkinMap *GetSkinMap(INode *pMeshNode)
    {
        SSkinMap *psm = NULL;
        DWORD iMesh;

        for (iMesh = 0; iMesh < m_cpsmSkinMaps; iMesh++)
        {
            if (pMeshNode == m_rgpsmSkinMaps[iMesh]->m_pMeshNode)
            {
                psm = m_rgpsmSkinMaps[iMesh];
                break;
            }
        }

        return psm;
    }
};