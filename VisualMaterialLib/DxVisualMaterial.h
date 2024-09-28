#pragma once

#include "./Define/DxVMCommandDef.h"
#include "./Define/DxVMDefine.h"

class DxVMCommand;
class DxVMCommandFX;

enum VM_EDIT_MODE
{
    VMEM_ZERO,
    VMEM_SELECT_BOX,            // �⺻ Ʋ. box �������� ����
    VMEM_SELECT_POINT,          // �⺻ Ʋ. point �������� ����
    VMEM_SELECT_BOX_FOR_LINE,   // ���� Box
};

class DxVisualMaterial
{
private:
    enum
    {
        VERSION = 0x105,    // �⺻���� Version
    };

public:
    enum
    {
        VERSION_FX = 0x108, // Fx ������ �ٽ� �������־�� �� ��� �̰��� �ø���.
    };
    

public:
    static BOOL         g_bLineCurve;           // Line�� ���� / Curve

private:
    float               m_fTime;
    LPDIRECT3DDEVICEQ   m_pd3dDevice;

    // Save & Load
private:
    TSTRING             m_strFileName;          // �̸�
    MAP_VM_COMMAND      m_mapVM_Command;        // Data

    // Edit
private:
    VM_CAN_SELECT_INDEX     m_sCanSelectIndex;      // ���ð����� Box
    VM_CAN_SELECT_INDEX     m_sSelectIndexBox;      // ������ Box
    D3DXVECTOR3             m_vMouseDownPoint;      // ���콺�� Down �� �� Point
    SET_DWORD               m_setSelectCommand;     // ���õ� Command
    VM_EDIT_MODE            m_emEditMode;
    D3DXVECTOR2             m_vSelectBoxMin;
    D3DXVECTOR2             m_vSelectBoxMax;
    bool                    m_bStartMoveVMCommand;  // VMCommand�� �����̱� ����.
    D3DXVECTOR3             m_vPreMoveMousePos;     // �̵� ���� ���콺 ��ǥ�� �����ϱ� ����.

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

    //------------------------------------------------- �� �ϳ��� ���õǾ��� ���, ���õ� Command�� �����ش�.
    DxVMCommand* GetCommand();

    //------------------------------------------------- �����̸��� �����´�.
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

    //------------------------------------------------- �Լ��� �̿��ؼ� m_setSelectCommand ������ �߰�, ������
    //                                                  m_bChangeSelectCommand ������ �̷��� �ϰ� ����
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
