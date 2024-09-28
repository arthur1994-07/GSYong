#pragma once

#include "../../SigmaCore/Util/GLSingletone.h"

namespace Tooltip
{

	class GLItemTooltipMan : public CSingletone< GLItemTooltipMan > 
	{
	protected:
		friend CSingletone;

	private:
		GLItemTooltipMan ();
		~GLItemTooltipMan ();

	public:
		void Clear ();
		void GenerateItemTooltip ();
		void NextLine ();
		void CutZero ( CString& cstr, int nCount );

	private:
		// 아이템 기본 정보;
		void MakeItemDescriptionBasicInfo ();
		void MakeItemDescriptionSchool ();				// 학원 정보;
		void MakeItemDescriptionClass ();				// 클래스 정보;
		void MakeItemDescriptionSex ();					// 성별 정보;

		// 아이템 추가 정보;
		bool MakeItemDescriptionRequirement ();			// 요구조건 정보;
		bool MakeItemDescriptionType ();				// 타입 정보;

		// 아이템 기본 능력치;
		bool MakeItemDescriptionAttack ();				// 공격력 정보;
		bool MakeItemDescriptionDefense ();				// 방어력 정보;
		bool MakeItemDescriptionEtc ();					// 그외 정보;

		// 아이템 부가 능력치;
		bool MakeItemDescriptionAddValue ();			// 가산 수치;
		bool MakeItemDescriptionResist ();				// 저항;
		bool MakeItemDescriptionBlow ();				// 상태이상;

		// 세트아이템;
		void MakeItemDescriptionSetItem ();
		bool MakeItemDescriptionSetItemName ( const SSETITEM *sSetItem );
		bool MakeItemDescriptionSetItemParts ( const SSETITEM *sSetItem );
		bool MakeItemDescriptionSetItemBonus ( const SSETITEM *sSetItem );

	public:
		virtual void SetItem ( SITEM* pItem ) { m_pItemData = pItem; }
		virtual const std::string& GetItemDescription () const { return m_strItemDescription; }
		virtual const std::string& GetItemDescriptionPiece () const { return m_strItemDescriptionPiece; }

	private:
		// 아이템 정보;
		SITEM* m_pItemData;

		// 전체 툴팁 텍스트;
		std::string m_strItemDescription;

		// MakeItemDescription 함수를 호출할 때;
		// 그 함수의 Description 정보를 임시로 저장한다;
		// 가산수치 정보를 텍스트화해서 알고싶다면;
		// MakeItemDescriptionAddValue();
		// std::string strTooltip = GetItemDescriptionPiece();
		std::string m_strItemDescriptionPiece;
	};

}