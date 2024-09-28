
#pragma	once

#include "../Util/UIMeshRender.h"

#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/Vehicle/GLVEHICLE.h"
#include "../../../EngineLib/DxMeshs/DxAniKeys.h"
#include "../../../EngineLib/G-Logic/TypeDefine.h"

class ClientActor;
class GLGaeaClient;

//class CUIMeshRender;
class CUIMeshData_SkinChar;
class GLGaeaClient;
struct GLCHARLOGIC;

class CharacterView : public CUIMeshRender
{
public:

	enum EMPREVIEWSLOT
	{
		EMPREVIEWSLOT_HEAD,
		EMPREVIEWSLOT_UPPER,
		EMPREVIEWSLOT_LOWER,
		EMPREVIEWSLOT_HAND,
		EMPREVIEWSLOT_FOOT,
		EMPREVIEWSLOT_WEAPON,

		// 기획팀에서 셋팅을 코스츔이 되도록 해버렸다. 지원해줘야한다. (EMPREVIEWSLOT_DECORATION,EMPREVIEWSLOT_EARRINGS)
		EMPREVIEWSLOT_DECORATION,
		EMPREVIEWSLOT_EARRINGS,

		EMPREVIEWSLOT_SIZE,
	};

	struct SPIECEINFO
	{
		std::string szFILENAME;
		std::string szSELFBODYEFF;
		BYTE        cGRADE;
		DWORD       dwMAINCOLOR;
		DWORD       dwSUBCOLOR;
		BOOL        bNONCHARDATA;

		SPIECEINFO() 
			: cGRADE(0)
			, dwMAINCOLOR(0)
			, dwSUBCOLOR(0)
			, bNONCHARDATA(FALSE)
		{
		}

		bool operator==(const SPIECEINFO& other) const
		{
			return szFILENAME == other.szFILENAME &&
				szSELFBODYEFF == other.szSELFBODYEFF &&
				cGRADE == other.cGRADE &&
				dwMAINCOLOR == other.dwMAINCOLOR &&
				dwSUBCOLOR == other.dwSUBCOLOR &&
				bNONCHARDATA == other.bNONCHARDATA;
		}
		
		bool operator!=(const SPIECEINFO& other) const
		{
			return !operator==(other);
		}

		void RESET()
		{
			szFILENAME.clear();
			szSELFBODYEFF.clear();
			cGRADE       = 0;
			dwMAINCOLOR  = 0;
			dwSUBCOLOR   = 0;
			bNONCHARDATA = FALSE;
		}
	};

	struct BackupCharacterData
	{
		SPIECEINFO Head;
		SPIECEINFO Hair;
		SITEMCUSTOM Item[SLOT_NSIZE_S_2];
		bool isUseArmSub;

		BackupCharacterData() { isUseArmSub = false; }

		bool operator==(const BackupCharacterData& other) const
		{
			if( Head != other.Head || Hair != other.Hair )
				return false;

			for (int i=0; i < SLOT_NSIZE_S_2; ++i)
			{
				if( Item[i].GetNativeID() != other.Item[i].GetNativeID() )
					return false;
			}

			if( isUseArmSub != other.isUseArmSub )
				return false;
				
			return true;
		}

		bool operator!=(const BackupCharacterData& other) const
		{
			return !operator==(other);
		}
	};

	struct VehicleData
	{
		SNATIVEID VehicleID;
		SNATIVEID Parts[ACCE_TYPE_SIZE];
		SVEHICLE_COLOR ColorArray[ACCE_TYPE_SIZE];


		VehicleData()
		{
			VehicleID = NATIVEID_NULL();
			for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
			{
				Parts[i] = NATIVEID_NULL();
				ColorArray[i] = SVEHICLE_COLOR();
			}
		}

		bool operator==(const VehicleData& other) const
		{
			if( VehicleID != other.VehicleID )
				return false;

			for (int i = 0; i < ACCE_TYPE_SIZE; ++i)
			{
				if( Parts[i] != other.Parts[i] )
					return false;

				if( ColorArray[i] != other.ColorArray[i] )
					return false;
			}

			return true;
		}

		bool operator!=(const VehicleData& other) const
		{
			return !operator==(other);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	CharacterView(EngineDeviceMan* pEngineDevice);

//	void SetCameraZ(float z);

	void SetScale(float scale);



	void SetUpdateCharacter(GLCHARLOGIC* pCharacter,bool isForce);

	void SetUpdateVehicle(const VehicleData& data,bool isForce);

	void Clear();
	void DeleteUIMeshData();

	void SetAnimationStop();
	void SetAnimationWalk();

	// 액터의 행동을 기억할 필요가 있다면 - 무기를 착용하는지 뒤에 매는지 여부를 결정하기 위해
	void SetPeaceModeData(ClientActor* pActor,GLGaeaClient* pGaeClient);

private:
	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

//////////////////////////////////////////////////////////////////////////
	void SetCamPosX(float value);
	void SetCamPosY(float value);
	void SetCamPosZ(float value);

	void SetPosition(float x,float y,float z);
	void SetRotationY(float rot);


private:
	void InitGeometry();
	void ResetAllItem();
	void ResetItem( EMSLOT emSlot );
	void ResetPiece( EMPIECECHAR emPiece );
	EMPIECECHAR SetPiece( const SPIECEINFO& sInfo );

	EMPREVIEWSLOT Slot_2_PreviewSlot( const EMSLOT emSlot );
	DWORD ConvertColor( const WORD wColor );
	BOOL IsPreviewItem( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot );
	void SetItem( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot );

	void SetCharacterMatrix();

	BackupCharacterData MakeBackupCharacterData(GLCHARLOGIC* pCharacter);
	BOOL LoadCharacter( GLCHARLOGIC* pCharacter );

	//////////////////////////////////////////////////////////////////////////
	void SetVehicleMatrix();

	BOOL LoadVehicle(const VehicleData& data);

	bool IsVisibleVehicle();
	
	
	void SetAnimationHelper(EMANI_MAINTYPE vehicleAni,EMANI_SUBTYPE bikeAni,EMANI_MAINTYPE boardAni);


private:
	SNATIVEID      m_ForAniVehicleID;
	EMANI_MAINTYPE m_VehicleAni;
	EMANI_SUBTYPE  m_BikeAni;
	EMANI_MAINTYPE m_BoardAni;


//	CUIMeshRender*          m_pMeshRender;
	EMCHARINDEX					m_emCharIndex;
	CUIMeshData_SkinChar*		m_pMeshCharacterData;
	CUIMeshData_SkinVehicle*	m_pMeshVehicleData;

	EMANI_MAINTYPE			m_emAniMainType;
	EMANI_SUBTYPE			m_emAniSubType;

	bool					m_initMatrix;
	float					m_Scale;
	float					m_CamPosX;
	float					m_CamPosY;
	float					m_CamPosZ;

	float                   m_ObjectPosX;
	float                   m_ObjectPosY;
	float                   m_ObjectPosZ;

	
	float					m_RotationY;


	D3DXMATRIX              m_matWorld;
	D3DXMATRIX              m_matProj;
	D3DXMATRIX              m_matView;
	D3DXVECTOR3				m_vCameraFromPt;


	BackupCharacterData m_UpdateBackUpCharacterData;
	BackupCharacterData m_BackUpCharacterData;

	VehicleData m_UpdateBackUpVehicleData;
	VehicleData m_BackUpVehicleData;


	ClientActor*  m_pActor;
	GLGaeaClient* m_pGaeClient;
};
