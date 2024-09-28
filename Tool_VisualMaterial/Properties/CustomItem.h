#pragma once

#define _XTP_STATICLINK
#include <XTToolkitPro.h>    // Xtreme Toolkit Pro components

class DxVMCommand;

//------------------------------------------------------------------ CCustomItemFileBox
enum FILE_TYPE
{
    EMFM_TEXTURE    = 0,
    EMFM_XFILE      = 1,
};

class CCustomItemFileBox : public CXTPPropertyGridItem
{
private:
    TSTRING         m_strDefExt;
    TSTRING         m_strFilter;
    TSTRING         m_strPath;
    TSTRING*        m_rstrEdit;
    DxVMCommand*    m_rVMCommand;

public:
	CCustomItemFileBox( CString strCaption, 
                        const TSTRING& strDefExt, 
                        const TSTRING& strFilter, 
                        const TSTRING& strPath, 
                        TSTRING* pstrEdit,
                        DxVMCommand* pVMCommand );

    CCustomItemFileBox( CString strCaption, 
                        FILE_TYPE emFileType, 
                        TSTRING* pstrEdit,
                        DxVMCommand* pVMCommand );


protected:
	virtual void OnInplaceButtonDown(CXTPPropertyGridInplaceButton* pButton);
    virtual void OnValueChanged(CString strValue);
};


//------------------------------------------------------------------ CCustomItemEdit
class CCustomItemEdit : public CXTPPropertyGridItem
{
private:
    TSTRING* m_rstrEdit;

public:
	CCustomItemEdit( CString strCaption, TSTRING* pstrEdit );

protected:
	virtual void OnValueChanged(CString strValue);
};


//------------------------------------------------------------------ CCustomItemEditFloat
class CCustomItemEditFloat : public CXTPPropertyGridItem
{
private:
    float* m_rfValue;

public:
	CCustomItemEditFloat( CString strCaption, float* pfValue, const TCHAR* pEdit );

protected:
	virtual void OnValueChanged(CString strValue);
};

//------------------------------------------------------------------ CCustomItemComboBox
class CCustomItemComboBox : public CXTPPropertyGridItem
{
private:
    int* m_rnValue;

public:
	CCustomItemComboBox( CString strCaption, int* pnValue, const TCHAR* pEdit );

protected:
	virtual void OnValueChanged(CString strValue);
};

//------------------------------------------------------------------ CCustomItemCheckBox
class CCustomItemCheckBox : public CXTPPropertyGridItemBool
{
private:
    DxVMCommand*    m_rVMCommand;

public:
	CCustomItemCheckBox( CString strCaption, BOOL bValue, BOOL* pbValue, DxVMCommand* pVMCommand );

protected:
	virtual void SetBool(BOOL bValue);
};
