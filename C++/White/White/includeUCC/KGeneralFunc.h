#pragma once

//////////////////////////////////////////////////////////////////////////
// 基本功能函数

//-------------------------------------
// 安全删除指针宏
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
// 安全删除指针函数模板
//-------------------------------------
template < typename T>
void SafeDelPointer( T*& p)
{
    SAFE_DELETE(p);
}

//-------------------------------------
// 安全删除map的second（其类型为指针）
//-------------------------------------
template < typename P1, 
typename P2>
void SafeDelMapSecond( typename map<P1, P2*>::value_type& p)
{
    SAFE_DELETE(p.second);
}

//-----------------------------------------------------------------------------
// 针对 CMDPacket 的不同参数类型（指针/引用），特化模板
// 类模板才可以对 non-type 模板形参进行偏特化，所以选择使用仿函数的形式实现
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
// 通用 KCmdPacket::SetItem，多个子项，针对指针类型的打包
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
// 比较指针所指数据是否相等
//-------------------------------------
template <typename T>
struct TComparePointer: public binary_function<T*, T*, bool>
{
    bool operator () ( T*const & lhs, T*const & rhs) const
    {
        return (*lhs == *rhs);
    }
};

