#include "pch.h"

#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../SigmaCore/Hash/CHash.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/gltexfile.h"

#include "../GLogicFile.h"
#include "../GlogicDataMan.h"
#include "./RandomOption.h"

#include <shlwapi.h>

namespace RandomOption
{
	INT iCsvCur = 0;
	const int PTVaildCount(float fValue)
	{
		float fTemp;
		float fBelowPT = fValue//modf(fValue, &fTemp)
			, fCodiValue;

 		if ( (fCodiValue = fBelowPT*1000) != 0.0f && (float)modf(fCodiValue, &fTemp) > 0.099f )
 			return 4;
 		else if ( (fCodiValue = fBelowPT*100) != 0.0f && (float)modf(fCodiValue, &fTemp) > 0.099f )
 			return 3;
 		else if ( (fCodiValue = fBelowPT*10) != 0.0f && (float)modf(fCodiValue, &fTemp) > 0.099f )
 			return 2;

		return 1;
	}

	const std::string GetFloatString(float fValue)
	{
		INT nPTCount(PTVaildCount(fValue));
		CString str("%.");
		str.AppendFormat("%dff", nPTCount );
		str.Format(str.GetString(), fValue);
		return str;
	}

	const bool Manager::BinExtract(void)
	{
		char _cApplicationPath[1024];
		::GetModuleFileName(0, _cApplicationPath, 1024);	
		::PathRemoveFileSpec(_cApplicationPath);

		const std::string _stringPath(sc::string::format("%1%\\data\\glogicserver\\", _cApplicationPath));

		const RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);
		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
		{
			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
			{
				const RandomOption::Entity& _entity(*_iteratorEntity);
				const std::string _stringName(_entity.getName());

				// 존재하지 않는 경우에만 bin파일을 생선한다;
				// 해당 부분 분석이 필요한 경우, 데이터를 열고 보면서하면 쉽다;
				const std::string strCheckFile(_stringPath + _stringName);
				if ( sc::file::isExist(strCheckFile.c_str()) == false )
				{
					CString strBuffer;// %.4f
					strBuffer.Append("//	아이템 랜덤 옵션 설정.\n");
					strBuffer.Append("//	랜덤옵션 발생 확율.\n");
					strBuffer.AppendFormat("fRATE	%.1ff\n", _entity.getRateRandomOptionGenerate() );
					strBuffer.Append("//	개조 시도시 아이템이 소멸 할 수 있는 확률.\n");
					strBuffer.AppendFormat("D_point	%.1ff\n",  _entity.getRateRandomOptionDestroy());
					strBuffer.Append("//	개조시에 필요한 금액, Item의 단가에 곱한다.\n");
					strBuffer.AppendFormat("S_value	%.1ff\n",  _entity.getRateMoneySpend());
					strBuffer.AppendFormat("wMinGain	%d\n", _entity.GetMinGain());
					strBuffer.AppendFormat("wMaxGain	%d\n", _entity.GetMaxGain());

					strBuffer.Append("//\n//\n");
					TCHAR _cOptionHeader[RandomOption::GENERATE_NSIZE][24] = {"NEW_OPTION_", "REB_OPTION_"};
					TCHAR _cOptSeHeader[RandomOption::GENERATE_NSIZE][24] = {"NEW_OPT_SE_", "REB_OPT_SE_"};
					char _szHelpTip[RandomOption::GENERATE_NSIZE][1024] = { "// MOB 에게서 ITEM 이 DROP 될때 사용되어지는 RANDOM OPTION 확률"
						, "// NPC 를 통해서, RANDOM OPTION 을 개조할 때 사용하는 RANDOM OPTION 확률" };
					for ( unsigned int _indexType(0); _indexType < RandomOption::GENERATE_NSIZE; ++_indexType )
					{
						const RandomOption::ValueBunchVector& _vectorValueBunch( *_entity.getVectorValueBunch((GENERATE)_indexType) );
						strBuffer.AppendFormat("%s\n", _szHelpTip[_indexType]);
						const int nSize(_vectorValueBunch.size());
						for ( int i = 0; i < nSize; ++i )
						{
							TCHAR _cOptionFull[64];
							TCHAR _cOptSE[64];
							_snprintf_s(_cOptionFull, 64, _TRUNCATE, "%s%u", _cOptionHeader[_indexType], i + 1);
							_snprintf_s(_cOptSE, 64, _TRUNCATE, "%s%u_", _cOptSeHeader[_indexType], i + 1);

							const ValueBunch& sBunch = _vectorValueBunch[i];
							const ValueVector& vecValue = sBunch.getValueVector();
							const int nSizeSelection(vecValue.size());
							
							strBuffer.AppendFormat("%s	%.1ff, %d, %d	\n"
								, _cOptionFull
								, sBunch.getRateChoose()
								, sBunch.getTypeRandomOption()
								, nSizeSelection);

							for ( int _indexOptSE(0); _indexOptSE < nSizeSelection; ++_indexOptSE )
							{
								TCHAR _cOptSEFull[64];
								_snprintf_s(_cOptSEFull, 64, _TRUNCATE, "%s%u", _cOptSE, _indexOptSE + 1);

								const RandomOption::Value& _pValue(vecValue[_indexOptSE]);
								if ( RandomOption::bValueInteger[sBunch.getTypeRandomOption()] )
								{
									strBuffer.AppendFormat("%s	%.1ff, %d, %d\n"
										, _cOptSEFull
										, _pValue.getRateChoose() * 100.0f
										, _pValue.getValueIntegerHigh()
										, _pValue.getValueIntegerLow() );
								}
								else
								{
									strBuffer.AppendFormat("%s	%.1ff, %.4ff, %.4ff\n"
										, _cOptSEFull
										, _pValue.getRateChoose() * 100.0f
										, _pValue.getValueFloatHigh()
										, _pValue.getValueFloatLow() );
// 									strBuffer.AppendFormat("%s	%.1ff, %s, %s\n"
// 										, _cOptSEFull
// 										, _pValue.getRateChoose() * 100.0f
// 										, GetFloatString(_pValue.getValueFloatHigh()).c_str()
// 										, GetFloatString(_pValue.getValueFloatLow()).c_str());
								}
							}
							strBuffer.Append("//\n");
						}
					}

					std::string strCreateDir(_stringPath + "\\RamdomopTemp\\");
					sc::file::createDirectory(strCreateDir.c_str());
					std::string strCreatePath(strCreateDir + _stringName);
					// 파일 생성;
					sc::CRijndael oRijndael;
					oRijndael.Initialize(sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

					int nLen = oRijndael.GetEncryptStringLength( strBuffer );
					char * szBuffer = new char[nLen];
					memset( szBuffer, 0, nLen );

					oRijndael.EncryptEx( strBuffer.GetString(), szBuffer, strBuffer.GetLength() );
					int nVersion = sc::CRijndael::VERSION;

					// we'll use a CFileException object to get error information
					CFileException ex;

					CFile file;
					if ( !file.Open( strCreatePath.c_str(), CFile::modeCreate|CFile::modeWrite, &ex ) )
					{
						TCHAR szError[1024];
						ex.GetErrorMessage(szError, 1024);
						_tprintf_s(_T("Couldn't open source file: %1024s"), szError);
						::MessageBox(0, szError, "error", MB_OK);

						delete [] szBuffer;
						return FALSE;
					}

					file.Write( &nVersion, sizeof(int) );	// 버젼 쓰기;
					file.Write( szBuffer, nLen );			// 파일 쓰기;
					file.Close();
				}
			}


		}
	}

	const bool Entity::loadCsv(CStringArray& StrArray)
	{
		//Generate Rate, Destroy point, Spend, wMinGain, wMaxGain
		this->fRateRandomOptionGenerate = atof(StrArray[1]);
		this->fRateRandomOptionDestroy = atof(StrArray[2]);
		this->fRateMoneySpend = atof(StrArray[3]);
		this->wMinGain = atoi(StrArray[4]);
		this->wMaxGain = atoi(StrArray[5]);
		
		GENERATE emGenerate((GENERATE)atoi(StrArray[6]));
		RandomOption::ValueBunchVector& _vectorValueBunch(
			emGenerate == GENERATE_DROP ? this->vectorValueBunchDrop : this->vectorValueBunchRemodel);
		
		const int nSize(atoi(StrArray[7]))
			, nIndex(atoi(StrArray[8]));
		if ( _vectorValueBunch.size() == 0 )
			_vectorValueBunch.resize(nSize);
		if ( nSize <= nIndex )
		{
			const std::string _stringError(sc::string::format("failed to load for Entity::loadCsv : %1% (nSize <= nIndex)", this->cName));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}
		
		RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[nIndex]);
		_valueBunch.loadCsv(StrArray);
		_valueBunch.refreshValue();

		return true;
	}

	const bool Entity::saveCsv(std::fstream& SFile) const
	{
		std::string _stringName(this->cName);
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);

		const RandomOption::ValueBunchVector& _vectorValueBunchDrop(this->vectorValueBunchDrop);
		const unsigned int _sizeValueBunchDrop(_vectorValueBunchDrop.size());
		for ( unsigned int _index(0); _index < _sizeValueBunchDrop; ++_index )
		{
			SFile << _stringName << ",";
			SFile << this->fRateRandomOptionGenerate << ",";
			SFile << this->fRateRandomOptionDestroy << ",";
			SFile << this->fRateMoneySpend << ",";
			SFile << this->wMinGain << ",";
			SFile << this->wMaxGain << ",";
			SFile << (int)RandomOption::GENERATE_DROP << "," << _sizeValueBunchDrop << "," << _index << ",";

			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunchDrop[_index]);
			if ( _valueBunch.saveCsv(SFile) == false )
				return false;

			SFile << std::endl;
		}

		const RandomOption::ValueBunchVector& _vectorValueBunchRemodel(this->vectorValueBunchRemodel);
		const unsigned int _sizeValueBunchRemodel(_vectorValueBunchRemodel.size());
		for ( unsigned int _index(0); _index < _sizeValueBunchRemodel; ++_index )
		{
			SFile << _stringName << ",";
			SFile << this->fRateRandomOptionGenerate << ",";
			SFile << this->fRateRandomOptionDestroy << ",";
			SFile << this->fRateMoneySpend << ",";
			SFile << this->wMinGain << ",";
			SFile << this->wMaxGain << ",";
			SFile << (int)RandomOption::GENERATE_REMODEL << "," << _sizeValueBunchRemodel << "," << _index << ",";

			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunchRemodel[_index]);
			if ( _valueBunch.saveCsv(SFile) == false )
				return false;

			SFile << std::endl;
		}

		return true;
	}

	const bool Value::saveCsv(std::fstream& SFile, bool bInteger) const
	{
		SFile << this->fRateChoose << ",";
		bInteger ? 
			(SFile<< this->valueIntegerLow << "," << this->valueIntegerHigh << ",")
			:(SFile<< this->valueFloatLow << "," << this->valueFloatHigh << ",");
		return true;
	}

	const bool Value::loadCsv(CStringArray& StrArray, bool bInteger)
	{
		this->fRateChoose = atof(StrArray[iCsvCur++]);
		if ( bInteger == true )
		{
			this->valueIntegerLow = atoi(StrArray[iCsvCur++]);
			this->valueIntegerHigh = atoi(StrArray[iCsvCur++]);
		}
		else
		{
			this->valueFloatLow = atof(StrArray[iCsvCur++]);
			this->valueFloatHigh = atof(StrArray[iCsvCur++]);
		}
		return true;
	}

	const bool ValueBunch::saveCsv(std::fstream& SFile) const
	{
		SFile << this->fRateChoose << ",";
		SFile << unsigned int(this->typeRandomOption) << ",";
		const unsigned int _sizeValue(this->vectorValue.size());
		SFile << _sizeValue << ",";
		for ( unsigned int _index(0); _index < _sizeValue; ++_index )
		{
			const RandomOption::Value& _value(this->vectorValue[_index]);
			_value.saveCsv(SFile, RandomOption::bValueInteger[typeRandomOption]);
		}
		return true;
	}

	const bool ValueBunch::loadCsv(CStringArray& StrArray)
	{
		const int nValueSize(atoi(StrArray[11]));
		this->vectorValue.resize(nValueSize);

		this->fRateChoose		= atof(StrArray[9]);
		this->typeRandomOption	= (RANDOM_OPTION)atoi(StrArray[10]);

		iCsvCur = 12;
		for ( INT i = 0; i < nValueSize; ++i )
		{
			RandomOption::Value& _value(this->vectorValue[i]);
			_value.loadCsv(StrArray, RandomOption::bValueInteger[typeRandomOption]);
		}
		return true;
	}

	void Manager::saveCsvHead(std::fstream& SFile) const
	{
		SFile << "Entity" << ",";			// 0
		SFile << "Generate Rate" << ",";	// 1 
		SFile << "Destroy point" << ",";	// 2
		SFile << "Spend" << ",";			// 3
		SFile << "wMinGain" << ",";			// 4
		SFile << "wMaxGain" << "," ;		// 5
		SFile << "Generate Type" << ",";	// 6
		SFile << "Bunch Size" << ",";		// 7
		SFile << "Bunch Index" << ",";		// 8

		SFile << "Opt Rate" << "," << "Opt Type" << "," << "Sector Num" << ",";
		SFile << "Rate1" << ","  << "Min 1" << "," << "Max 1" << ",";
		SFile << "Rate2" << ","  << "Min 2" << "," << "Max 2" << ",";
		SFile << "Rate3" << ","  << "Min 3" << "," << "Max 3" << ",";
		SFile << "Rate4" << ","  << "Min 4" << "," << "Max 4" << ",";
		SFile << "Rate5" << ","  << "Min 5" << "," << "Max 5" << ",";
		SFile << "Rate6" << ","  << "Min 6" << "," << "Max 6" << ",";
		SFile << "Rate7" << ","  << "Min 7" << "," << "Max 7" << ",";
		SFile << "Rate8" << ","  << "Min 8" << "," << "Max 8" << ",";
		SFile << "Rate9" << ","  << "Min 9" << "," << "Max 9" << ",";
		SFile << "Rate10" << "," << "Min 10" << ","<< "Max 10"<< ",";

		SFile << std::endl;
	}

	const bool Manager::loadCsv(CWnd* pWnd)
	{
		// Clear;
		this->clear();

		CFileDialog dlg( TRUE, ".csv", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			"csv file (*.csv)|*.csv|", pWnd );

		dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
		if ( dlg.DoModal() != IDOK )
			return S_FALSE;

		sc::CStringFile StrFile( 10240 );
		if( !StrFile.Open ( dlg.GetPathName().GetString() ) )
			return S_FALSE;

		// 구분자 지정;
		STRUTIL::ClearSeparator();
		STRUTIL::RegisterSeparator( "," );

		CString strLine;
		CStringArray StrArray;

		// 첫줄은 CSV Head;
		StrFile.GetNextLine( strLine );
		STRUTIL::StringSeparate( strLine, StrArray );

		int iLine = 1;
		char szError[ 256 ];

		while( StrFile.GetNextLine( strLine ) )
		{
			++iLine;

			STRUTIL::StringSeparate( strLine, StrArray, TRUE );

			// name;
			std::string _stringName(StrArray[0]);
			RandomOption::Entity* const _pEntity(Manager::create(_stringName.c_str()));
			if ( _pEntity == NULL )
			{
				const std::string _stringError(sc::string::format("RandomOption::Manager::loadCsv _ can't create entity : %1%", _stringName));
				::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				return false;
			}
			_pEntity->loadCsv(StrArray);
		}

		// CSV로드 이후 데이터에 존재하지않는 파일이 있다면;
		// 해당 파일들만 골라내어 생성한다;
		this->BinExtract();

		return true;
	}

	const bool Manager::saveCsv(CWnd* pWnd)
	{
		CFileDialog dlg(
			FALSE,
			".csv",
			NULL,
			OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
			"csv file (*.csv)|*.csv|",
			pWnd);

		// 폴더나 파일명이 한글이면 fstream 문제발생함;
		// 임시로 setlocale 을 이용해서 문제 해결;
		// 추후에 fstream 을 이용하지 않는 방식으로 수정해야 함;
		setlocale(LC_ALL, "Korean");

		std::string InitPath(GLOGIC::GetPath());
		dlg.m_ofn.lpstrInitialDir = InitPath.c_str();
		if (dlg.DoModal() != IDOK)
			return false;

		std::string FileName(dlg.GetPathName().GetString());
		std::fstream streamFILE;
		streamFILE.open(FileName.c_str(), std::ios_base::out);

		this->saveCsvHead(streamFILE);

		const RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);
		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
		{
			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
			{
				const RandomOption::Entity& _entity(*_iteratorEntity);
				const std::string _stringName(_entity.getName());
				if ( _entity.saveCsv(streamFILE) == false )
				{
					const std::string _stringError(sc::string::format("failed to save for csv : %1%", _stringName));
					::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				}
			}
		}
		streamFILE.close();
		return true;
	}
}