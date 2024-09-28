#pragma once

namespace RandomOption
{
	__forceinline const RandomOption::Value& Value::operator =(const RandomOption::Value& _rhs)
	{
		this->fRateChoose = _rhs.fRateChoose;
		this->valueIntegerHigh = _rhs.valueIntegerHigh;
		this->valueIntegerLow = _rhs.valueIntegerLow;

		return *this;
	}
	__forceinline const float Value::getRateChoose(void) const
	{
		return this->fRateChoose;
	}
	__forceinline const float Value::getValue(const bool _bValueInteger) const
	{
		sc::Random& _random(sc::Random::getInstance());

		const float _fRate(_random.GetFloatRate());

		if ( _bValueInteger == true )
		{
			const float _fValueLow(float(this->valueIntegerLow));
			const float _fValueGap(float(this->valueIntegerHigh) - _fValueLow);

			return _fValueLow + _fValueGap * _fRate;
		}
		else
		{
			const float _fValueLow(this->valueFloatLow);
			const float _fValueGap(this->valueFloatHigh - _fValueLow);

			return _fValueLow + _fValueGap * _fRate;
		}

		return 0.0f;
	}
	__forceinline const float Value::getValueFloatHigh(void) const
	{
		return this->valueFloatHigh;
	}
	__forceinline const float Value::getValueFloatLow(void) const
	{
		return this->valueFloatLow;
	}
	__forceinline const int Value::getValueIntegerHigh(void) const
	{
		return this->valueIntegerHigh;
	}
	__forceinline const int Value::getValueIntegerLow(void) const
	{
		return this->valueIntegerLow;
	}
	__forceinline void Value::setRateChoose(const float _fRateChoose)
	{
		this->fRateChoose = _fRateChoose;
	}
	__forceinline void Value::setValueHigh(const int _valueIntegerHigh)
	{
		this->valueIntegerHigh = _valueIntegerHigh;
	}
	__forceinline void Value::setValueHigh(const float _valueFloatHigh)
	{
		this->valueFloatHigh = _valueFloatHigh;
	}
	__forceinline void Value::setValueLow(const int _valueIntegerLow)
	{
		this->valueIntegerLow = _valueIntegerLow;
	}
	__forceinline void Value::setValueLow(const float _valueFloatLow)
	{
		this->valueFloatLow = _valueFloatLow;
	}





	__forceinline const bool ValueBunch::operator ==(const RandomOption::RANDOM_OPTION _typeRandomOption) const
	{
		return this->typeRandomOption == _typeRandomOption;
	}
	__forceinline const RandomOption::ValueBunch& ValueBunch::operator =(const RandomOption::ValueBunch& _rhs)
	{
		this->fRateChoose = _rhs.fRateChoose;
		this->typeRandomOption = _rhs.typeRandomOption;
		this->valueIntegerHigh = _rhs.valueIntegerHigh;
		this->valueIntegerLow = _rhs.valueIntegerLow;		
		this->vectorValue = _rhs.vectorValue;

		return *this;
	}
	__forceinline const float ValueBunch::getRateChoose(void) const
	{
		return this->fRateChoose;
	}
	__forceinline const int ValueBunch::getValueIntegerHigh(void) const
	{
		return this->valueIntegerHigh;
	}
	__forceinline const int ValueBunch::getValueIntegerLow(void) const
	{
		return this->valueIntegerLow;
	}
	__forceinline const float ValueBunch::getValueFloatHigh(void) const
	{
		return this->valueFloatHigh;
	}
	__forceinline const float ValueBunch::getValueFloatLow(void) const
	{
		return this->valueFloatLow;
	}
	__forceinline const RandomOption::RANDOM_OPTION ValueBunch::getTypeRandomOption(void) const
	{
		return this->typeRandomOption;
	}
	__forceinline const RandomOption::ValueVector& ValueBunch::getValueVector(void) const
	{
		return this->vectorValue;
	}
	__forceinline RandomOption::ValueVector& ValueBunch::getValueVector(void)
	{
		return this->vectorValue;
	}
	__forceinline const RandomOption::Value& ValueBunch::getValue(void) const
	{
		sc::Random& _random(sc::Random::getInstance());
		const float _fPercent(_random.GetFloatRate());

		const unsigned int _sizeValue(this->vectorValue.size());

		float _fRange(0.0f);
		for ( unsigned int _index(0); _index < _sizeValue; ++_index )
		{
			const RandomOption::Value& _value(this->vectorValue[_index]);
			_fRange += _value.getRateChoose();
			if ( _fPercent <= _fRange )
				return _value;
		}

		return this->vectorValue[0];
	}
	__forceinline RandomOption::Value* const ValueBunch::_createValue(void)
	{
		RandomOption::ValueVector& _vectorValue(this->vectorValue);

		const unsigned int _indexValue(_vectorValue.size());
		_vectorValue.push_back(RandomOption::Value());
		return &_vectorValue[_indexValue];
	}





	__forceinline const bool Entity::operator ==(const char* const _cName) const
	{
		if ( ::strcmp(this->cName, _cName) == 0 )
			return true;

		return false;
	}
	__forceinline const RandomOption::Entity& Entity::operator =(const RandomOption::Entity& _rhs)
	{
		::StringCchCopy(this->cName, RandomOption::LENGTH_NAME, _rhs.cName);
		this->fRateRandomOptionGenerate = _rhs.fRateRandomOptionGenerate;
		this->fRateRandomOptionDestroy = _rhs.fRateRandomOptionDestroy;
		this->fRateMoneySpend = _rhs.fRateMoneySpend;
		for ( unsigned int _index(RandomOption::GENERATE_NSIZE); _index--; )
			this->sizeRandomOptionGenerate[_index] = _rhs.sizeRandomOptionGenerate[_index];

		this->vectorValueBunchDrop = _rhs.vectorValueBunchDrop;
		this->vectorValueBunchRemodel = _rhs.vectorValueBunchRemodel;
		this->wMinGain = _rhs.wMinGain;
		this->wMaxGain = _rhs.wMaxGain;

		return *this;
	}
	__forceinline const char* const Entity::getName(void) const
	{
		return this->cName;
	}
	__forceinline const float Entity::getRateRandomOptionGenerate(void) const
	{
		return this->fRateRandomOptionGenerate;
	}
	__forceinline const float Entity::getRateRandomOptionDestroy(void) const
	{
		return this->fRateRandomOptionDestroy;
	}
	__forceinline const float Entity::getRateMoneySpend(void) const
	{
		return this->fRateMoneySpend;
	}
	__forceinline const unsigned int Entity::getSizeValueBunch(const RandomOption::GENERATE _typeGenerate) const
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			return this->vectorValueBunchDrop.size();
		case RandomOption::GENERATE_REMODEL:
			return this->vectorValueBunchRemodel.size();
		}

		return 0;
	}
	__forceinline const unsigned int Entity::getSizeRandomOptionGenerate(const RandomOption::GENERATE _typeGenerate) const
	{
		if ( _typeGenerate >= RandomOption::GENERATE_NSIZE )
			return 0;

		return min(this->sizeRandomOptionGenerate[_typeGenerate], (this->wMaxGain == 0? 5:this->wMaxGain));
	}	
	__forceinline const RandomOption::ValueBunchVector* const Entity::getVectorValueBunch(const RandomOption::GENERATE _typeGenerate) const
	{
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			return &this->vectorValueBunchDrop;
		case RandomOption::GENERATE_REMODEL:
			return &this->vectorValueBunchRemodel;
		}

		return 0;
	}
	__forceinline RandomOption::ValueBunch* const Entity::_createValueBunch(const RandomOption::GENERATE _typeGenerate)
	{
		RandomOption::ValueBunchVector* _pVectorValueBunch(0);
		switch ( _typeGenerate )
		{
		case RandomOption::GENERATE_DROP:
			_pVectorValueBunch = &this->vectorValueBunchDrop;
			break;
		case RandomOption::GENERATE_REMODEL:
			_pVectorValueBunch = &this->vectorValueBunchRemodel;
			break;
		default:
			return 0;
		}

		RandomOption::ValueBunchVector& _vectorValueBunch(*_pVectorValueBunch);
		const unsigned int _indexValueBunch(_pVectorValueBunch->size());
		_vectorValueBunch.push_back(RandomOption::ValueBunch());
		return &_vectorValueBunch[_indexValueBunch];
	}
	





	__forceinline const unsigned int Manager::size(void) const
	{
		return this->sizeEntity;
	}
	__forceinline const RandomOption::EntityListMap& Manager::getMapEntityList(void) const
	{
		return this->mapEntityList;
	}
	__forceinline RandomOption::EntityListMap& Manager::getMapEntityList(void)
	{
		return this->mapEntityList;
	}
	__forceinline void Manager::clear(void)
	{
		this->sizeEntity = 0;
		this->mapEntityList.clear();
	}
}