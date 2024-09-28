/**************************************************************************

Filename    :   CLIKDataBinding.h
Content     :   Data binding interface for CLIK components in MMO Kit.
Created     :
Authors     :   Prasad Silva, Nate Mitchell

Copyright   :   Copyright 2011 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#ifndef INC_CLIKDataBinding_H
#define INC_CLIKDataBinding_H

#include "../../RanThirdParty/GFx/Src/Kernel/SF_RefCount.h"

using namespace Scaleform;

class GfxSlotBase;

struct SBindingSlot
{
	GfxSlotBase* pSlot;
	std::vector< GFx::Value* > vecBind;

	SBindingSlot( GfxSlotBase* pslot )
		: pSlot( pslot )
	{
	}
};
typedef std::vector< SBindingSlot >		VEC_BIND_SLOT;
typedef VEC_BIND_SLOT::iterator			VEC_BIND_SLOT_ITER;

struct CLIKDataBindingHandlerDesc
{
    GFx::Value  HandlerClosure;
    GFx::Value  BoundObj;
    CLIKDataBindingHandlerDesc() {}
    CLIKDataBindingHandlerDesc(const GFx::Value& handlerClosure, const GFx::Value& boundObj) :
        BoundObj(boundObj), HandlerClosure(handlerClosure) {}
	~CLIKDataBindingHandlerDesc()
	{
		HandlerClosure.SetUndefined();
		BoundObj.SetUndefined();
	}
    bool    operator== (const CLIKDataBindingHandlerDesc& o) const
    {
        return BoundObj == o.BoundObj && HandlerClosure == o.HandlerClosure;
    }
};

class CLIKDataWrapper : public RefCountBase<CLIKDataWrapper, Stat_Default_Mem>
{
public:
    virtual ~CLIKDataWrapper() {}
    virtual void        GetValue(GFx::Movie* pmovie, GFx::Value* value)  = 0;
    virtual bool        SetValue(const GFx::Value& value)   { SF_UNUSED(value); return false; }
    virtual void        OnAddHandler(const char* binding, const CLIKDataBindingHandlerDesc& desc)       
                            { SF_UNUSED2(binding, desc); }
    virtual void        OnRemoveHandler(const char* binding, const CLIKDataBindingHandlerDesc& desc)    
                            { SF_UNUSED2(binding, desc); }
};

class CLIKDataBindingAdapter : public RefCountBase<CLIKDataBindingAdapter, Stat_Default_Mem>
{
public:
    typedef     const GFx::FunctionHandler::Params          ParamsType;

	CLIKDataBindingAdapter(GFx::Movie* pmovie, GFx::ExternalInterface* pxiChain = NULL);

	void            RegisterBoundData(const char* key, Ptr<CLIKDataWrapper> boundData);
	void            UnregisterBoundData(const char* key);

	void            NotifyDataChange(const char* key);

	static void     OnCreateBinding(CLIKDataBindingAdapter* pthis, ParamsType& params);
	static void     OnDeleteBinding(CLIKDataBindingAdapter* pthis, ParamsType& params);

	CLIKDataWrapper*    GetDataWrapper(const char* key) const;
	void GetDataWrapperType( EMGFX_SLOT_TYPE emType, VEC_BIND_SLOT& vecBindSlot );

protected:
	typedef std::vector< CLIKDataBindingHandlerDesc >				DescArray;
	typedef std::vector< CLIKDataBindingHandlerDesc >::iterator		DescArray_iter;

    class Binding : public NewOverrideBase<Stat_Default_Mem>
    {
    public:
        Binding(CLIKDataWrapper* data) : Data(data) {}
        void     AddHandlerDesc(const char* binding, const CLIKDataBindingHandlerDesc& desc);
        void     RemoveHandlerDesc(const char* binding, const CLIKDataBindingHandlerDesc& desc);
        bool     GetValue(GFx::Movie* pmovie, GFx::Value* value);
        bool     SetValue(const GFx::Value& value);
        void     NotifyDataChange(GFx::Movie* pmovie);
        CLIKDataWrapper*    GetDataWrapper() const;

	public :
        Ptr<CLIKDataWrapper>    Data;
        DescArray               HandlerDescriptors;
    };
    Hash<String, Binding, String::HashFunctor>    Bindings;

    GFx::Movie*     pMovie;
};

#endif  // INC_CLIKDataBinding_H
