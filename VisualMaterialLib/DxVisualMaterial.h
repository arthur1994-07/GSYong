#pragma once

#include "./Define/DxVMCommandDef.h"
#include "./Define/DxVMDefine.h"

class DxVMCommand;
class DxVMCommandFX;

enum VM_EDIT_MODE
{
    VMEM_ZERO,
    VMEM_SELECT_BOX,            // 기본 틀. box 형식으로 선택
    VMEM_SELECT_POINT,          // 기본 틀. point 형식으로 선택
    VMEM_SELECT_BOX_FOR_LINE,   // 작은 Box
};

class DxVisualMaterial
{
private:
    enum
    {
        VERSION = 0x105,    // 기본적인 Version
    };

public:
    enum
    {
        VERSION_FX = 0x108, // Fx 파일을 다시 생성해주어야 할 경우 이것을 올린다.
    };
    

public:
    static BOOL         g_bLineCurve;           // Line을 직선 / Curve

private:
    float               m_fTime;
    LPDIRECT3DDEVICEQ   m_pd3dDevice;

    // Save & Load
private:
    TSTRING             m_strFileName;          // 이름
    MAP_VM_COMMAND      m_mapVM_Command;        // Data

    // Edit
private:
    VM_CAN_SELECT_INDEX     m_sCanSelectIndex;      // 선택가능한 Box
    VM_CAN_SELECT_INDEX     m_sSelectIndexBox;      // 선택한 Box
    D3DXVECTOR3             m_vMouseDownPoint;      // 마우스가 Down 된 곳 Point
    SET_DWORD               m_setSelectCommand;     // 선택된 Command
    VM_EDIT_MODE            m_emEditMode;
    D3DXVECTOR2             m_vSelectBoxMin;
    D3DXVECTOR2             m_vSelectBoxMax;
    bool                    m_bStartMoveVMCommand;  // VMCommand를 움직이기 시작.
    D3DXVECTOR3             m_vPreMoveMousePos;     // 이동 전의 마우스 좌표를 저장하기 위함.

public:
    void Create( LPDIRECT3DDEVICEQ pd3dDevice );
    void OnResetDevice();
    void OnLostDevice();
    void FrameMove( float fElapsedTime );
    void Render();
    void CreateDataFX();

    //Edit
public:
    //------------------------------------------------ Mouse Up
    void MouseUp();

    //------------------------------------------------ CreateCommand
    void CreateCommand( VMC_TYPE emType );

    //------------------------------------------------ Is Camera Move
    BOOL IsCameraMove();

    //------------------------------------------------- 단 하나만 선택되었을 경우, 선택된 Command를 보내준다.
    DxVMCommand* GetCommand();

    //------------------------------------------------- 파일이름을 가져온다.
    const TSTRING& GetFileName();

private:
    void RenderLineNow();
    void RenderLine();
    void RenderHermiteLine( const D3DXVECTOR3& vLine1, const D3DXVECTOR3& vLine2, DWORD dwColor );
    D3DXVECTOR3 GetHermiteLastDir( const D3DXVECTOR3& vLine1, const D3DXVECTOR3& vLine2 );
    void GetPointInput( D3DXVECTOR3& vPoint, DWORD dwID, int nIndexBox );
    void GetPointOutput( D3DXVECTOR3& vPoint, DWORD dwID, int nIndexBox );
    DWORD GetLineColor( DWORD dwInputID, int nInputIndexBox, DWORD dwOutputID, int nOutputIndexBox );

    //------------------------------------------------- Select Box For Link
    BOOL Action_SelectBoxForLink();

    //------------------------------------------------- Link Box To Box
    BOOL Action_LinkBoxToBox( const VM_CAN_SELECT_INDEX& sCanSelectIndex );
    BOOL SetLineLink2( DWORD dwID, int nIndexBox, const VM_CAN_SELECT_INDEX& sCanSelectIndex );

    //------------------------------------------------- Delete Box For Link
    BOOL Action_DeleteBoxForLink( const VM_CAN_SELECT_INDEX& sSelectIndexBox );

    //------------------------------------------------- Move Command
    BOOL Action_MoveCommand();

    //------------------------------------------------- Select Command
    BOOL Action_SelectCommand( BOOL bBox );
    void Action_SelectCommand_Point();
    void Action_SelectCommand_Point2( DWORD dwSelectID );

    //------------------------------------------------- 함수를 이용해서 m_setSelectCommand 변수에 추가, 삭제함
    //                                                  m_bChangeSelectCommand 때문에 이렇게 하고 있음
    void InsertSelectCommand( DWORD dwSelectID );
    void DeleteSelectCommand( SET_DWORD_ITER& iter );
    void ClearSelectCommand();

    //------------------------------------------------- Select Command At Box
    BOOL Action_SelectBox();
    void Render_SelectBoxLine();

    //------------------------------------------------- Key Process
    void KeyProcess();

    //------------------------------------------------- Delete Command
    void DeleteCommand();
    void DeleteCommand2( DWORD dwID );

    //------------------------------------------------- Etc
    BOOL GetMousePointWorld( D3DXVECTOR3& vMousePt );
    DWORD GetEmptyIndex_Command();
    void SetEditMode( VM_EDIT_MODE emMode );

    // Save & Load
public:
    void Save( const TSTRING& strFullName, const TSTRING& strFileName );
    BOOL Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName );
    void CleanUp();

public:
	DxVisualMaterial(void);
	~DxVisualMaterial(void);
};
