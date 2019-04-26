#pragma once

//////////////////////////////////////////////////////////////////////////
// �������ܺ���

//-------------------------------------
// ��ȫɾ��ָ���
//-------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(x)    \
    if(x != NULL)         \
    {                     \
    delete x;         \
    x = NULL;         \
    }
#endif


//-------------------------------------
// ��ȫɾ��ָ�뺯��ģ��
//-------------------------------------
template < typename T>
void SafeDelPointer( T*& p)
{
    SAFE_DELETE(p);
}

//-------------------------------------
// ��ȫɾ��map��second��������Ϊָ�룩
//-------------------------------------
template < typename P1, 
typename P2>
void SafeDelMapSecond( typename map<P1, P2*>::value_type& p)
{
    SAFE_DELETE(p.second);
}

//-----------------------------------------------------------------------------
// ��� CMDPacket �Ĳ�ͬ�������ͣ�ָ��/���ã����ػ�ģ��
// ��ģ��ſ��Զ� non-type ģ���βν���ƫ�ػ�������ѡ��ʹ�÷º�������ʽʵ��
//-----------------------------------------------------------------------------
template< typename T, bool bPointer>
struct TPackItem;

template<typename T>
struct TPackItem<T, true>
{
	void operator () ( KCmdItem& item, T* pData)
	{
		CMDPacket( item, pData);
	}
};

template<typename T>
struct TPackItem<T, false>
{
	void operator () ( KCmdItem& item, T* pData)
	{
		CMDPacket( item, *pData);
	}
};

//-----------------------------------------------------------------------------
// ͨ�� KCmdPacket::SetItem�����������ָ�����͵Ĵ��
//-----------------------------------------------------------------------------
template<typename T, bool bPointer>
struct TPushItem : public unary_function<T*, void>
{
    TPushItem(KCmdPacket& r):_tPacket(r){}

    void operator () ( T* pData)
    {
		KCmdItem item;

		TPackItem<T, bPointer>()( item, pData);

        _tPacket.SetItem( item.GetString() );
    }

private:
    KCmdPacket& _tPacket;
};


//////////////////////////////////////////////////////////////////////////
// 

//-------------------------------------
// �Ƚ�ָ����ָ�����Ƿ����
//-------------------------------------
template <typename T>
struct TComparePointer: public binary_function<T*, T*, bool>
{
    bool operator () ( T*const & lhs, T*const & rhs) const
    {
        return (*lhs == *rhs);
    }
};

