//ListPtr.cpp
#include "stdafx.h"
#include "ListPtr.h"


ListPtr::ListPtr(void)
	: m_ulSize(0)
{
}
ListPtr::~ListPtr(void)
{
	clear();
}

ListPtr::iterator ListPtr::begin(void)
{
	return *m_iterator.m_pNext;
}

ListPtr::iterator ListPtr::end(void)
{
	return m_iterator;
}

void*ListPtr::front(void)
{
	return m_iterator.m_pNext->m_pMember;
}

void ListPtr::pop_front(void)
{
	iterator*piterator=m_iterator.m_pNext;
	if (piterator!=&m_iterator)
	{
		m_iterator.m_pNext=piterator->m_pNext;
		piterator->m_pNext->m_pPrev=&m_iterator;
		delete piterator;
		piterator=NULL;

		m_ulSize--;
	}
}
void ListPtr::push_front(void*ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pBeginiterator=m_iterator.m_pNext;
		m_iterator.m_pNext=piterator;
		pBeginiterator->m_pPrev=piterator;
		piterator->m_pPrev=&m_iterator;
		piterator->m_pNext=pBeginiterator;
		m_ulSize++;
	}
}

void*ListPtr::back(void)
{
	return m_iterator.m_pPrev->m_pMember;
}

void ListPtr::pop_back(void)
{
	iterator*piterator=m_iterator.m_pPrev;
	if (piterator!=&m_iterator)
	{
		m_iterator.m_pPrev=piterator->m_pPrev;
		piterator->m_pPrev->m_pNext=&m_iterator;
		delete piterator;
		piterator=NULL;

		m_ulSize--;
	}
}

void ListPtr::push_back(void*ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pEnditerator=m_iterator.m_pPrev;

		m_iterator.m_pPrev=piterator;
		pEnditerator->m_pNext=piterator;
		piterator->m_pPrev=pEnditerator;
		piterator->m_pNext=&m_iterator;
		m_ulSize++;
	}
}

void ListPtr::insert(ListPtr::iterator titerator,void*ptr)
{
	iterator*piterator=new iterator(ptr);
	if (piterator)
	{
		iterator*pInsertAfteriterator=titerator.m_pPrev;;
		iterator*pInsertBeforeiterator=pInsertAfteriterator->m_pNext;
		pInsertAfteriterator->m_pNext=piterator;
		piterator->m_pPrev=pInsertAfteriterator;
		piterator->m_pNext=pInsertBeforeiterator;
		pInsertBeforeiterator->m_pPrev=piterator;
		m_ulSize++;
	}
}

void ListPtr::erase(ListPtr::iterator titerator)
{
	iterator*pEraseAfteriterator=titerator.m_pPrev;;
	iterator*pEraseiterator=pEraseAfteriterator->m_pNext;
	iterator*pEraseBeforeiterator=pEraseiterator->m_pNext;
	if (pEraseiterator!=&m_iterator)
	{
		pEraseAfteriterator->m_pNext=pEraseBeforeiterator;
		pEraseBeforeiterator->m_pPrev=pEraseAfteriterator;
		delete pEraseiterator;
		pEraseiterator=NULL;

		m_ulSize--;
	}
}

unsigned long ListPtr::size(void)
{
	return m_ulSize;
}

void ListPtr::clear(void)
{
	while (size())
	{
		pop_front();
	}
}

void ListPtr::remove(void*pPtr)
{
	ListPtr::iterator item=find(pPtr);
	erase(item);
}

ListPtr::iterator ListPtr::find(void*pPtr)
{
	ListPtr::iterator item=begin();
	while (item!=end())
	{
		if (item.m_pMember==pPtr)
		{
			break;
		}
		++item;
	}
	return item;
}