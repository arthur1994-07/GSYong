#include "pch.h"

#include "./GLProductClient.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../Enginelib/DxSound/DxSoundLib.h"

#include "../../RanLogic/Product/GLProduct.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Msg/ProductMsg.h"
#include "../../RanLogic/Msg/GLContrlPcMsg.h"
#include "../../RanLogic/GLogicDataMan.h"

#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Msg/ProductMsg.h"

GLProductClient::GLProductClient( GLGaeaClient* pGaeaClent ):
	m_iMakeCount( 0 )
{
	m_pGaeaClient = pGaeaClent;
//	m_ProductRecipeExpSet.clear();
// �����ڿ��� �ʱ�ȭ�ص� ������ ������ �׷��� ������ ��û�ϴ� �κп��� �ʱ�ȭ�ϴ°� �����غ����� ����
}

GLProductClient::~GLProductClient()
{
	
}

WORD GLProductClient::GetCountProduct( DWORD nID ) 
{
	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nID );
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();

	BYTE eState;
	// �� ������ ReqProduct �� �����ؼ� ����

	return CountProduct( pProductRecipe, pCharacter, eState );;
}

WORD GLProductClient::CountProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState )
{
    if ( pProductRecipe == NULL )
    {
        eState = Product::EStateERROR;
        return 0;
    }
    //���� ���� �˻�
    if ( !IsUsableProductType( pProductRecipe->eProductType ))  //���� ���� �о߰� �ƴҶ�
    {
        eState = Product::EStateTypeFail;
        return 0;
    }
    if ( !CheckProductLearn( pProductRecipe ) )
    {
        eState = Product::EStateNotLearn;
        return 0;
    }
    if ( !CheckProductLevel( pProductRecipe, pCharLogic->m_wLevel ) )
    {
        eState = Product::EStateLevelFail;
        return 0;
    }
    if ( !CheckProductSkill( pProductRecipe ) )
    {
        eState = Product::EStateSkillFail;
        return 0;
    }
    //���� ���� ���� �˻�
    const WORD wCntMaterial = CountProductMaterial( pProductRecipe, pCharLogic, eState );   
    if ( wCntMaterial == 0 )
    {
        return 0;
    }
    const WORD wCntMoney = CountProductMoney( pProductRecipe, pCharLogic->GetInvenMoney() );
    if ( wCntMoney == 0 )
    {
        eState = Product::EStateNotMoney;
        return 0;
    }
    const WORD wCntInven = CountProductInven( pProductRecipe, pCharLogic );
    if ( wCntInven == 0 )
    {
        eState = Product::EStateInven;
        return 0;
    }
    eState = Product::EStateSuccess;
    return min( wCntMaterial, min( wCntMoney, wCntInven ) );
}

bool GLProductClient::CheckProduct( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState, WORD nCheckCount, bool bOngoing )
{
    if ( pProductRecipe == NULL || pCharLogic == NULL )
    {
        eState = Product::EStateERROR;
        return false;
    }
    if ( nCheckCount <= 0 )
    {
        eState = Product::EStateERROR;
        return false;
    }

    const WORD wCntProduct = CountProduct( pProductRecipe, pCharLogic, eState );
    if ( wCntProduct == 0 ) //�Ѱ��� ���� ���� ��
    {
        return false;
    }
    if ( wCntProduct < nCheckCount && !bOngoing )    //���ϴ� ���� ������ ���۰��� �������� ���� ��
    {
        eState = Product::EStateOverCount;
        return false;
    }
    eState = Product::EStateEnough;
    return true;
}

bool GLProductClient::CheckProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic,  WORD wMultiProduct, BYTE& eState )
{
	if ( pProductRecipe == NULL || pCharLogic == NULL )
	{
		eState = Product::EStateERROR;
		return false;
	}

   if ( wMultiProduct <= 0 )
    {
        eState = Product::EStateERROR;
        return false;
    }

	// ��� �� Ȯ��;
	for ( int i=0; i<ITEMMIX_ITEMNUM; ++i )
	{
		if ( pProductRecipe->sMeterialItem[ i ].sNID == NATIVEID_NULL() )
			continue;

		WORD nNum = static_cast< WORD >( pProductRecipe->sMeterialItem[ i ].nNum );
		if ( nNum <= 0 )
			continue;

		// ���ÿ� ������ �����ϴ� ��� ��� ��Ḧ �˻��Ѵ�
		nNum *= wMultiProduct;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sMeterialItem[ i ].sNID );
		if ( !pItem )
			continue;

		for ( int j=0; j<nNum; ++j )
		{
			SINVENITEM* pInvenItem = pCharLogic->m_cInventory.FindItem(
				pProductRecipe->sMeterialItem[ i ].sNID, j );
			if ( !pInvenItem )
				continue;

			const SITEMCUSTOM& sItemCustom = pInvenItem->CustomItem();
			if ( sItemCustom.nidDISGUISE != NATIVEID_NULL() )
			{
				eState = Product::EStateCostumeFail;
				return false;
			}
		}
	}


	return true;
};

class CCompareResultItemID
{
public:
    bool operator() ( const SProductRecipe* lvalue, const SProductRecipe* rvalue )
    {
        
        if( lvalue->wWs < rvalue->wWs )
            return true;
        
        if( lvalue->wWs == rvalue->wWs )
            if( lvalue->dwKey < rvalue->dwKey )
                return true;

        return false;
    }
};

bool GLProductClient::FindProductList( PtrVector_ProductRecipe& vecData, const BYTE eFindType, const BYTE eFindClass )
{
    vecData.clear();
    if( eFindType == Product::ETypeTotal)
    {
        for( int i = Product::ETypeHeadGear; i < Product::ETypeTotal; i++ )
        {
            if( IsUsableProductType( i ) )
            {
                GLProductRecipeMan::GetInstance().Find( vecData, i, eFindClass );
            }
        }
    }
    else
    {
        GLProductRecipeMan::GetInstance().Find( vecData, eFindType, eFindClass );
    }
    if( !vecData.empty() )
        std::sort( vecData.begin(), vecData.end(), CCompareResultItemID() );
    return true;
}

void GLProductClient::ProductOutputText( BYTE eState, const SProductRecipe* sRecipe )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sRecipe->sResultItem.sNID );
    if( !pItem )
        return;

    switch( eState )
    {
    case Product::EStateNothing:
    case Product::EStatePart:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_FB_MATERIAL" ) );
        }
        break;
    case Product::EStateOverCount:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "PRODUCT_FB_COUNT" ) );
        }
        break;
    case Product::EStateNotLearn:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEWORD( "PRODUCT_GOODS_MESSAGE", 3 ) );
        }
        break;
    case Product::EStateNotMoney:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EMITEM_MIX_FB_NOMONEY" ) );
        }
        break;
    case Product::EStateInven:
        {
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EMITEM_MIX_FB_NOINVEN" ) );
        }
        break;
    case Product::EStateRateFail:
        {
            std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_FAIL" ), pItem->GetName() );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strMsg );
        }
        break;
    case Product::EStateSuccess:
        {
            std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_SUCCESS" ), pItem->GetName() );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strMsg );
        }
        break;
	case Product::EStateGetEXP:
		{
            std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_SUCCESS" ), pItem->GetName() );
            m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strMsg );
			strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_GET_EXP" ), sRecipe->wWsp );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strMsg );
		}
		break;

	case Product::EStateCostumeFail:
		{
			std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_FAIL_COSTUME" ), pItem->GetName() );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, strMsg );
		}
		break;
    }
}

void GLProductClient::ProductOutputText( BYTE eState, WORD wKey )
{
	switch( eState )
	{
	case Product::EStateTypeAdd:
		{
			std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_TYPE_ADD" ), ID2GAMEWORD("PRODUCT_TYPE_STRING", wKey ) );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strMsg );
		}
		break;
	case Product::EStateTypeReset:
		{
			std::string strMsg = sc::string::format( ID2GAMEINTEXT( "PRODUCT_RESET_SUCCESS" ), wKey );
			m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strMsg );
		}
		break;
	}
}

//=================================================================================
//  ���� ��Ŷ ó�� �Լ�
//=================================================================================
HRESULT	GLProductClient::ReqProduct( DWORD nID, WORD nCount, bool bOngoing )
{
	if( nCount <= 0 )
		return E_FAIL;

	m_pGaeaClient->GetCharacter();
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if ( !pCharacter )
	{
		ReqProductCancel();

		return E_FAIL;
	}

	if ( !pCharacter->IsValidBody() )
	{
		ReqProductCancel();

		return E_FAIL;
	}

	if ( !pCharacter->isAction( GLAT_IDLE ) && !bOngoing )
	{
		ReqProductCancel();

		return E_FAIL;
	}

    if ( pCharacter->IsCheckedSkillFlagSpecial( EMSPECA_HALLUCINATE ) )
	{
		ReqProductCancel();

        return E_FAIL;
	}

	const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nID );
	if( !pProductRecipe )
	{
		ReqProductCancel();

		return E_FAIL;
	}

	BYTE eState;
	if ( !CheckProduct( pProductRecipe, pCharacter, eState, nCount, bOngoing ) )	//���꿡 ������ �ִٸ�
	{
		ReqProductCancel();

		ProductOutputText( eState, pProductRecipe );							//�����޽��� ���
		return E_FAIL;
	}

	// ���� ��� �˻�;
	if ( !CheckProductMaterial( pProductRecipe, pCharacter, nCount, eState ) )
	{
		ReqProductCancel();

		ProductOutputText( eState, pProductRecipe );							//�����޽��� ���
		return E_FAIL;
	}

	switch( pCharacter->VehicleState() )	//Ż�� ����
	{
	case EMVEHICLE_DRIVER:
		{
			pCharacter->ReqSetVehicle( false );
		}
		break;

	case EMVEHICLE_PASSENGER:
		{
			GLMSG::SNET_GET_OFF_WITH_VEHICLE NetMsg;
			NetMsg.dwGaeaID = m_pGaeaClient->GETMYGAEAID();
			NetMsg.bDriver = FALSE;
			m_pGaeaClient->NETSENDTOFIELD( &NetMsg );
		}
		break;
	}

	if( !pCharacter->IsActState( EM_ACT_PEACEMODE ) )
		pCharacter->ReqTogglePeaceMode();

	// ExpProductRecipe( nID );  //�ش� ���չ� ����ġ ����

	GLMSG::SNET_PRODUCT_REQ NetMsg;
	NetMsg.nKey = nID;
	NetMsg.nCount = nCount;
	NetMsg.bOngoing = bOngoing;
	m_pGaeaClient->NETSENDTOAGENT( &NetMsg );	//��Ŷ ����

	return S_OK;
}

HRESULT GLProductClient::ReqProductCancel()
{
	m_pGaeaClient->GetCharacter();
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return E_FAIL;

	/*if( !pCharacter->isAction( GLAT_MOTION ) )
		return E_FAIL;*/

	GLMSG::SNET_PRODUCT_REQ NetMsg;
    NetMsg.nKey = UINT_MAX;
    //key���� UINT_MAX���϶� ���� ��ҿ�û���� �Ǵ��Ѵ�.
	m_pGaeaClient->NETSENDTOAGENT( &NetMsg );

	return S_OK;
}

HRESULT GLProductClient::ReqLearnProductBook( WORD wPosX, WORD wPosY )
{
    GLCharacter* pChar = m_pGaeaClient->GetCharacter();
    if ( !pChar )
        return E_FAIL;

    if ( !pChar->IsValidBody() )
        return E_FAIL;

    if ( pChar->ValidWindowOpen() )
        return E_FAIL;

    SINVENITEM* pInvenItem = pChar->m_cInventory.FindPosItem( wPosX, wPosY );
    if ( !pInvenItem )
        return E_FAIL;

    wPosX = pInvenItem->wPosX;
    wPosY = pInvenItem->wPosY;

    SITEM* pItem = GLogicData::GetInstance().GetItem( pInvenItem->sItemCustom.GetNativeID() );
    if ( !pItem || pItem->sBasicOp.emItemType != ITEM_PRODUCT_BOOK )
        return E_FAIL;

    DWORD nProductBook = pItem->sSkillBookOp.sSkill_ID.dwID;

    const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nProductBook );
    if ( !pProductRecipe )
        return E_FAIL;

	// ���ռ��� ��� �� �ִ� �о߰� �ƴϸ� �޽������
	if( IsUsableProductType( pProductRecipe->eProductType ) == FALSE )
	{
		m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "UNLEARNED_PRODUCT" ) );
		return E_FAIL;
	}

    if ( IsLearnedProductRecipe( nProductBook ) )
    {
        std::string strText = sc::string::format( ID2GAMEINTEXT( "LEARN_PRODUCTBOOK_FAIL" ), pInvenItem->sItemCustom.GETNAME() );
        m_pGaeaClient->PrintMsgTextDlg( NS_UITEXTCOLOR::DISABLE, strText );
        return E_FAIL;
    }
    GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK NetMsg;
    NetMsg.nItemID = pInvenItem->sItemCustom.GetNativeID().dwID;
    NetMsg.nPosX = wPosX;
    NetMsg.nPosY = wPosY;
    m_pGaeaClient->NETSENDTOAGENT( &NetMsg );

    return S_OK;
}

HRESULT GLProductClient::ReqProductRecipeList()
{
    GLMSG::NET_PRODUCT_BOOK_LIST_REQ SendData;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pGaeaClient->NETSENDTOAGENT(NET_MSG_REQ_PRODUCTBOOK, SendBuffer);
    return S_OK;
}

HRESULT GLProductClient::ReqProductTypeList()
{
    GLMSG::NET_PRODUCT_TYPE_LIST_REQ SendData;
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PRODUCT_TYPE_LIST_REQ, SendBuffer);

	// ���� ����Ʈ ��û�Ҷ� ���õ��� ���� ��û�ϵ��� ��
	// �������� ���� UIâ ���½� �ѹ��� �ҷ������� ������ ���� �߻��Ǵ� ��û�� �ƴϹǷ� �������� ��û�ϵ��� ������.
	// Ÿ�� ��û�� ���õ��� �׻� ���� �߻��ϹǷ� ��Ʈ�� ����
	// ���µ� ��û�Ҷ� ��� ���ۼ� ��ϵ� ���� ��û�ؾ���.
	ReqProductRecipeList();
	ReqProductExpList();
    return S_OK;
}

HRESULT GLProductClient::ReqProductExpList()
{
	m_ProductRecipeExpSet.clear();
	// �����͸� ��û������ �����͸� �ʱ�ȭ

    GLMSG::NET_PRODUCT_ISGETEXP_LIST_REQ SendData;
     msgpack::sbuffer SendBuffer;
     msgpack::pack(SendBuffer, SendData);
     m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PRODUCT_ISGETEXP_LIST_REQ, SendBuffer);
    return S_OK;
}

HRESULT GLProductClient::ReqProductTypeAdd( WORD wProductType )
{
    // �н��� �ʿ��� �о��ΰ�?
    if ( IsUnableProductType( wProductType ) )
    {
        return E_FAIL;
    }
    if ( IsOpenProductType( wProductType ) )
    {
        return E_FAIL;
    }
    if ( IsStudyProductType( wProductType ) )
    {
        return E_FAIL;
    }
    GLMSG::NET_PRODUCT_TYPE_ADD_REQ SendData;
    {
        SendData.m_nProductType = wProductType;
    }
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PRODUCT_TYPE_ADD_REQ, SendBuffer);
    return S_OK;
}

HRESULT GLProductClient::ReqProductTypeDel( std::vector<int>& vecProductList, WORD wPosX, WORD wPosY )
{
	GLMSG::NET_PRODUCT_TYPE_DEL_REQ SendMsg;
    {
	    SendMsg.m_nInvecPosX = wPosX;
	    SendMsg.m_nInvecPosY = wPosY;
    }

	std::vector<int>::iterator it = vecProductList.begin();
	for( ; it != vecProductList.end(); ++it )
	{
		int wProductType = *it;
		if ( IsStudyProductType( wProductType ) ==  false )
		{
			return E_FAIL;
		}
		else
		{
			SendMsg.m_vecProductType.push_back( wProductType );
		}
	}
	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendMsg);
	m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PRODUCT_TYPE_DEL_REQ, SendBuffer);

	return S_OK;
}

//=================================================================================
//  ���� ��Ŷ ó�� �Լ�
//=================================================================================
void GLProductClient::ProductMsgProcess(  NET_MSG_GENERIC* pMsg  )
{
    switch ( pMsg->nType )
    {
    case NET_MSG_REQ_LEARN_PRODUCTBOOK_FB:
        MsgLearnProductBookFb( pMsg );
        break;
    case NET_MSG_PRODUCT_FB:
        MsgProductFb( pMsg );
        break;
    case NET_MSG_PRODUCTBOOK:
        MsgProductRecipeListAck( pMsg );
        break;
    case NET_MSG_PRODUCT_TYPE_LIST_ACK:
        MsgProductTypeListtAck( pMsg );
        break;
    case NET_MSG_PRODUCT_ISGETEXP_LIST_ACK:
        MsgProductExpListAck( pMsg );
        break;
    case NET_MSG_PRODUCT_TYPE_ADD_ACK:
        MsgProductTypeAddAck( pMsg );
        break;
    case NET_MSG_PRODUCT_TYPE_DEL_ACK:
        MsgProductTypeDelAck( pMsg );
        break;
    }
}

void GLProductClient::MsgProductFb( NET_MSG_GENERIC* pMsg )
{
    const GLMSG::SNET_PRODUCT_FB* pNetMsg = (GLMSG::SNET_PRODUCT_FB*)pMsg;

    GLCharacter* pMyChar = m_pGaeaClient->GetCharacter();
    if ( !pMyChar ) 
    {
        return;
    }
    //�ڽ��� ĳ�����϶�
    if ( pMyChar->CharDbNum() == pNetMsg->dwChaDbNum )
    {
        if ( pNetMsg->nKey == UINT_MAX) //���� ��� �϶�
        {
            if( pMyChar->IsValidBody() )
            {
                if( pMyChar->isAction( GLAT_MOTION ) )
                    pMyChar->TurnAction( GLAT_IDLE );
            }

		//!! SF_TODO
		//m_pGaeaClient->GetInterface()->MsgProductCancel();
			DelExpProductRecipe(pNetMsg->nKey);

			EndProduct();
        }
        else
        {
            const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( pNetMsg->nKey );
            if( !pProductRecipe )
                return;

            const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );
            if( !pItem )
                return;

            ProductOutputText( pNetMsg->nState, pProductRecipe );

            switch( pNetMsg->nState )
            {
            case Product::EStateMotion:
			case Product::EStateOngoingMotion:
                {
                    if( pMyChar->IsValidBody() )
                    {
                        pMyChar->SetMotion(
                            EMANI_MAINTYPE( GLCONST_CHAR::sProductMotionID.wMainID ),
                            EMANI_SUBTYPE( GLCONST_CHAR::sProductMotionID.wSubID ),
                            pProductRecipe->fDelaySec/* * static_cast< FLOAT >( pNetMsg->nCount )*/ );
                    }

					if ( pNetMsg->nState == Product::EStateMotion )
					{
						StartProduct( pItem->GetName(), pNetMsg->nCount, pProductRecipe->fDelaySec );
					}
					else
					{
						//!! SF_TODO
						//m_pGaeaClient->GetInterface()->MsgProductGoAlong();
						GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductOngoingStart );
					}

                }
                break;/////////////////////////////////////////////////////////
			case Product::EStateNeedConfirm:
				//!! SF_TODO
				//m_pGaeaClient->GetInterface()->MsgProductNeedConfirm();
				GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductNeedConfirm, "-s, -n, -f", pItem->GetName(), pNetMsg->nCount, pProductRecipe->fDelaySec );
				break;
            case Product::EStateNothing:
            case Product::EStatePart:
            case Product::EStateNotLearn:
            case Product::EStateOverCount:
            case Product::EStateNotMoney:
            case Product::EStateLevelAndSkillFail:
            case Product::EStateLevelFail:
            case Product::EStateSkillFail:
            case Product::EStateTypeFail:
            case Product::EStateInven:
			case Product::EStateCostumeFail:
                {
                    DxSoundLib::GetInstance()->PlaySound( "GRINDING_FAIL" );
                }
            case Product::EStateCancel:
            case Product::EStateComplete:   // ���� �Ϸ��
                {
                    if( pMyChar->IsValidBody() )
                    {
                        if( pMyChar->isAction( GLAT_MOTION ) )
                            pMyChar->TurnAction( GLAT_IDLE );
                    }

					EndProduct();
                }
                break;/////////////////////////////////////////////////////////
            case Product::EStateRateFail:   // Ȯ�� ���н�
                {
					DelExpProductRecipe(pNetMsg->nKey);

                    DxSoundLib::GetInstance()->PlaySound( "GRINDING_FAIL" );

					// �����ص� Progress Bar�������� �����Ѵ�.
					// Lua �� Success�� ȣ��ÿ� ProgressBar�� ���⶧����
					// ���� ��ũ��Ʈ�� �������� �ð��� �޾ƿ��� ����̾��µ�
					// ���� �Ϸ�Ǹ� �ϳ��� ���� ������� ������

					m_iMakeCount += 1;
					if( m_iMakeCount < pProductRecipe->sResultItem.nNum )
					{
						break ;
					}

					m_iMakeCount= 0;

					GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductSuccess );
					GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
                }
                break;/////////////////////////////////////////////////////////
            case Product::EStateGetEXP: //���ۿ� �����ϰ� ����ġ�� ����� ��    (break���� ���� EStateSuccess������ �Բ� ���)
                {
                    ExpProductRecipe( pNetMsg->nKey );  //�ش� ���չ� ����ġ ����
                }
            case Product::EStateSuccess:    //���ۿ� �����ϰ� ����ġ�� ���������� ��
                {
                    DxSoundLib::GetInstance()->PlaySound( "GRINDING_SUCCEED" );

					m_iMakeCount += 1;
					if( m_iMakeCount < pProductRecipe->sResultItem.nNum )
					{
						break ;
					}

					m_iMakeCount= 0;

					// EndProduct();
					GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductSuccess );
					GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
                }
                break;/////////////////////////////////////////////////////////
            }
        }
        
    }
    //�ֺ� ĳ�����϶�
    else
    {
        const DWORD dwMygaeaID = m_pGaeaClient->GETMYGAEAID();
        std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar( pNetMsg->dwCharGaeaID );
        if( !pChar )
        {
            return;
        }

        if( !pChar->IsValidBody() )
        {
            return;
        }
        if ( pNetMsg->nKey == UINT_MAX) //���� ��� �϶�
        {
            if( pChar->isAction( GLAT_MOTION ) )
                pChar->TurnAction( GLAT_IDLE );
        }
        else
        {
            switch( pNetMsg->nState )
            {
            case Product::EStateMotion: //���� ���
			case Product::EStateOngoingMotion:
                {
                    const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( pNetMsg->nKey );
                    if( !pProductRecipe )
                        return;
                    pChar->SetMotion(
                        EMANI_MAINTYPE( GLCONST_CHAR::sProductMotionID.wMainID ),
                        EMANI_SUBTYPE( GLCONST_CHAR::sProductMotionID.wSubID ),
                        pProductRecipe->fDelaySec/* * static_cast< FLOAT >( pNetMsg->nCount )*/ );
                }
                break;
            default:
                {
                    if( pChar->isAction( GLAT_MOTION ) )
                        pChar->TurnAction( GLAT_IDLE );
                }
                break;
            }
        }
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >( "Product_UpdateList" );
}

void GLProductClient::MsgLearnProductBookFb( NET_MSG_GENERIC* pMsg )
{
    const GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB* pNetMsg = (GLMSG::SNETPC_REQ_LEARN_PRODUCTBOOK_FB*)pMsg;

    const SITEM* pItem = GLogicData::GetInstance().GetItem( pNetMsg->nItemID );
    if( !pItem )
        return;

    LearnProductRecipe( pItem->sSkillBookOp.sSkill_ID.dwID );

    std::string strText = sc::string::format( ID2GAMEINTEXT( "LEARN_PRODUCTBOOK_SUCCESS" ), pItem->GetName() );
    m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::PALEGREEN, strText );

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLProductClient::MsgProductRecipeListAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_PRODUCT_BOOK_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if ( m_pGaeaClient->GetCharacter()->m_CharDbNum != RecvData.m_dwChaDbNum )  //CharDbNum�� ��ġ�ϴ��� Ȯ��
    {
        return;    
    }
    m_ProductRecipeLearnSet.clear();
    const int nVecSize = RecvData.m_vecBookList.size();
    for ( int i = 0; i < nVecSize; i++ )
    {
        DWORD dwRecipe = RecvData.m_vecBookList[i];
        LearnProductRecipe( dwRecipe );
    }

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
}

void GLProductClient::MsgProductTypeListtAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_PRODUCT_TYPE_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if ( m_pGaeaClient->GetCharacter()->m_CharDbNum != RecvData.m_dwChaDbNum )  //CharDbNum�� ��ġ�ϴ��� Ȯ��
    {
        return;    
    }
    ClearStudyProducttype();
    const int nVecSize = RecvData.m_vecType.size();
    for ( int i = 0; i < nVecSize; i++ )
    {
        WORD wType = (WORD)RecvData.m_vecType[i];
        SetStudyProductType( wType, true );
    }
    m_nProductTypeMaxStudyPoint = (WORD)GLCONST_CHAR::dwStudyPointDefault;
    m_nProductTypeStudyPoint = m_nProductTypeMaxStudyPoint - nVecSize;

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ProductUpdateType );
}

void GLProductClient::MsgProductExpListAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if ( m_pGaeaClient->GetCharacter()->m_CharDbNum != RecvData.m_dwChaDbNum )  //CharDbNum�� ��ġ�ϴ��� Ȯ��
    {
        return;    
    }

	if( RecvData.m_bFirst == true ) 	
		m_ProductRecipeExpSet.clear();
    // ������ ����ߴ��� ó�� m_bFirst�� ���� �ʱ�ȭ�ؾ��Ѵ�
	// �ٸ� �������� ��Ƽ� ������ ����
	// �α׾ƿ��� m_bFirst�� True�� �ٲ��ִ� �������� ���� 


    const int nVecSize = RecvData.m_vecIsGetExp.size();
    for ( int i = 0; i < nVecSize; i++ )
    {
        const DWORD dwExp = (DWORD)RecvData.m_vecIsGetExp[i];
        m_ProductRecipeExpSet.insert( dwExp );
    }

	if( RecvData.m_bLast == true )
	{
		CountProductSkill();    //����ġ���� ���չ� ����Ʈ�� ����ġ ���

		GLWidgetScript::GetInstance().LuaCallFunc< void >(
			NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );
	}
}

void GLProductClient::MsgProductTypeAddAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_PRODUCT_TYPE_ADD_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if ( m_pGaeaClient->GetCharacter()->m_CharDbNum != RecvData.m_dwChaDbNum )  //CharDbNum�� ��ġ�ϴ��� Ȯ��
    {
        return;    
    }
    if ( RecvData.m_bOK == false )
    {
        return;
    }
    if ( Product::ETypeHeadGear > RecvData.m_nProductType || RecvData.m_nProductType >= Product::ETypeTotal )
    {
        return;
    }
    StudyProductType( RecvData.m_nProductType );

	//Enum Type�� ���ӿ��� �ε����� ��ġ���� �ʱ� ������ +1�� ���ش�
	ProductOutputText( Product::EStateTypeAdd, RecvData.m_nProductType+1 ); //�Լ����� �����ϴ�. �׷��� �������� ������ �ִ�.)

	GLWidgetScript::GetInstance().LuaCallFunc< void >( "MsgProductTypeAddAck" );
}

void GLProductClient::MsgProductTypeDelAck( NET_MSG_GENERIC* pMsg )
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*)pMsg;
    GLMSG::NET_PRODUCT_TYPE_DEL_ACK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;
    if ( m_pGaeaClient->GetCharacter()->m_CharDbNum != RecvData.m_dwChaDbNum )  //CharDbNum�� ��ġ�ϴ��� Ȯ��
    {
        return;    
    }
    if ( RecvData.m_bOK == false )
    {
        return;
    }
    const int nVecSize = RecvData.m_vecProductType.size();
    for ( int i = 0; i < nVecSize; i++ )
    {
        SetStudyProductType( RecvData.m_vecProductType[i] , false );    //�ش� ���ۺо߸� �ʱ�ȭ
    }
    m_nProductTypeStudyPoint += nVecSize;   //�ʱ�ȭ �� ������ŭ �н�����Ʈ ����
    ReqProductRecipeList(); //�ٽ� ���ռ� ����� ��û
    ReqProductExpList();    //�ٽ� ����ġ ����� ��û

	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ProductRefresh );

	ProductOutputText( Product::EStateTypeReset, nVecSize );
}

lua_tinker::table GLProductClient::GetStudiedProducts()
{
	lua_tinker::table tbProduct( GLWidgetScript::GetInstance().GetLuaState() );

	int nStudied = 1;
	for( int i=Product::ETypeHeadGear; i<Product::ETypeTotal; ++i )
	{
		if( IsStudyProductType( i ) )
		{
			tbProduct.set( nStudied, i );
			++nStudied;
		}
	}

	return tbProduct;
}

lua_tinker::table GLProductClient::GetProductList( const BYTE eFindType, const BYTE eFindClass, const bool bShowProductAble )
{
	lua_tinker::table tbProductList( GLWidgetScript::GetInstance().GetLuaState() );
	FindProductList( m_PtrVectorProductRecipe, eFindType, eFindClass );
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return tbProductList;

	if( bShowProductAble )
	{
		FilteringProduct();
	}

	int nTotalLine = static_cast<int>( m_PtrVectorProductRecipe.size() );
	int nProductListCount = 1;
	for( int i = 0; i < nTotalLine; ++i )
	{
		lua_tinker::table tbProduct( GLWidgetScript::GetInstance().GetLuaState() );
		SITEM* pItem = GLItemMan::GetInstance().GetItem( m_PtrVectorProductRecipe[ i ]->sResultItem.sNID );
		if( !pItem )
			continue;

		CString strItem = pItem->GetName();
		BYTE eState = 0;
		DWORD dwColor = 0;
		int nStrCount = 0;

		const WORD nProductMaterialCount = CountProductMaterial( m_PtrVectorProductRecipe[ i ], pCharacter, eState );
		if( CheckProductLearn( m_PtrVectorProductRecipe[i] ) == false )
		{
			eState = Product::EStateNotLearn;
			dwColor = NS_UITEXTCOLOR::RED;
		}
		else
		{
			dwColor = ITEM_INFOR_COLOR::dwItemRank[ pItem->sBasicOp.emLevel ];
		}
		nStrCount = MultiByteToWideChar(CP_ACP, 0, strItem, strItem.GetLength(), NULL, NULL);

		tbProduct.set( "itemName", strItem.GetString() );
		tbProduct.set( "nStrCount", nStrCount );
		tbProduct.set( "eState", eState );
		tbProduct.set( "dwColor", dwColor );
		tbProduct.set( "nProductCount", nProductMaterialCount );
		tbProduct.set( "nIndex", i );

		tbProductList.set( nProductListCount++, tbProduct );
	}

	return tbProductList;
}

std::string GLProductClient::GetSelectedProductName( const unsigned int nScrollPos )
{
	std::string strTemp = "";
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return strTemp;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return strTemp;

	SITEM* pItem = GLItemMan::GetInstance().GetItem( sProductRecipe->sResultItem.sNID );

	return pItem->GetNameStr();
}

const DWORD GLProductClient::GetSelectedProductCost( const unsigned int nScrollPos )
{
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return 0;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return 0;

	return sProductRecipe->dwPrice;
}

lua_tinker::table GLProductClient::GetSelectedProductData( const unsigned int nScrollPos )
{
	lua_tinker::table tbProductData( GLWidgetScript::GetInstance().GetLuaState() );
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return tbProductData;

	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return tbProductData;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return tbProductData;

	int nProductListCount = 1;
	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL ; i++ )
	{
		lua_tinker::table tbProduct( GLWidgetScript::GetInstance().GetLuaState() );
		const SProductItem& sProductItem = sProductRecipe->sMeterialItem[ i ];

		int nItemCnt = sProductItem.nNum;
		if( nItemCnt <= 0 )
			continue;

		SITEM* pItem = GLItemMan::GetInstance().GetItem( sProductItem.sNID );
		if( !pItem )
			continue;

		DWORD nMaterial = pCharacter->m_cInventory.CountTurnItem( sProductItem.sNID );

		tbProduct.set( 1, sProductItem.sNID.dwID );
		tbProduct.set( 2, nItemCnt );
		tbProduct.set( 3, nMaterial );

		tbProductData.set( nProductListCount, tbProduct );
		++nProductListCount;
	}
	return tbProductData;
}

lua_tinker::table GLProductClient::GetSelectedProductRequirment( const unsigned int nScrollPos )
{
	lua_tinker::table tbRequirmentData( GLWidgetScript::GetInstance().GetLuaState() );
	tbRequirmentData.set( 1, false );
	tbRequirmentData.set( 2, "" );
	tbRequirmentData.set( 3, "" );
	tbRequirmentData.set( 4, "" );
	tbRequirmentData.set( 5, false );
	tbRequirmentData.set( "msg", "" );
	tbRequirmentData.set( "msgColor", "" );

	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return tbRequirmentData;

	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return tbRequirmentData;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return tbRequirmentData;

	if( CheckProductLearn( sProductRecipe ) == false ) //���չ��� �̽����;
	{
		tbRequirmentData.set( 1, false );
	}
	else
	{
		tbRequirmentData.set( 1, true );
		//����üũ�� �ʿ��ϸ鼭 �ʿ䷹���� �����ϴٸ�;
		if ( CheckProductLevel( sProductRecipe, pCharacter->m_wLevel ) == false )
		{
			tbRequirmentData.set( 2, sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 11 ), sProductRecipe->wLevel ).c_str() );
		}
		//���õ�üũ�� �ʿ��ϸ鼭 �ʿ������ �����ϴٸ�;
		if ( CheckProductSkill( sProductRecipe ) == false )
		{
			tbRequirmentData.set( 3, sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 12 ), sProductRecipe->wWs ).c_str() );
		}
		//����ġ�� ���� �� �ִ� ���չ��̸鼭 ����ġ ȹ���� ������ ���ٸ�;
		if (sProductRecipe->wWsp != Product::PRODUCT_NOT_USE_REQ
			&& m_pGaeaClient->GetProductClient()->IsExpProductRecipe( sProductRecipe->dwKey ) == false )
		{
			tbRequirmentData.set( 4, sc::string::format(ID2GAMEWORD("PRODUCT_WINDOW_TEXT", 13 ), sProductRecipe->wWsp , sProductRecipe->wWspPer ).c_str() );
		}
	}

	std::string strProductMessage;
	BYTE eState;
	if ( CheckProduct( sProductRecipe, pCharacter, eState , 1 ) )  //���� ���ɽ�
	{
		if( sProductRecipe->dwRate == Product::PRODUCT_NOT_USE_REQ )
		{
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 2 );
		}
		else
		{
			strProductMessage = sc::string::format(ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 1 ), sProductRecipe->dwRate);   //Ȯ�����۽�
		}
		tbRequirmentData.set( 5, true );
		tbRequirmentData.set( "msgColor", "#0BFF00" );
	}
	else    //���� �Ұ���
	{
		switch( eState )
		{
		case Product::EStateNothing:
		case Product::EStatePart:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 0 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateNotLearn:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 3 );
			tbRequirmentData.set( "msgColor", "#FF0000" );
			break;
		case Product::EStateNotMoney:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 4 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateLevelFail:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 5 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateSkillFail:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 6 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateLevelAndSkillFail:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 7 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateTypeFail:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 8 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		case Product::EStateInven:
			strProductMessage = ID2GAMEWORD("PRODUCT_GOODS_MESSAGE", 9 );
			tbRequirmentData.set( "msgColor", "#FF8500" );
			break;
		}
	}
	tbRequirmentData.set( "msg", strProductMessage.c_str() );

	return tbRequirmentData;
}

lua_tinker::table GLProductClient::GetSelectedProductResultData( const unsigned int nScrollPos )
{
	lua_tinker::table tbResultData( GLWidgetScript::GetInstance().GetLuaState() );
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return tbResultData;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return tbResultData;

	tbResultData.set(1, sProductRecipe->sResultItem.sNID.dwID);
	tbResultData.set(2, sProductRecipe->sResultItem.nNum);
	return tbResultData;
}

const DWORD GLProductClient::GetSelectedProductKey( const unsigned int nScrollPos )
{
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return 0;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return 0;

	return sProductRecipe->dwKey;
}

const FLOAT GLProductClient::GetSelectedProductDelay( const unsigned int nScrollPos )
{
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return 0;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return 0;

	return sProductRecipe->fDelaySec;
}

const FLOAT GLProductClient::GetSelectedProductRate( const unsigned int nScrollPos )
{
	unsigned int nTotalLine = m_PtrVectorProductRecipe.size();
	if( nTotalLine < nScrollPos )
		return 0;

	SProductRecipe*	sProductRecipe = m_PtrVectorProductRecipe[nScrollPos];
	if( !sProductRecipe )
		return 0;

	return sProductRecipe->dwRate;
}

void GLProductClient::ReqProductTypeDelete( lua_tinker::table tbProductList, WORD wPosX, WORD wPosY )
{
	GLMSG::NET_PRODUCT_TYPE_DEL_REQ SendMsg;
	SendMsg.m_nInvecPosX = wPosX;
	SendMsg.m_nInvecPosY = wPosY;

	for( int i = 1; i <= tbProductList.TableLen(); ++i )
	{
		int nProductType = tbProductList.get< int >( i );
		if( -1 == nProductType )
			continue;

		if( false == IsStudyProductType( nProductType ) )
			continue;

		SendMsg.m_vecProductType.push_back( nProductType );
	}

	msgpack::sbuffer SendBuffer;
	msgpack::pack(SendBuffer, SendMsg);
	m_pGaeaClient->NETSENDTOAGENT(NET_MSG_PRODUCT_TYPE_DEL_REQ, SendBuffer);
}

const BYTE GLProductClient::GetStudyState( WORD wProductType )
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal )
		return Product::E_STUDY_NONE;

	if( true == IsUnableProductType( wProductType ) )
		return Product::E_STUDY_UNABLE;

	if( true == IsOpenProductType( wProductType ) )
		return Product::E_STUDY_PUBLIC;

	if( true == IsStudyProductType( wProductType ) )
		return Product::E_STUDY_STUDIED;

	if( 0 == GetProductTypeStudyPoint() )
		return Product::E_STUDY_DISABLE;

	return Product::E_STUDY_NEED;
}

void GLProductClient::FilteringProduct()
{
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	if( !pCharacter )
		return;

	PtrVector_ProductRecipe PtrVecFilteringData;
	int nTotalLine = static_cast<int>( m_PtrVectorProductRecipe.size() );

	for( int i=0; i<nTotalLine; ++i )
	{
		BYTE eState;
		if ( CheckProductLearn( m_PtrVectorProductRecipe[i] ) == false )
		{
			continue;
		}
		if ( CountProductMaterial( m_PtrVectorProductRecipe[ i ], pCharacter, eState ) == 0 )
		{
			continue;
		}
		PtrVecFilteringData.push_back( m_PtrVectorProductRecipe[ i ] );
	}
	m_PtrVectorProductRecipe.clear();
	m_PtrVectorProductRecipe = PtrVecFilteringData;
}

void GLProductClient::StartProduct( const char* cItemName, int nCount, float fDelaySec )
{
	if( fDelaySec <= 0.0f )
		return;
	
	m_iMakeCount = 0;


	GLWidgetScript::GetInstance().LuaCallFunc< void >(
		NSWIDGET_SCRIPT::g_strFunc_ProductStart, "-s, -n, -f", cItemName, nCount, fDelaySec );
}

void GLProductClient::EndProduct()
{
	m_iMakeCount= 0;
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_ProductEnd );
}
