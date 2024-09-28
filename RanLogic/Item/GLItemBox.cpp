#include "../pch.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "./GLItemBox.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ITEM
{

SBOX::SBOX()
    : ShowContents(false)
{
    CLEAR ();
}

void SBOX::CLEAR ()
{
    for ( int i=0; i<(int)vecItems.size(); ++i )
    {
        vecItems[i].nidITEM = SNATIVEID(false);
        vecItems[i].dwAMOUNT = 0;
    }
}

bool SBOX::INSERT ( const SNATIVEID &nidITEM, DWORD dwAMOUNT )
{
    if ( nidITEM==SNATIVEID(false) )	return false;

	if ( (int)vecItems.size() >= ITEM_SIZE )
		return false;

	SBOX_ITEM sBox;
	sBox.nidITEM = nidITEM;
	sBox.dwAMOUNT = dwAMOUNT;
	vecItems.push_back(sBox);
	return true;
}

bool SBOX::DEL ( int nIndex )
{
    if ( nIndex < 0 )				return false;
    if ( ITEM_SIZE <= nIndex )		return false;

	if ( (int)vecItems.size() <= nIndex )
		return false;

//	vecItems[nIndex].nidITEM = SNATIVEID(false);
//	vecItems[nIndex].dwAMOUNT = 1;
//	VECBOX_CITR itr = vecItems.at(nIndex);
//	vecItems.erase(itr);
	vecItems.erase(vecItems.begin()+nIndex);

    return true;
}

bool SBOX::FIND ( const SNATIVEID &nidITEM, IN OUT int& nIndex, IN OUT int& nAmount ) const
{
	if ( nidITEM==SNATIVEID(false) )	return false;

	int nSize( min((int)vecItems.size(), ITEM_SIZE) );
	for ( int i=0; i<nSize && vecItems[i].nidITEM!=SNATIVEID(false); ++i )
	{
		if ( vecItems[i].nidITEM==nidITEM )
		{
			nIndex = i;
			nAmount = vecItems[i].dwAMOUNT;
			return true;
		}
	}
	return false;
}

bool SBOX::VALID() const
{
	if ( (int)vecItems.size() > 0 && vecItems[0].nidITEM!=SNATIVEID(false))
		return true;

    return false;
}

BOOL SBOX::LOAD ( sc::BaseStream &SFile )
{
    DWORD dwVER(0), dwSIZE(0);
    SFile >> dwVER;
    SFile >> dwSIZE;

    switch (dwVER)
    {
    case VERSION:
        {
			SBOX_ITEM	_sITEMS;
			DWORD dwNUM(0);
			SFile >> dwNUM;
			for ( DWORD i=0; i<dwNUM; ++i )
			{
				SFile.ReadBuffer ( &_sITEMS, sizeof(SBOX_ITEM) );
				vecItems.push_back ( _sITEMS );
			}
			SFile >> ShowContents;
//            SFile.ReadBuffer( sITEMS, sizeof(SBOX_ITEM)*ITEM_SIZE );
//            SFile >> ShowContents;
        }
        break;
	case 0x0103:
		{
			SBOX_ITEM	_sITEMS[ITEM_PREV_SIZE];
			SFile.ReadBuffer( _sITEMS, sizeof(SBOX_ITEM)*ITEM_PREV_SIZE );
			SFile >> ShowContents;
			//SNATIVEID sFalseID(false);
			for ( int i = 0; i < ITEM_PREV_SIZE /*&& _sITEMS[i].nidITEM != sFalseID*/; ++i )
				vecItems.push_back(_sITEMS[i]);
		}
		break;

    case 0x0102:
        {
			SBOX_ITEM	_sITEMS[ITEM_PREV_SIZE];
            SFile.ReadBuffer ( _sITEMS, sizeof(SBOX_ITEM)*ITEM_PREV_SIZE );
			//SNATIVEID sFalseID(false);
			for ( int i = 0; i < ITEM_PREV_SIZE /*&& _sITEMS[i].nidITEM != sFalseID*/; ++i )
				vecItems.push_back(_sITEMS[i]);
        }
        break;

    case 0x0101:
        {			
            SBOX_ITEM_101 _sOLD_ITEMS[ITEM_PREV_SIZE];
            SFile.ReadBuffer ( _sOLD_ITEMS, sizeof(SBOX_ITEM_101)*ITEM_PREV_SIZE );
			//SNATIVEID sFalseID(false);
            for ( int i=0; i<ITEM_PREV_SIZE /*&& _sOLD_ITEMS[i].nidITEM != sFalseID*/; ++i )
            {
				SBOX_ITEM sBox;
				sBox.nidITEM = _sOLD_ITEMS[i].nidITEM;
				sBox.dwAMOUNT = _sOLD_ITEMS[i].dwAMOUNT;
				vecItems.push_back(sBox);
            }			
        }
        break;	
    case 0x0100:
        {
            SNATIVEID _sOLD_ITEMS[ITEM_PREV_SIZE];
            SFile.ReadBuffer ( _sOLD_ITEMS, sizeof(DWORD)*ITEM_PREV_SIZE );
			//SNATIVEID sFalseID(false);
            for ( int i=0; i<ITEM_PREV_SIZE /*&& _sOLD_ITEMS[i] != sFalseID*/; ++i )
            {
				SBOX_ITEM sBox;
                sBox.nidITEM = _sOLD_ITEMS[i];
                sBox.dwAMOUNT = 1;
				vecItems.push_back(sBox);
            }
        }
        break;

    default:
        SFile.SetOffSet ( SFile.GetfTell()+dwSIZE );
        MessageBox ( NULL, "item/box unknown data version.", "ERROR", MB_OK );
        break;
    };

	if ( (int)vecItems.size() != 0 )
	{
		SNATIVEID sFalseID(false);
		for ( VECBOX_ITR itr = vecItems.begin(); itr != vecItems.end(); )
		{
			SBOX_ITEM sBox = *itr;
			if ( sBox.dwAMOUNT == 0 || sFalseID == sBox.nidITEM )
				itr = vecItems.erase(itr);
			else
				itr++;
		}
	}
    return TRUE;
}

BOOL SBOX::SAVE ( sc::SerialFile &SFile )
{
    SFile << DWORD(VERSION);
    SFile.BeginBlock();
    {
		SFile << (DWORD) vecItems.size();
		if ( vecItems.size() > 0 )
		{
			SFile.WriteBuffer ( &(vecItems[0]), DWORD(sizeof(SBOX_ITEM)*vecItems.size()) );
		}

        SFile << ShowContents;
    }
    SFile.EndBlock();

    return TRUE;
}

VOID SBOX::SaveCsvHead ( std::fstream &SFile )
{
    for( int i=0; i<ITEM_SIZE; ++i )
    {
        SFile << "sITEMS " << i << " nidITEM wMainID" << ",";
        SFile << "sITEMS " << i << " nidITEM wSubID" << ",";
        SFile << "sITEMS " << i << " dwAMOUNT" << ",";
    }

    SFile << "Show Contents" << ",";
}

VOID SBOX::SaveCsv ( std::fstream &SFile )
{
	const int nBoxSize((int)vecItems.size());
	
    for( int i=0; i<ITEM_SIZE; ++i )
    {
		if ( i < nBoxSize )
		{
			SFile << vecItems[i].nidITEM.wMainID << ",";
			SFile << vecItems[i].nidITEM.wSubID << ",";
			SFile << vecItems[i].dwAMOUNT << ",";
		}
		else
		{
			SFile << "0" << ",";
			SFile << "0" << ",";
			SFile << "0" << ",";
		}
    }

    SFile << ShowContents << ",";
}

VOID SBOX::LoadCsv(CStringArray& StrArray, int& iCsvCur)
{
	vecItems.clear();
    for( int i=0; i<ITEM_SIZE; ++i )
    {
		SBOX_ITEM _sBox;
		_sBox.nidITEM.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		_sBox.nidITEM.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		_sBox.dwAMOUNT = (DWORD)atol( StrArray[ iCsvCur++ ] );
		if ( _sBox.dwAMOUNT != 0 && _sBox.nidITEM != NATIVEID_NULL() )
			vecItems.push_back(_sBox);
    }

    ShowContents = ( 0!= atoi( StrArray[ iCsvCur++ ] ) );
}

} // namespace ITEM