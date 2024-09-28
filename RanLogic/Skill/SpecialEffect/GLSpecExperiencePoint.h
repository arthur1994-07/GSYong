#pragma once

#include "../../../SigmaCore/Math/Random.h"

namespace SKILL
{
	namespace ExperiencePoint
	{
		extern HRESULT LoadFile(const std::string& fileName, const bool bPastLoad);
	}

	class ExperiencePointSettings
	{
	public:
		class Table;
	public:
		inline void setValid(void);
		inline const bool isValid(void) const;		
		
		inline const unsigned int getNItem(const unsigned int nTableIndex) const;
		inline const ExperiencePointSettings::Table& getTable(const unsigned int nTableIndex) const;
		inline const float getRate(const unsigned int nTableIndex) const;

		const bool push(const unsigned int nTableIndex);
		const bool push(const unsigned int nTableIndex, const float fSelectRate, const float fMinRate, const float fMaxRate);

	public:
		static ExperiencePointSettings* const getInstance(void);
		~ExperiencePointSettings(void);

	private:
		ExperiencePointSettings(void);

	public:
		friend HRESULT SKILL::ExperiencePoint::LoadFile(const std::string& fileName, const bool bPastLoad);
		class Table
		{
		public:			
			inline const float getRate(void) const;
			inline const float getSelectRate(const unsigned int nItemIndex) const;
			inline const float getMinRate(const unsigned int nItemIndex) const;
			inline const float getMaxRate(const unsigned int nItemIndex) const;
			inline const unsigned int getNItem(void) const;
			void push(const float fSelectRate, const float fMinRate, const float fMaxRate);

		public:
			Table(void);
			~Table(void);

		private:
			class Item
			{
			public:
				inline const float getSelectRate(void) const;
				inline const float getMinRate(void) const;
				inline const float getMaxRate(void) const;
				inline const float getRate(void) const;

			public:
				inline const Item& operator =(const Item& rhs);
				Item(const float fSelectRate, const float fMinRate, const float fMaxRate);
				~Item(void);
				
			public:
				const float m_fSelectRate;
				const float m_fMinRate;
				const float m_fMaxRate;
			};
			typedef std::vector<Item> ItemVector;
			typedef ItemVector::iterator ItemVectorIter;
			typedef ItemVector::const_iterator ItemVectorCIter;

			ItemVector m_ItemVector;
		};

	private:
		typedef std::vector<Table> TableVector;
		typedef TableVector::iterator TableVectorIter;
		typedef TableVector::const_iterator TableVectorCIter;

		TableVector m_TableVector;
		bool m_bValid;
	};

	inline void ExperiencePointSettings::setValid(void)
	{
		m_bValid = true;
	}
	inline const bool ExperiencePointSettings::isValid(void) const
	{
		return m_bValid;
	}



	inline const unsigned int ExperiencePointSettings::getNItem(const unsigned int nTableIndex) const
	{
		if ( nTableIndex >= m_TableVector.size() )
			return 0;
		return m_TableVector[nTableIndex].getNItem();
	}
	inline const ExperiencePointSettings::Table& ExperiencePointSettings::getTable(const unsigned int nTableIndex) const
	{
		return m_TableVector[nTableIndex];
	}
	inline const float ExperiencePointSettings::getRate(const unsigned int nTableIndex) const
	{
		if ( nTableIndex >= m_TableVector.size() )
			return 0.0f;		

		return m_TableVector[nTableIndex].getRate();
	}
	

	inline const unsigned int ExperiencePointSettings::Table::getNItem(void) const
	{
		return m_ItemVector.size();
	}

	inline const float ExperiencePointSettings::Table::getSelectRate(const unsigned int nItemIndex) const
	{
		return m_ItemVector[nItemIndex].getSelectRate();
	}
	inline const float ExperiencePointSettings::Table::getMinRate(const unsigned int nItemIndex) const
	{
		return m_ItemVector[nItemIndex].getMinRate();
	}
	inline const float ExperiencePointSettings::Table::getMaxRate(const unsigned int nItemIndex) const
	{
		return m_ItemVector[nItemIndex].getMaxRate();
	}

	inline const float ExperiencePointSettings::Table::getRate(void) const
	{
		const float fRate(sc::Random::getInstance().GetFloatPercent());
		float fAccumRate(0.0f);
		for ( ItemVectorCIter _iter(m_ItemVector.begin()); _iter != m_ItemVector.end(); ++_iter )
		{
			fAccumRate += _iter->getSelectRate();
			if ( fRate < fAccumRate )
				return _iter->getRate();
		}

		return 0.0f;
	}

	inline const ExperiencePointSettings::Table::Item& ExperiencePointSettings::Table::Item::operator =(const ExperiencePointSettings::Table::Item& rhs)
	{
		::memcpy(this, &rhs, sizeof(Item));
		return *this;
	}
	inline const float ExperiencePointSettings::Table::Item::getRate(void) const
	{
		return sc::Random::getInstance().RandomNumber(m_fMinRate, m_fMaxRate);
	}
	inline const float ExperiencePointSettings::Table::Item::getMinRate(void) const
	{
		return m_fMinRate;
	}
	inline const float ExperiencePointSettings::Table::Item::getMaxRate(void) const
	{
		return m_fMaxRate;
	}
	inline const float ExperiencePointSettings::Table::Item::getSelectRate(void) const
	{
		return m_fSelectRate;
	}
}