//ListPtr.h
#ifndef __LIST_PTR_H__
#define __LIST_PTR_H__


class ListPtr
{
public:
	ListPtr(void);
	virtual~ListPtr(void);
public:
	class iterator
	{
	public:
		iterator(void*pMember=NULL)
		{
			m_pMember=pMember;
			m_pNext=this;
			m_pPrev=this;
		}
		virtual~iterator(void)
		{
		}
	public:
		void operator ++()
		{
			*this=*m_pNext;
		}
		void operator --()
		{
			*this=*m_pPrev;
		}
		void* operator *()
		{
			return m_pMember;
		}
		iterator &operator =(iterator titerator)
		{
			m_pMember=titerator.m_pMember;
			m_pNext=titerator.m_pNext;
			m_pPrev=titerator.m_pPrev;
			return (*this);
		}
		bool operator ==(iterator titerator)
		{
			return (m_pMember==titerator.m_pMember &&
					m_pNext==titerator.m_pNext &&
					m_pPrev==titerator.m_pPrev);
		}
		bool operator !=(iterator titerator)
		{
			return (m_pMember!=titerator.m_pMember ||
					m_pNext!=titerator.m_pNext ||
					m_pPrev!=titerator.m_pPrev);
		}
	protected:
		void*		m_pMember;
		iterator*	m_pNext;
		iterator*	m_pPrev;

		friend class ListPtr;
	};
public:
	ListPtr::iterator begin(void);
	ListPtr::iterator end(void);
	void*front(void);
	void pop_front(void);
	void push_front(void*ptr);
	void*back(void);
	void pop_back(void);
	void push_back(void*ptr);
	void insert(ListPtr::iterator titerator,void*ptr);
	void erase(ListPtr::iterator titerator);
	unsigned long size(void);
	void clear(void);
	void remove(void*pPtr);
	ListPtr::iterator find(void*pPtr);
protected:
	iterator		m_iterator;
	unsigned long	m_ulSize;
};


#endif