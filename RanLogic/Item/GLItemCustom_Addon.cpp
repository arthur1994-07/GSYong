#include "../pch.h"
#include "../RandomAddon/RandomAddon.h"
#include "../RandomAddon/RandomAddonDef.h"
 
#include "../../SigmaCore/Math/Random.h"

#include "./GLItemDef.h"
#include "./GLItem.h"
#include "./GLItem_RandomGenOp_Utility.h"
#include "./GLItemCustom_Addon.h"

SITEMCUSTOM_ADDON::SITEMCUSTOM_ADDON(void) 
: m_sADDON_0(EMITEM_ADDON::EMADD_NOTINIT)	// 비 초기화 상태임을 의미;
, m_sADDON_1(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_2(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_3(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_4(EMITEM_ADDON::EMADD_NOTINIT)
, m_sADDON_5(EMITEM_ADDON::EMADD_NOTINIT)
{
}

SITEMCUSTOM_ADDON::~SITEMCUSTOM_ADDON(void) 
{
}

bool SITEMCUSTOM_ADDON::IsInit(void) const
{
	if ( m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NOTINIT
		&& m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NOTINIT )
		return false;

	return true;
}

bool SITEMCUSTOM_ADDON::IsEmpty(void) const
{
	if (   ( (m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[0].emTYPE == EMITEM_ADDON::EMADD_NONE) )  
		&& ( (m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[1].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[2].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[3].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[4].emTYPE == EMITEM_ADDON::EMADD_NONE) )
		&& ( (m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NOTINIT ) ||  (m_sADDON[5].emTYPE == EMITEM_ADDON::EMADD_NONE) ) )
		return true;
	
	return false;
}

void SITEMCUSTOM_ADDON::ResetAddon(void)
{
	for ( INT i(0); (i < SITEMCUSTOM_ADDON::ADDON_SIZE); ++i )
		m_sADDON[i] = ITEM::SADDON();
}

DWORD SITEMCUSTOM_ADDON::GetOptType(DWORD dwIndex) const
{
	return m_sADDON[dwIndex].emTYPE;
}

const UINT SITEMCUSTOM_ADDON::GetOptIndex ( const BYTE _nOptionType ) const
{
	for ( UINT i = 0; i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
	{
		if ( _nOptionType == m_sADDON[i].emTYPE )
			return i;
	}

	return SITEMCUSTOM_ADDON::ADDON_SIZE;
}

void SITEMCUSTOM_ADDON::DefaultInitialize( const SNATIVEID& sNID )
{
	DefaultInitialize(GLItemMan::GetInstance().GetItem(sNID));
}

void SITEMCUSTOM_ADDON::DefaultInitialize( const SITEM* pItem )
{
	if ( !pItem )
		return;

	const INT nMax(min(SITEMCUSTOM_ADDON::ADDON_SIZE, ITEM::SSUIT::ADDON_SIZE ));
	for ( INT i = 0; i < nMax; ++i )
	{
		m_sADDON[i] = pItem->sSuitOp.sADDON[i];
	}

	//this->Generate_RandomAddon(RandomAddon::GENERATE_GEN|RandomAddon::GENERATE_ONLYRANDOM, pItem);
}

// 랜덤옵션과 비슷한 메커니즘으로 돌아간다;
bool SITEMCUSTOM_ADDON::Generate_RandomAddon( DWORD dwGenerateOp, const SITEM* const pItem, const ITEM_RANDOMOP_UTILL::FixOption& fixOption, SITEM* pItemStamp )
{
	RandomAddon::Entity sEntity;
	if ( !pItem || E_FAIL == RandomAddon::Manager::GetInstance().Find( pItem->sSuitOp.szRandomAddonScript, sEntity ) )
		return false;

	RandomAddon::GENERATE_OPTION emGenerateOp = static_cast<RandomAddon::GENERATE_OPTION>(dwGenerateOp);

	// 옵션 발생확률 검사;
	if ( (emGenerateOp & RandomAddon::GENERATE_GEN) 
		&& !sc::Random::getInstance().IsSucceed(sEntity.GetGenerateRate()) )
		return false;

	// 발생가능 횟수 미리 파악안한다, 구할수 있을만큼 구해놓고, 밀어넣을 공간있는만큼 밀어넣는다;
	// 효율면에선 조금더 뛰어나지만 코드 가독성에있어선 그닥 별로인거같다;

	const RandomAddon::VEC_BUNCH& vecGenerateBunch = emGenerateOp & RandomAddon::GENERATE_GEN ? sEntity.vecBunchDrop : sEntity.vecBunchRemodel;
	const INT nBunchSize(vecGenerateBunch.size());
	//	설정한 옵션중 임의 순위로 부여 하기 위해서 순서를 섞음;
	std::vector<DWORD> vecOrder(nBunchSize);	
	for ( DWORD i(0); i < nBunchSize; ++i )
		vecOrder[i] = i;

	// 섞는당;
	std::random_shuffle(vecOrder.begin(), vecOrder.end());

	// 개조 시 유저가 연마를 했으면 랜덤가산옵의 연마도 초기화 시키지 않으므로, 랜덤옵 붙을 수 있는 갯수를 보정한다;
	/*
	const WORD wFixMaximum(pItemStamp == NULL ? 0 : pItemStamp->sSuitOp.wReModelNum);
	if ( emGenerateOp & RandomAddon::GENERATE_GEN == 0 )
		nGEN = (unsigned int)RESETOptVALUE(fixOption, wFixMaximum);
	*/

	// 결과값들, 함수 마지막에서 해당값을 기준으로 초기화시킨다;
	ITEM::SADDON sAddonResult[SITEMCUSTOM_ADDON::ADDON_SIZE];
	INT nGenCount(0);

	// 고정된 값은 미리 담아둔다;
	for ( INT i(0); i < SITEMCUSTOM_ADDON::ADDON_SIZE && m_sADDON[i].emTYPE != EMADD_NONE; ++i )
	{
		if ( fixOption == (BYTE)m_sADDON[i].emTYPE )
			sAddonResult[nGenCount++] = m_sADDON[i];
	}

	// 옵션 발생;
	bool bFirstAddOption = false;
	for ( INT i(0); (i < nBunchSize) && (nGenCount < SITEMCUSTOM_ADDON::ADDON_SIZE); ++i )
	{
		//	임의 순위 랜덤 설정;
		const DWORD dwOrderType(vecOrder[i]);
		const RandomAddon::Bunch& sGenerateBunch = vecGenerateBunch[dwOrderType];
		switch (sGenerateBunch.emAddon)
		{
		case EMADD_NONE:
		case EMADDEX_INC_NONE:
		case EMADDEX_INCR_NONE:
			continue;
		}

		// 고정된 옵션이라면 담을 필요 없다;
		if ( fixOption == (BYTE)sGenerateBunch.emAddon )
			continue;

		// 발생확률 체크;
		const float fNowRate(sc::Random::getInstance().GetFloatPercent());

		// 첫번째 발생하는 값은 무조건 성공시켜 하나를 보장한다;
		if ( fNowRate < sGenerateBunch.fRateChoose || ( false == bFirstAddOption ) )
		{
			const RandomAddon::Value& sGenerateValue = sGenerateBunch.GetValue();
			const float fValue(sGenerateValue.GetValue(ITEM::bAddonValueInteger[sGenerateBunch.emAddon]));
			
			// 값 설정후 저장;
			ITEM::SADDON sAddon;
			sAddon.emTYPE = sGenerateBunch.emAddon;
			if ( ITEM::bAddonValueInteger[sGenerateBunch.emAddon] == true )
				sAddon.nVALUE = (INT)fValue;
			else
				sAddon.fVALUE = fValue;

			sAddonResult[nGenCount++] = sAddon;

			bFirstAddOption = true;
		}
	}

	// 선택된 랜덤 애드온을 실제 값으로 세팅;
	{
		if ( emGenerateOp & RandomAddon::GENERATE_ONLYRANDOM )
		{
			ResetAddon();
			for ( INT i(0), nInsert(0); (i < SITEMCUSTOM_ADDON::ADDON_SIZE) && (nInsert < nGenCount); ++i )
			{
				// 랜덤이었던 곳을 다른 값으로 갱신한다;
				if ( pItem->sSuitOp.sADDON[i].emTYPE == EMADD_RANDOM )
				{
					m_sADDON[i] = sAddonResult[nInsert++];
				}
			}

			for ( INT i (0); i < SITEMCUSTOM_ADDON::ADDON_SIZE; ++i )
			{
				if ( m_sADDON[i].emTYPE == EMADD_RANDOM )
					m_sADDON[i].emTYPE = EMADD_NONE;
			}
		}
		else
		{
			ResetAddon();
			const INT nResultSize(min(SITEMCUSTOM_ADDON::ADDON_SIZE, nGenCount));
			for ( INT i(0); i < nResultSize; ++i )
			{
				// 고정된 곳이라면 갱신할 필요 없다;
				//if ( fixOption == (BYTE)m_sADDON[i].emTYPE )
				//	continue;

				m_sADDON[i] = sAddonResult[i];
			}
		}
	}
	return true;
}