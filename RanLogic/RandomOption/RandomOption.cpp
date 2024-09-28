#include "pch.h"

#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../SigmaCore/Hash/CHash.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/Common/gltexfile.h"
#include "../GLogicFile.h"
#include "../GlogicDataMan.h"
#include "./RandomOption.h"

#include <shlwapi.h>


const char* RandomOption::Manager::_FILEHEADSERVER = "RandomOptionServer";
const char* RandomOption::Manager::_FILEHEADCLIENT = "RandomOptionClient";
namespace RandomOption
{
	extern HRESULT LoadFile(const RandomOption::LOAD_TYPE _typeLoad, GLogicFile* const _pFileLogic, const bool _bTool)
	{
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		switch ( _typeLoad )
		{
//  		case RandomOption::LOAD_CLIENT:
//  			if ( _pFileLogic == 0 )
//  				return E_FAIL;
//  			if ( _managerRandomOption.loadFileClient(_pFileLogic) == false )
//  				return E_FAIL;
//  			return S_OK;
//  		case RandomOption::LOAD_SERVER:
//  			if ( _pFileLogic == 0 )
//  				return E_FAIL;
//  			if ( _managerRandomOption.loadFileServer(_pFileLogic, _bTool) == false )
//  				return E_FAIL;
//  			return S_OK;
 		case RandomOption::LOAD_CLIENT:
 		case RandomOption::LOAD_SERVER:
 			if ( _pFileLogic == 0 )
 				return E_FAIL;
 			if ( _managerRandomOption.loadFile(_pFileLogic, _bTool) == false )
 				return E_FAIL;
 			return S_OK;
		case RandomOption::LOAD_TOOL:
			if ( _managerRandomOption.loadFileTool() == false )
				return E_FAIL;
			return S_OK;
		}

		return E_FAIL;
	}
	HRESULT LoadFileOld(const bool _bConvert)
	{
		char _cApplicationPath[256];
		::GetModuleFileName(0, _cApplicationPath, 256);	
		::PathRemoveFileSpec(_cApplicationPath);

		const std::string _stringPathServer(std::string(_cApplicationPath) + std::string("\\data\\glogicserver\\"));
		const std::string _stringPathFull(_stringPathServer + "glogicserver.rcc");

		ZipMan& _managerZip(ZipMan::GetInstance());
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		if ( _managerRandomOption.size() == 0 )
			_managerZip.LOADFILE_RCC(_stringPathFull);

		_managerRandomOption.clear();

		const ZipMan::VECTOR_FILE_NAME* const _pVectorFileName(_managerZip.getFileListByExtension("bin"));
		if ( _pVectorFileName == 0 )
			return E_FAIL;

		// .bin file의 경우 random-option file 외에도 있으므로 유의 하여야 한다;
		const ZipMan::VECTOR_FILE_NAME& _vectorFileName(*_pVectorFileName);		

		const unsigned int _sizeFileRandomOption(_vectorFileName.size());
		for ( unsigned int _index(_sizeFileRandomOption); _index--; )
		{
			gltexfile _fileText;
			_fileText.reg_sep("\t ,");
			_fileText.SetZipFile(_stringPathFull);

			const std::string& _stringNameFile(_vectorFileName[_index]);
			if ( _fileText.open(_stringPathServer + _stringNameFile, true) == false )
			{
				const std::string _stringError(sc::string::format("can't find file : %1%", _stringNameFile));
				::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				return E_FAIL;
			}

			if ( _managerRandomOption.loadFile_old(_fileText, _stringNameFile, _bConvert) == false )
				return E_FAIL;			
		}

		return S_OK;
	}



	Value::Value(const RandomOption::Value& _rhs)
		: fRateChoose(_rhs.fRateChoose)
		, valueIntegerHigh(_rhs.valueIntegerHigh)
		, valueIntegerLow(_rhs.valueIntegerLow)
	{
	}
	Value::Value(const float _fRateChoose, const int _valueIntegerLow, const int _valueIntegerHigh)
		: fRateChoose(_fRateChoose)
		, valueIntegerHigh(_valueIntegerHigh)
		, valueIntegerLow(_valueIntegerLow)
	{
	}
	Value::Value(const float _fRateChoose, const float _valueFloatLow, const float _valueFloatHigh)
		: fRateChoose(_fRateChoose)
		, valueFloatHigh(_valueFloatHigh)
		, valueFloatLow(_valueFloatLow)
	{
	}
	Value::Value(void)
		: fRateChoose(0.0f)
		, valueIntegerHigh(0)
		, valueIntegerLow(0)
	{
	}
	const bool Value::saveFile(sc::SerialFile& _fileStreamServer) const
	{
		_fileStreamServer << this->fRateChoose;
		_fileStreamServer << this->valueIntegerHigh;
		_fileStreamServer << this->valueIntegerLow;

		return true;
	}

	const bool Value::loadFile(sc::BaseStream& _fileStream)
	{
		_fileStream >> this->fRateChoose;
		_fileStream >> this->valueIntegerHigh;
		_fileStream >> this->valueIntegerLow;

		return true;
	}
	const bool Value::loadFile_old(gltexfile& _fileText, const TCHAR* const _cOptSEFull, RandomOption::ValueTemporary& _valueTemporary, const bool _bValueInteger)
	{
		_fileText.getflag(_cOptSEFull, 1, 3, this->fRateChoose, true);
		this->fRateChoose *= 0.01f;			

		if ( _bValueInteger == true )
		{
			_fileText.getflag(_cOptSEFull, 2, 3, this->valueIntegerHigh, true);
			_fileText.getflag(_cOptSEFull, 3, 3, this->valueIntegerLow, true);
			if ( this->valueIntegerHigh > _valueTemporary.valueIntegerHighCompare )
				_valueTemporary.valueIntegerHighCompare = this->valueIntegerHigh;
			if ( this->valueIntegerLow < _valueTemporary.valueIntegerLowCompare )
				_valueTemporary.valueIntegerLowCompare = this->valueIntegerLow;
		}
		else
		{
			_fileText.getflag(_cOptSEFull, 2, 3, this->valueFloatHigh, true);
			_fileText.getflag(_cOptSEFull, 3, 3, this->valueFloatLow, true);
			if ( this->valueFloatHigh > _valueTemporary.valueFloatHighCompare )
				_valueTemporary.valueFloatHighCompare = this->valueFloatHigh;
			if ( this->valueFloatLow < _valueTemporary.valueFloatLowCompare )
				_valueTemporary.valueFloatLowCompare = this->valueFloatLow;
		}

		return true;
	}





	ValueBunch::ValueBunch(const RandomOption::ValueBunch& _rhs)
		: fRateChoose(_rhs.fRateChoose)
		, typeRandomOption(_rhs.typeRandomOption)
		, valueIntegerHigh(_rhs.valueIntegerHigh)
		, valueIntegerLow(_rhs.valueIntegerLow)
		, vectorValue(_rhs.vectorValue)
	{
	}
	ValueBunch::ValueBunch(const float _fRateChoose, const RandomOption::RANDOM_OPTION _typeRandomOption)
		: fRateChoose(_fRateChoose)		
		, typeRandomOption(_typeRandomOption)
		, valueIntegerHigh(0)
		, valueIntegerLow(0)
	{
	}
	ValueBunch::ValueBunch(void)
		: fRateChoose(0.0f)
		, typeRandomOption(RandomOption::RANDOM_OPTION_NULL)
		, valueIntegerHigh(0)
		, valueIntegerLow(0)
	{
	}
	void ValueBunch::_rebalance(void)
	{
		const bool _bValueInteger(RandomOption::bValueInteger[this->typeRandomOption]);

		this->vectorValue.clear();

		const float _fValueHighest((_bValueInteger == true ? float(this->valueIntegerHigh) : this->valueFloatHigh));
		const float _fValueLowest(_bValueInteger == true ? float(this->valueIntegerLow) : this->valueFloatLow);
		const float _fValueGap(_fValueHighest - _fValueLowest);

		const float _fValue70(_fValueLowest + _fValueGap * 0.6f);
		const float _fValue80(_fValueLowest + _fValueGap * 0.7f);

		if ( _bValueInteger == true )
		{
			const RandomOption::Value _value0(0.95f, int(_fValueLowest), int(_fValue70));
			const RandomOption::Value _value1(0.04f, int(_fValue70), int(_fValue80));
			const RandomOption::Value _value2(0.01f, int(_fValue80), int(_fValueHighest));
			this->vectorValue.push_back(_value0);
			this->vectorValue.push_back(_value1);
			this->vectorValue.push_back(_value2);

			this->valueIntegerHigh = int(_fValueHighest);
			this->valueIntegerLow = int(_fValueLowest);
		}
		else
		{
			const RandomOption::Value _value0(0.95f, _fValueLowest, _fValue70);
			const RandomOption::Value _value1(0.04f, _fValue70, _fValue80);
			const RandomOption::Value _value2(0.01f, _fValue80, _fValueHighest);
			this->vectorValue.push_back(_value0);
			this->vectorValue.push_back(_value1);
			this->vectorValue.push_back(_value2);

			this->valueFloatHigh = _fValueHighest;
			this->valueFloatLow = _fValueLowest;
		}
	}
	const bool ValueBunch::saveFileServer(sc::SerialFile& _fileStreamServer) const
	{
		_fileStreamServer << this->fRateChoose;
		_fileStreamServer << unsigned int(this->typeRandomOption);
		_fileStreamServer << this->valueIntegerHigh;
		_fileStreamServer << this->valueIntegerLow;

		const unsigned int _sizeValue(this->vectorValue.size());
		_fileStreamServer << _sizeValue;
		for ( unsigned int _index(0); _index < _sizeValue; ++_index )
		{
			const RandomOption::Value& _value(this->vectorValue[_index]);
			_value.saveFile(_fileStreamServer);
		}

		return true;
	}

	const bool ValueBunch::saveFileClient(sc::SerialFile& _fileStreamClient) const
	{
		_fileStreamClient << unsigned int(this->typeRandomOption);
		_fileStreamClient << this->valueIntegerHigh;
		_fileStreamClient << this->valueIntegerLow;
		
		return true;
	}
	const bool ValueBunch::loadFileClient(sc::BaseStream& _fileStream)
	{
		unsigned int _typeRandomOption;
		_fileStream >> _typeRandomOption;
		this->typeRandomOption = RandomOption::RANDOM_OPTION(_typeRandomOption);

		_fileStream >> this->valueIntegerHigh;
		_fileStream >> this->valueIntegerLow;

		return true;
	}
	const bool ValueBunch::loadFileServer(sc::BaseStream& _fileStream)
	{
		_fileStream >> this->fRateChoose;

		unsigned int _typeRandomOption;
		_fileStream >> _typeRandomOption;
		this->typeRandomOption = RandomOption::RANDOM_OPTION(_typeRandomOption);

		_fileStream >> this->valueIntegerHigh;
		_fileStream >> this->valueIntegerLow;

		unsigned int _sizeValue;
		_fileStream >> _sizeValue;

		for ( unsigned int _index(0); _index < _sizeValue; ++_index )
		{
			this->vectorValue.push_back(RandomOption::Value());
			RandomOption::Value& _value(this->vectorValue[_index]);
			if ( _value.loadFile(_fileStream) == false )
				return false;
		}

		return true;
	}
	const bool ValueBunch::loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const TCHAR* const _cOptionFull, const TCHAR* const _cOptSE, const RandomOption::GENERATE _typeGenerate, const bool _bConvert)
	{
		float _fRateChoose(0.0f);
		unsigned int _typeRandomOption(0);
		int _sizeSection(0);					

		_fileText.getflag(_cOptionFull, 1, 3, this->fRateChoose, true);
		_fileText.getflag(_cOptionFull, 2, 3, this->typeRandomOption, true);
		_fileText.getflag(_cOptionFull, 3, 3, _sizeSection, true);

		if ( (_typeRandomOption >= RandomOption::RANDOM_OPTION_NSIZE) || (_sizeSection <= 0) )
		{
			const std::string _stringError(sc::string::format("invalid data format : %1%", _stringNameFile));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		const bool _bValueInteger(RandomOption::bValueInteger[this->typeRandomOption]);

		int _valueIntegerHighCompare(-2100000000);
		int _valueIntegerLowCompare(2100000000);
		float _valueFloatHighCompare(-210000000.0f);
		float _valueFloatLowCompare(210000000.0f);
		RandomOption::ValueTemporary _valueTemporary;
		for ( int _indexOptSE(0); _indexOptSE < _sizeSection; ++_indexOptSE )
		{
			TCHAR _cOptSEFull[64];
			_snprintf_s(_cOptSEFull, 64, _TRUNCATE, "%s%u", _cOptSE, _indexOptSE + 1);

			RandomOption::Value* const _pValue(ValueBunch::_createValue());
			if ( _pValue == 0 )
			{
				const std::string _stringError(sc::string::format("can't create value : %1%", _stringNameFile));
				::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				return false;
			}

			if ( _pValue->loadFile_old(_fileText, _cOptSEFull, _valueTemporary, _bValueInteger) == false )
				return false;			
		}

		if ( _bValueInteger == true )
		{
			this->valueIntegerHigh = _valueTemporary.valueIntegerHighCompare;
			this->valueIntegerLow = _valueTemporary.valueIntegerLowCompare;
		}
		else
		{
			this->valueFloatHigh = _valueTemporary.valueFloatHighCompare;
			this->valueFloatLow = _valueTemporary.valueFloatLowCompare;
		}

		if ( _typeGenerate == RandomOption::GENERATE_REMODEL )
		{
			if ( (_sizeSection == 2) && (_bConvert == true) )
				ValueBunch::_rebalance();
		}		

		return true;
	}
	void ValueBunch::addValue(const unsigned int _indexInsert, const RandomOption::Value& _value)
	{
		this->vectorValue.insert(this->vectorValue.begin() + _indexInsert, _value);
	}
	void ValueBunch::removeValue(const unsigned int _indexValue)
	{
		if ( _indexValue >= this->vectorValue.size() )
			return;

		this->vectorValue.erase(this->vectorValue.begin() + _indexValue);
	}
	void ValueBunch::refreshValue(void)
	{
		const RandomOption::ValueVector& _vectorValue(this->vectorValue);

		RandomOption::ValueTemporary _valueTemporary;
		const bool _bValueInteger(RandomOption::bValueInteger[this->typeRandomOption]);
		if ( _bValueInteger == true )
		{	
			for ( ValueVectorConstIterator _iteratorValue(_vectorValue.begin()); _iteratorValue != _vectorValue.end(); ++_iteratorValue )
			{
				const RandomOption::Value& _value(*_iteratorValue);
				if ( _valueTemporary.valueIntegerLowCompare > _value.getValueIntegerLow() )
					_valueTemporary.valueIntegerLowCompare = _value.getValueIntegerLow();

				if ( _valueTemporary.valueIntegerHighCompare < _value.getValueIntegerHigh() )
					_valueTemporary.valueIntegerHighCompare = _value.getValueIntegerHigh();
			}

			this->valueIntegerLow = _valueTemporary.valueIntegerLowCompare;
			this->valueIntegerHigh = _valueTemporary.valueIntegerHighCompare;
		}
		else
		{
			for ( ValueVectorConstIterator _iteratorValue(_vectorValue.begin()); _iteratorValue != _vectorValue.end(); ++_iteratorValue )
			{
				const RandomOption::Value& _value(*_iteratorValue);
				if ( _valueTemporary.valueFloatLowCompare > _value.getValueFloatLow() )
					_valueTemporary.valueFloatLowCompare = _value.getValueFloatLow();

				if ( _valueTemporary.valueFloatHighCompare < _value.getValueFloatHigh() )
					_valueTemporary.valueFloatHighCompare = _value.getValueFloatHigh();
			}

			this->valueFloatLow = _valueTemporary.valueFloatLowCompare;
			this->valueFloatHigh = _valueTemporary.valueFloatHighCompare;
		}
	}



	Entity::Entity(const RandomOption::Entity& _rhs)
		: fRateRandomOptionGenerate(_rhs.fRateRandomOptionGenerate)
		, fRateRandomOptionDestroy(_rhs.fRateRandomOptionDestroy)
		, fRateMoneySpend(_rhs.fRateMoneySpend)
		, wMinGain(_rhs.wMinGain)
		, wMaxGain(_rhs.wMaxGain)
		, vectorValueBunchDrop(_rhs.vectorValueBunchDrop)
		, vectorValueBunchRemodel(_rhs.vectorValueBunchRemodel)
	{
		for ( unsigned int _index(RandomOption::GENERATE_NSIZE); _index--; )
			this->sizeRandomOptionGenerate[_index] = _rhs.sizeRandomOptionGenerate[_index];

		::StringCchCopy(this->cName, RandomOption::LENGTH_NAME, _rhs.cName);
	}
	Entity::Entity(const RandomOption::Entity& _rhs, const char* const _cName)
		: fRateRandomOptionGenerate(_rhs.fRateRandomOptionGenerate)
		, fRateRandomOptionDestroy(_rhs.fRateRandomOptionDestroy)
		, fRateMoneySpend(_rhs.fRateMoneySpend)
		, wMinGain(_rhs.wMinGain)
		, wMaxGain(_rhs.wMaxGain)
		, vectorValueBunchDrop(_rhs.vectorValueBunchDrop)
		, vectorValueBunchRemodel(_rhs.vectorValueBunchRemodel)
	{
		for ( unsigned int _index(RandomOption::GENERATE_NSIZE); _index--; )
			this->sizeRandomOptionGenerate[_index] = _rhs.sizeRandomOptionGenerate[_index];

		::StringCchCopy(this->cName, RandomOption::LENGTH_NAME, _cName);
	}
	Entity::Entity(const char* const _cName)
		: fRateRandomOptionGenerate(0.0f)
		, fRateRandomOptionDestroy(0.0f)
		, fRateMoneySpend(0.0f)
		, wMinGain(0)
		, wMaxGain(5)
	{
		for ( unsigned int _index(RandomOption::GENERATE_NSIZE); _index--; )
			this->sizeRandomOptionGenerate[_index] = 5;

		::StringCchCopy(this->cName, RandomOption::LENGTH_NAME, _cName);
	}
	Entity::Entity(void)
		: fRateRandomOptionGenerate(0.0f)
		, fRateRandomOptionDestroy(0.0f)
		, fRateMoneySpend(0.0f)
		, wMinGain(0)
		, wMaxGain(5)
	{
		for ( unsigned int _index(RandomOption::GENERATE_NSIZE); _index--; )
			this->sizeRandomOptionGenerate[_index] = 5;

		::memset(this->cName, 0x00, sizeof(char) * RandomOption::LENGTH_NAME);
	}
	const bool Entity::getInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, RandomOption::Information& _informationReference) const
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				const RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				const RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);		

				if ( _bValueInteger == true )
				{
					_informationReference.bValueInteger = true;
					_informationReference.fRateChoose = _valueBunch.getRateChoose();
					_informationReference.valueIntegerHigh = _valueBunch.getValueIntegerHigh();
					_informationReference.valueIntegerLow = _valueBunch.getValueIntegerLow();
				}
				else
				{
					_informationReference.bValueInteger = false;
					_informationReference.fRateChoose = _valueBunch.getRateChoose();
					_informationReference.valueFloatHigh = _valueBunch.getValueFloatHigh();
					_informationReference.valueFloatLow = _valueBunch.getValueFloatLow();
				}
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				const RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				const RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);		

				if ( _bValueInteger == true )
				{
					_informationReference.bValueInteger = true;
					_informationReference.fRateChoose = _valueBunch.getRateChoose();
					_informationReference.valueIntegerHigh = _valueBunch.getValueIntegerHigh();
					_informationReference.valueIntegerLow = _valueBunch.getValueIntegerLow();
				}
				else
				{
					_informationReference.bValueInteger = false;
					_informationReference.fRateChoose = _valueBunch.getRateChoose();
					_informationReference.valueFloatHigh = _valueBunch.getValueFloatHigh();
					_informationReference.valueFloatLow = _valueBunch.getValueFloatLow();
				}
			}
			break;
		}	

		return true;
	}
	const bool Entity::getInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue, RandomOption::Information& _informationReference) const
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				const RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;				

				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				const RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				const RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());
				if ( _indexValue >= _vectorValue.size() )
					return false;				

				const RandomOption::Value& _value(_vectorValue[_indexValue]);

				if ( _bValueInteger == true )
				{
					_informationReference.bValueInteger = true;
					_informationReference.fRateChoose = _value.getRateChoose();
					_informationReference.valueIntegerHigh = _value.getValueIntegerHigh();
					_informationReference.valueIntegerLow = _value.getValueIntegerLow();
				}
				else
				{
					_informationReference.bValueInteger = false;
					_informationReference.fRateChoose = _value.getRateChoose();
					_informationReference.valueFloatHigh = _value.getValueFloatHigh();
					_informationReference.valueFloatLow = _value.getValueFloatLow();
				}
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				const RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;				

				const bool _bValueInteger(RandomOption::bValueInteger[_typeRandomOption]);

				const RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				const RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());
				if ( _indexValue >= _vectorValue.size() )
					return false;				

				const RandomOption::Value& _value(_vectorValue[_indexValue]);

				if ( _bValueInteger == true )
				{
					_informationReference.bValueInteger = true;
					_informationReference.fRateChoose = _value.getRateChoose();
					_informationReference.valueIntegerHigh = _value.getValueIntegerHigh();
					_informationReference.valueIntegerLow = _value.getValueIntegerLow();
				}
				else
				{
					_informationReference.bValueInteger = false;
					_informationReference.fRateChoose = _value.getRateChoose();
					_informationReference.valueFloatHigh = _value.getValueFloatHigh();
					_informationReference.valueFloatLow = _value.getValueFloatLow();
				}
			}
			break;
		}	

		return true;
	}
	const bool Entity::setInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, RandomOption::Information& _information)
	{
		// valueBunch 값 뿐만이 아닌 내부 값도 변경 해 주어야 한다;
		return false;
	}
	const bool Entity::setInformation(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue, RandomOption::Information& _information)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;				

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());
				if ( _indexValue >= _vectorValue.size() )
					return false;				

				RandomOption::Value& _value(_vectorValue[_indexValue]);

				_value.setRateChoose(_information.fRateChoose);
				if ( _information.bValueInteger == true )
				{
					_value.setValueHigh(_information.valueIntegerHigh);
					_value.setValueLow(_information.valueIntegerLow);
				}
				else
				{
					_value.setValueHigh(_information.valueFloatHigh);
					_value.setValueLow(_information.valueFloatLow);
				}				
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				RandomOption::ValueVector& _vectorValue(_valueBunch.getValueVector());
				if ( _indexValue >= _vectorValue.size() )
					return false;

				RandomOption::Value& _value(_vectorValue[_indexValue]);

				_value.setRateChoose(_information.fRateChoose);
				if ( _information.bValueInteger == true )
				{
					_value.setValueHigh(_information.valueIntegerHigh);
					_value.setValueLow(_information.valueIntegerLow);
				}
				else
				{
					_value.setValueHigh(_information.valueFloatHigh);
					_value.setValueLow(_information.valueFloatLow);
				}
			}
			break;
		}
		Entity::refreshValueBunch(_typeGenerate, _typeRandomOption);

		return true;
	}
	const bool Entity::removeValueBunch(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				_vectorValueBunch.erase(_iteratorValueBunch);
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorConstIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				_vectorValueBunch.erase(_iteratorValueBunch);
			}
			break;
		default:
			return false;
		}

		return true;
	}
	const bool Entity::removeValueBunch(const RandomOption::GENERATE _typeGenerate, const unsigned int _indexValueBunch)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				if ( _indexValueBunch >= _vectorValueBunch.size() )
					return false;

				_vectorValueBunch.erase(_vectorValueBunch.begin() + _indexValueBunch);
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				if ( _indexValueBunch >= _vectorValueBunch.size() )
					return false;

				_vectorValueBunch.erase(_vectorValueBunch.begin() + _indexValueBunch);
			}
			break;
		default:
			return false;
		}

		return true;
	}
	const bool Entity::removeValue(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption, const unsigned int _indexValue)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				_valueBunch.removeValue(_indexValue);
				_valueBunch.refreshValue();
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					return false;

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				_valueBunch.removeValue(_indexValue);
				_valueBunch.refreshValue();
			}
			break;
		default:
			return false;
		}

		return true;
	}
	const bool Entity::removeValue(const RandomOption::GENERATE _typeGenerate, const unsigned int _indexValueBunch, const unsigned int _indexValue)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				if ( _indexValueBunch >= _vectorValueBunch.size() )
					return false;			

				RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[_indexValueBunch]);
				_valueBunch.removeValue(_indexValue);
				_valueBunch.refreshValue();
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				if ( _indexValueBunch >= _vectorValueBunch.size() )
					return false;

				RandomOption::ValueBunch& _valueBunch(_vectorValueBunch[_indexValueBunch]);
				_valueBunch.removeValue(_indexValue);
				_valueBunch.refreshValue();
			}
			break;
		default:
			return false;
		}

		return true;
	}
	void Entity::refreshValueBunch(const RandomOption::GENERATE _typeGenerate, const RandomOption::RANDOM_OPTION _typeRandomOption)
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchDrop);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					break;

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				_valueBunch.refreshValue();
			}
			break;
		case RandomOption::GENERATE_REMODEL:
			{
				RandomOption::ValueBunchVector& _vectorValueBunch(this->vectorValueBunchRemodel);
				RandomOption::ValueBunchVectorIterator _iteratorValueBunch(std::find(_vectorValueBunch.begin(), _vectorValueBunch.end(), _typeRandomOption));
				if ( _iteratorValueBunch == _vectorValueBunch.end() )
					break;

				RandomOption::ValueBunch& _valueBunch(*_iteratorValueBunch);
				_valueBunch.refreshValue();
			}
			break;
		}
	}
	
	const bool Entity::saveFileServer(sc::SerialFile& _fileStreamServer) const
	{
		std::string _stringName(this->cName);
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);

		_fileStreamServer << _stringName;
		_fileStreamServer << this->fRateRandomOptionGenerate;
		_fileStreamServer << this->fRateRandomOptionDestroy;
		_fileStreamServer << this->fRateMoneySpend;
		_fileStreamServer << this->wMinGain;
		_fileStreamServer << this->wMaxGain;

		for ( unsigned int _index(0); _index < RandomOption::GENERATE_NSIZE; ++_index )
			_fileStreamServer << this->sizeRandomOptionGenerate[_index];

		const RandomOption::ValueBunchVector& _vectorValueBunchDrop(this->vectorValueBunchDrop);
		const RandomOption::ValueBunchVector& _vectorValueBunchRemodel(this->vectorValueBunchRemodel);

		const unsigned int _sizeValueBunchDrop(_vectorValueBunchDrop.size());
		_fileStreamServer << _sizeValueBunchDrop;
		for ( unsigned int _index(0); _index < _sizeValueBunchDrop; ++_index )
		{
			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunchDrop[_index]);
			if ( _valueBunch.saveFileServer(_fileStreamServer) == false )
				return false;
		}

		const unsigned int _sizeValueBunchRemodel(_vectorValueBunchRemodel.size());
		_fileStreamServer << _sizeValueBunchRemodel;
		for ( unsigned int _index(0); _index < _sizeValueBunchRemodel; ++_index )
		{
			const RandomOption::ValueBunch& _valueBunch(_vectorValueBunchRemodel[_index]);
			if ( _valueBunch.saveFileServer(_fileStreamServer) == false )
				return false;
		}
		

		return true;
	}	

	const bool Entity::loadFile_001(sc::BaseStream& _fileStream)
	{
		_fileStream >> this->fRateRandomOptionGenerate;
		_fileStream >> this->fRateRandomOptionDestroy;
		_fileStream >> this->fRateMoneySpend;
		this->wMinGain = 0;
		this->wMaxGain = 5;

		for ( unsigned int _index(0); _index < RandomOption::GENERATE_NSIZE; ++_index )
			_fileStream >> this->sizeRandomOptionGenerate[_index];

		RandomOption::ValueBunchVector& _vectorValueBunchDrop(this->vectorValueBunchDrop);

		unsigned int _sizeValueBunchDrop;
		_fileStream >> _sizeValueBunchDrop;
		for ( unsigned int _index(0); _index < _sizeValueBunchDrop; ++_index )
		{
			_vectorValueBunchDrop.push_back(RandomOption::ValueBunch());
			RandomOption::ValueBunch& _valueBunch(_vectorValueBunchDrop[_index]);
			if ( _valueBunch.loadFileServer(_fileStream) == false )
				return false;
		}

		RandomOption::ValueBunchVector& _vectorValueBunchRemodel(this->vectorValueBunchRemodel);

		unsigned int _sizeValueBunchRemodel;
		_fileStream >> _sizeValueBunchRemodel;
		for ( unsigned int _index(0); _index < _sizeValueBunchRemodel; ++_index )
		{
			_vectorValueBunchRemodel.push_back(RandomOption::ValueBunch());
			RandomOption::ValueBunch& _valueBunch(_vectorValueBunchRemodel[_index]);
			if ( _valueBunch.loadFileServer(_fileStream) == false )
				return false;
		}
		return true;
	}

	const bool Entity::loadFile_002(sc::BaseStream& _fileStream)
	{
		_fileStream >> this->fRateRandomOptionGenerate;
		_fileStream >> this->fRateRandomOptionDestroy;
		_fileStream >> this->fRateMoneySpend;
		_fileStream >> this->wMinGain;
		_fileStream >> this->wMaxGain;
		for ( unsigned int _index(0); _index < RandomOption::GENERATE_NSIZE; ++_index )
			_fileStream >> this->sizeRandomOptionGenerate[_index];

		RandomOption::ValueBunchVector& _vectorValueBunchDrop(this->vectorValueBunchDrop);

		unsigned int _sizeValueBunchDrop;
		_fileStream >> _sizeValueBunchDrop;
		for ( unsigned int _index(0); _index < _sizeValueBunchDrop; ++_index )
		{
			_vectorValueBunchDrop.push_back(RandomOption::ValueBunch());
			RandomOption::ValueBunch& _valueBunch(_vectorValueBunchDrop[_index]);
			if ( _valueBunch.loadFileServer(_fileStream) == false )
				return false;
		}

		RandomOption::ValueBunchVector& _vectorValueBunchRemodel(this->vectorValueBunchRemodel);

		unsigned int _sizeValueBunchRemodel;
		_fileStream >> _sizeValueBunchRemodel;
		for ( unsigned int _index(0); _index < _sizeValueBunchRemodel; ++_index )
		{
			_vectorValueBunchRemodel.push_back(RandomOption::ValueBunch());
			RandomOption::ValueBunch& _valueBunch(_vectorValueBunchRemodel[_index]);
			if ( _valueBunch.loadFileServer(_fileStream) == false )
				return false;
		}
		return true;
	}

	const bool Entity::loadFileServer(sc::BaseStream& _fileStream)
	{
		char szTemp[255];
		DWORD dwVersion;
		_fileStream.GetFileType(szTemp, 255, dwVersion);
		switch ( dwVersion )
		{
		case 0x0001:	return loadFile_001(_fileStream);
		case 0x0002:	return loadFile_002(_fileStream);
		}
        return true;
	}

	const bool Entity::loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const bool _bConvert)
	{
		_fileText.getflag("fRATE", 1, 1, this->fRateRandomOptionGenerate, true);
		_fileText.getflag("D_point", 1, 1, this->fRateRandomOptionDestroy, true);
		_fileText.getflag("S_value", 1, 1, this->fRateMoneySpend, true);
		_fileText.getflag("wMinGain", 1, 1, this->wMinGain, true);
		_fileText.getflag("wMaxGain", 1, 1, this->wMaxGain, true);

		TCHAR _cOptionHeader[RandomOption::GENERATE_NSIZE][24] = {"NEW_OPTION_", "REB_OPTION_"};
		TCHAR _cOptSeHeader[RandomOption::GENERATE_NSIZE][24] = {"NEW_OPT_SE_", "REB_OPT_SE_"};
		for ( unsigned int _indexType(0); _indexType < RandomOption::GENERATE_NSIZE; ++_indexType )
		{
			for ( unsigned int _indexOption(0); _indexOption < 256; ++_indexOption )
			{
				TCHAR _cOptionFull[64];
				TCHAR _cOptSE[64];
				_snprintf_s(_cOptionFull, 64, _TRUNCATE, "%s%u", _cOptionHeader[_indexType], _indexOption + 1);
				_snprintf_s(_cOptSE, 64, _TRUNCATE, "%s%u_", _cOptSeHeader[_indexType], _indexOption + 1);

				if ( _fileText.findflag(_cOptionFull) == 0 )
					break;

				RandomOption::ValueBunch* const _pValueBunch(Entity::_createValueBunch(RandomOption::GENERATE(_indexType)));
				if ( _pValueBunch == 0 )
				{
					const std::string _stringError(sc::string::format("can't create value-bunch : %1%", _stringNameFile));
					::MessageBox(0, _stringError.c_str(), "error", MB_OK);
					return false;
				}

				if ( _pValueBunch->loadFile_old(_fileText, _stringNameFile, _cOptionFull, _cOptSE, RandomOption::GENERATE(_indexType), _bConvert) == false )
					return false;
			}
		}

		return true;
	}




	
	Manager::~Manager(void)
	{
	}
	RandomOption::Manager& Manager::getInstance(void)
	{
		static RandomOption::Manager _instance;
		return _instance;
	}

	Manager::Manager(void)
	{
	}

	void Manager::remove(const char* const _cName)
	{
		if ( _cName == 0 )
			return;

		std::string _stringName(_cName);
		const unsigned int _lengthName(_stringName.length());
		if ( _lengthName == 0 )
			return;
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);		

		RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);

		const unsigned int _hashKey(HASH::generateHashKey(_stringName.c_str(), _lengthName));
		RandomOption::EntityListMapIterator _iteratorEntityList(_mapEntityList.find(_hashKey));
		if ( _iteratorEntityList == _mapEntityList.end() )
			return;

		RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			const RandomOption::Entity& _entity(*_iteratorEntity);
			if ( _entity == _stringName.c_str() )
			{
				_listEntity.erase(_iteratorEntity);
				if ( _listEntity.size() == 0 )
					_mapEntityList.erase(_iteratorEntityList);
				--this->sizeEntity;
				return;
			}
		}
	}
	const RandomOption::Entity* const Manager::find(const char* const _cName) const
	{
		if ( _cName == 0 )
			return 0;

		std::string _stringName(_cName);
		const unsigned int _lengthName(_stringName.length());
		if ( _lengthName == 0 )
			return 0;
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);		

		const RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);

		const unsigned int _hashKey(HASH::generateHashKey(_stringName.c_str(), _lengthName));
		RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.find(_hashKey));
		if ( _iteratorEntityList == _mapEntityList.end() )
			return 0;

		const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			const RandomOption::Entity& _entity(*_iteratorEntity);
			if ( _entity == _stringName.c_str() )
				return &_entity;
		}

		return 0;
	}
	RandomOption::Entity* const Manager::find(const char* const _cName)
	{
		if ( _cName == 0 )
			return 0;

		std::string _stringName(_cName);
		const unsigned int _lengthName(_stringName.length());
		if ( _lengthName == 0 )
			return 0;
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);		

		RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);

		
		const unsigned int _hashKey(HASH::generateHashKey(_stringName.c_str(), _lengthName));
		RandomOption::EntityListMapIterator _iteratorEntityList(_mapEntityList.find(_hashKey));
		if ( _iteratorEntityList == _mapEntityList.end() )
			return 0;

		RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			RandomOption::Entity& _entity(*_iteratorEntity);
			if ( _entity == _stringName.c_str() )
				return &_entity;
		}

		return 0;
	}
	RandomOption::Entity* const Manager::create(const char* const _cName)
	{
		if ( _cName == 0 )
			return 0;

		std::string _stringName(_cName);
		const unsigned int _lengthName(_stringName.length());		
		if ( _lengthName == 0 )
			return 0;
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);		

		RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);		
		
		const unsigned int _hashKey(HASH::generateHashKey(_stringName.c_str(), _lengthName));
		RandomOption::EntityListMapIterator _iteratorEntityList(_mapEntityList.find(_hashKey));
		if ( _iteratorEntityList == _mapEntityList.end() )
		{
			RandomOption::EntityList _listEntity;
			_mapEntityList.insert(RandomOption::EntityListValue(_hashKey, _listEntity));
			_iteratorEntityList = _mapEntityList.find(_hashKey);
		}

		RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			RandomOption::Entity& _entity(*_iteratorEntity);
			if ( _entity == _stringName.c_str() )
				return &_entity;
		}

		RandomOption::Entity _entityNew(_stringName.c_str());
		_listEntity.push_front(_entityNew);

		RandomOption::EntityListIterator _iteratorEntity(_listEntity.begin());
		RandomOption::Entity& _entity(*_iteratorEntity);

		++this->sizeEntity;

		return &_entity;
	}
	RandomOption::Entity* const Manager::insert(const RandomOption::Entity& _entity)
	{
		std::string _stringName(_entity.getName());
		const unsigned int _lengthName(_stringName.length());
		if ( _lengthName == 0 )
			return 0;
		std::transform(_stringName.begin(), _stringName.end(), _stringName.begin(), tolower);

		RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);

		const unsigned int _hashKey(HASH::generateHashKey(_stringName.c_str(), _lengthName));
		RandomOption::EntityListMapIterator _iteratorEntityList(_mapEntityList.find(_hashKey));
		if ( _iteratorEntityList == _mapEntityList.end() )
		{
			RandomOption::EntityList _listEntity;
			_mapEntityList.insert(RandomOption::EntityListValue(_hashKey, _listEntity));
			_iteratorEntityList = _mapEntityList.find(_hashKey);
		}

		RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
		for ( RandomOption::EntityListIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
		{
			RandomOption::Entity& _entity(*_iteratorEntity);
			if ( _entity == _stringName.c_str() )
				return &_entity;
		}

		_listEntity.push_front(_entity);

		RandomOption::EntityListIterator _iteratorEntity(_listEntity.begin());
		RandomOption::Entity& _entityNew(*_iteratorEntity);

		++this->sizeEntity;

		return &_entityNew;
	}
	RandomOption::Entity* const Manager::rename(const char* const _cNameTarget, const char* const _cNameSource)
	{
		if ( (_cNameTarget == 0) || (_cNameSource == 0) )
			return 0;

		std::string _stringNameTarget(_cNameTarget);
		std::string _stringNameSource(_cNameSource);
		const unsigned int _lengthNameTarget(_stringNameTarget.length());
		const unsigned int _lengthNameSource(_stringNameSource.length());
		if ( (_lengthNameTarget == 0) || (_lengthNameSource == 0) )
			return 0;
		std::transform(_stringNameTarget.begin(), _stringNameTarget.end(), _stringNameTarget.begin(), tolower);
		std::transform(_stringNameSource.begin(), _stringNameSource.end(), _stringNameSource.begin(), tolower);

		const RandomOption::Entity* const _pEntityTarget(Manager::find(_cNameTarget));
		if ( _pEntityTarget == 0 )
			return 0;

		const RandomOption::Entity _entitySource(*_pEntityTarget, _cNameSource);
		Manager::remove(_cNameTarget);

		RandomOption::Entity* const _pEntitySource(Manager::insert(_entitySource));
		return _pEntitySource;
	}

	void Manager::calcSize(void)
	{
		INT nResize(0);
		RandomOption::EntityListMapIterator itr = mapEntityList.begin()
			, itrEnd = mapEntityList.end();
		for ( ; itr != itrEnd; ++itr )
		{
			RandomOption::EntityList& lstEntities = (*itr).second;
			nResize += lstEntities.size();
		}
		sizeEntity = nResize;
	}

	const bool Manager::saveFile(void)
	{
		calcSize();
		const unsigned int _sizeEntity(this->sizeEntity);
		if ( _sizeEntity == 0 )
			return false;

		char _cApplicationPath[256];
		::GetModuleFileName(0, _cApplicationPath, 256);	
		::PathRemoveFileSpec(_cApplicationPath);

		GLogicFile _logicFile;
		const std::string& _strFileName(_logicFile.GetRandomOptionFileName());
		const std::string _strPath(std::string(_cApplicationPath) + std::string("\\data\\glogic\\"));

		sc::SerialFile _fileStream;		
		_fileStream.SetFileType(RandomOption::Manager::_FILEHEADSERVER, RandomOption::Manager::VERSION);
		if ( _fileStream.OpenFile(FOT_WRITE, (_strPath + _strFileName).c_str()) == FALSE )
			return false;

		_fileStream.SetEncode(true);
		_fileStream << _sizeEntity;		
		
		const RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);
		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
		{
			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
			{
				const RandomOption::Entity& _entity(*_iteratorEntity);
				const std::string _stringName(_entity.getName());
				if ( _entity.saveFileServer(_fileStream) == false )
				{
					const std::string _stringError(sc::string::format("failed to save for server : %1%", _stringName));
					::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				}
			}
		}

// 		GLogicFile _logicFile;
// 		const std::string& _stringNameFileServer(_logicFile.GetRandomOptionServerFileName());
// 		const std::string& _stringNameFileClient(_logicFile.GetRandomOptionClientFileName());
// 		const std::string _stringPathServer(std::string(_cApplicationPath) + std::string("\\data\\glogicserver\\"));
// 		const std::string _stringPathClient(std::string(_cApplicationPath) + std::string("\\data\\glogic\\"));
// 
// 
// 		sc::SerialFile _fileStreamServer;		
// 		_fileStreamServer.SetFileType(RandomOption::Manager::_FILEHEADSERVER, RandomOption::Manager::VERSION);
// 		if ( _fileStreamServer.OpenFile(FOT_WRITE, (_stringPathServer + _stringNameFileServer).c_str()) == FALSE )
// 			return false;
// 
// 		_fileStreamServer.SetEncode(true);
// 		_fileStreamServer << _sizeEntity;		
// 
// 		const RandomOption::EntityListMap& _mapEntityList(this->mapEntityList);
// 		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
// 		{
// 			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
// 			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
// 			{
// 				const RandomOption::Entity& _entity(*_iteratorEntity);
// 				const std::string _stringName(_entity.getName());
// 				if ( _entity.saveFileServer(_fileStreamServer) == false )
// 				{
// 					const std::string _stringError(sc::string::format("failed to save for server : %1%", _stringName));
// 					::MessageBox(0, _stringError.c_str(), "error", MB_OK);
// 				}
// 			}
// 		}

// 		sc::SerialFile _fileStreamClient;
// 		_fileStreamClient.SetFileType(RandomOption::Manager::_FILEHEADCLIENT, RandomOption::Manager::VERSION);
// 		if ( _fileStreamClient.OpenFile(FOT_WRITE, (_stringPathClient + _stringNameFileClient).c_str()) == FALSE )
// 			return false;
// 
// 		_fileStreamClient.SetEncode(true);
// 		_fileStreamClient << _sizeEntity;
// 
// 		for ( RandomOption::EntityListMapConstIterator _iteratorEntityList(_mapEntityList.begin()); _iteratorEntityList != _mapEntityList.end(); ++_iteratorEntityList )
// 		{
// 			const RandomOption::EntityList& _listEntity(_iteratorEntityList->second);
// 			for ( RandomOption::EntityListConstIterator _iteratorEntity(_listEntity.begin()); _iteratorEntity != _listEntity.end(); ++_iteratorEntity )
// 			{
// 				const RandomOption::Entity& _entity(*_iteratorEntity);
// 				const std::string _stringName(_entity.getName());
// 				if ( _entity.saveFileClient(_fileStreamClient) == false )
// 				{
// 					const std::string _stringError(sc::string::format("failed to save for client : %1%", _stringName));
// 					::MessageBox(0, _stringError.c_str(), "error", MB_OK);
// 				}
// 			}
// 		}

		return true;
	}

	const bool Manager::_loadFileServer(sc::BaseStream& _fileStream, const std::string& _stringNameFileRandomOption, const std::string& _stringPathFileRandomOption)
	{
		DWORD _version;
		char _cTypeFile[_MAX_FNAME];
		_fileStream.GetFileType(_cTypeFile, _MAX_FNAME, _version);
		const std::string _stringTypeFile(_cTypeFile);
		if ( _stringTypeFile != std::string(_FILEHEADSERVER) )
		{
			const std::string _stringError(sc::string::format("RandomOption::Manager::loadFileTool _ %1% is not file-server", _stringPathFileRandomOption));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		_fileStream.SetEncode(true);

		_fileStream >> this->sizeEntity;
		for ( unsigned int _count(this->sizeEntity); _count--; )
		{
			std::string _stringName;
			_fileStream >> _stringName;
			RandomOption::Entity* const _pEntity(Manager::create(_stringName.c_str()));
			if ( _pEntity == 0 )
			{
				const std::string _stringError(sc::string::format("RandomOption::Manager::loadFileTool _ can't create entity : %1%", _stringPathFileRandomOption));
				::MessageBox(0, _stringError.c_str(), "error", MB_OK);
				return false;
			}

			_pEntity->loadFileServer(_fileStream);
		}

		return true;
	}
	const bool Manager::loadFileServer(GLogicFile* const _pFileLogic, const bool _bTool)
	{
		const std::string _stringPathLogic(GLOGIC::GetServerPath());
		const std::string _stringNameFileRandomOption(_pFileLogic->GetRandomOptionServerFileName());
		const std::string _stringPathFull(_stringPathLogic + _stringNameFileRandomOption);


		std::auto_ptr<sc::BaseStream> _pStreamBase(
			GLOGIC::openfile_basestream(
			_bTool == true ? false : true,
			GLOGIC::strGLOGIC_SERVER_ZIPFILE,
			_stringPathFull.c_str(),
			_stringNameFileRandomOption,
			false,
			GLOGIC::UseLogicPackFile()));

		if ( _pStreamBase.get() == 0 )
		{
			const std::string _stringError(sc::string::format("RandomOption::Manager::loadFileTool _ can't find file : %1%", _stringPathFull));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		sc::BaseStream& _fileStream(*_pStreamBase);
		return Manager::_loadFileServer(_fileStream, _stringNameFileRandomOption, _stringPathFull);
	}

	const bool Manager::loadFile(GLogicFile* const _pFileLogic, const bool _bTool)
	{
		const std::string _stringPathLogic(GLOGIC::GetPath());
		const std::string _stringNameFileRandomOption(_pFileLogic->GetRandomOptionFileName());
		const std::string _stringPathFull(_stringPathLogic + _stringNameFileRandomOption);

		std::auto_ptr<sc::BaseStream> _pStreamBase(
			GLOGIC::openfile_basestream(
			_bTool == true ? false : true,
			GLogicData::GetInstance().GetGlogicZipFile(),
			_stringPathFull.c_str(),
			_stringNameFileRandomOption,
			false,
			GLOGIC::UseLogicPackFile()));

		if ( _pStreamBase.get() == 0 )
		{
			const std::string _stringError(sc::string::format("RandomOption::Manager::loadFile _ can't find file : %1%", _stringPathFull));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		sc::BaseStream& _fileStream(*_pStreamBase);
		return Manager::_loadFileServer(_fileStream, _stringNameFileRandomOption, _stringPathFull);
	}

	const bool Manager::loadFileTool(void)
	{
		char _cApplicationPath[256];
		::GetModuleFileName(0, _cApplicationPath, 256);	
		::PathRemoveFileSpec(_cApplicationPath);

		GLogicFile _logicFile;
		const std::string _stringPath(sc::string::format("%1%\\data\\glogic\\", _cApplicationPath));
		const std::string _stringPathFull(_stringPath + "glogic.rcc");		
		const std::string& _stringNameFileRandomOption(_logicFile.GetRandomOptionFileName());
		const std::string _stringPathFileRandomOption(_stringPath + _stringNameFileRandomOption);

		const std::string _stringPathServer(sc::string::format("%1%\\data\\glogicserver\\", _cApplicationPath));
		const std::string _stringPathServerFull(_stringPathServer + "glogicserver.rcc");		

		ZipMan& _managerZip(ZipMan::GetInstance());
		RandomOption::Manager& _managerRandomOption(RandomOption::Manager::getInstance());
		if ( _managerRandomOption.size() == 0 )
			_managerZip.LOADFILE_RCC(_stringPathServerFull);

		////////////////////////////////////////////////////////

		_managerRandomOption.clear();

		std::auto_ptr<sc::BaseStream> _pStreamBase(
			GLOGIC::openfile_basestream(
			FALSE,
			GLogicData::GetInstance().GetGlogicZipFile(),
			_stringPathFileRandomOption.c_str(),
			_stringNameFileRandomOption,
			false,
			FALSE));

		if ( _pStreamBase.get() == 0 )
		{
			const std::string _stringError(sc::string::format("RandomOption::Manager::loadFileTool _ can't find file : %1%", _stringPathFileRandomOption));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		sc::BaseStream& _fileStream(*_pStreamBase);
		return Manager::_loadFileServer(_fileStream, _stringNameFileRandomOption, _stringPathFull);
	}
	const bool Manager::loadFile_old(gltexfile& _fileText, const std::string& _stringNameFile, const bool _bConvert)
	{
		if ( _fileText.findflag("fRATE") == 0 )
			return true; // it's not file-random-option;

		RandomOption::Entity* const _pEntity(Manager::create(_stringNameFile.c_str()));
		if ( _pEntity == 0 )
		{
			const std::string _stringError(sc::string::format("can't create entity : %1%", _stringNameFile));
			::MessageBox(0, _stringError.c_str(), "error", MB_OK);
			return false;
		}

		if ( _pEntity->loadFile_old(_fileText, _stringNameFile, _bConvert) == false )
			return false;		

		return true;
	}	
}
