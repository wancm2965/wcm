#include "StdAfx.h"
#include "Node.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

CNode::CNode()
: m_parent(NULL)
{}

CNode::CNode(NodeData t, CNode* parent)
: m_NodeData (t)
, m_parent (parent)
{}

CNode::~CNode() 
{
	for(int i = 0; i < NumChildren(); ++i)
	{
		delete vecChildren[i]; 
	}
}

void CNode::SetParent(CNode* parent)
{
	m_parent = parent;
}

CNode* CNode::GetParent()
{
	if (NULL == m_parent)
		return this;

	return m_parent;
}

NodeData& CNode::GetData()
{
	return m_NodeData;
}

CNode* CNode::GetChild(int i)
{
	return vecChildren[i];
}

bool CNode::HasChildren() const
{
	return NumChildren() > 0;
}

bool CNode::IsFolder() const
{
	return m_NodeData.bFolder;
}

int CNode::NumChildren() const
{
	return vecChildren.size();
}

void CNode::AddChild(CNode* child)
{
	if(NULL == child)
		return;

	vecChildren.push_back(child); 
	child->SetParent(this); 
}

void CNode::RemoveChild(CNode* child)
{
	Children::iterator iter = vecChildren.begin();
	for (; iter < vecChildren.end(); ++iter)
	{
		if (*iter == child) 
		{
			vecChildren.erase(iter);
			return;
		}
	}

	MEMBER_NOTEDATA_MAP_IT itMap = m_mapMemberNoteData.begin();
	for (; itMap != m_mapMemberNoteData.end(); itMap++)
	{
		if (itMap->second == child) 
		{
			delete itMap->second;
			itMap->second = NULL;
			m_mapMemberNoteData.erase(itMap);
		}
	}
}

void CNode::AddChild(CStdString strID, CNode* child)
{
	MEMBER_NOTEDATA_MAP_IT itMap = m_mapMemberNoteData.find(strID);
	if(itMap != m_mapMemberNoteData.end())
		m_mapMemberNoteData.erase(itMap);

	m_mapMemberNoteData[strID] = child;
}

CNode* CNode::GetNode(CStdString MemberID)
{
	MEMBER_NOTEDATA_MAP_IT itMap = m_mapMemberNoteData.find(MemberID);
	if(itMap == m_mapMemberNoteData.end())
		return this;

	return itMap->second;
}

CNode* CNode::GetLastChild()
{
	if (HasChildren())
	{
		return vecChildren[NumChildren() - 1]->GetLastChild();
	}
	return this;
}


void CNode::SetNodeIndex( CNode* child, int nIndex )
{
	if(nIndex >= (int)vecChildren.size())
		return;

	int nCurIndex = 0;
	Children::iterator it = vecChildren.begin();
	for (; it < vecChildren.end(); it++)
	{
		if (*it == child)
		{
			if(nCurIndex == nIndex)
			{
				return;
			}

			vecChildren.erase(it);
			if(nIndex == (int)vecChildren.size())
			{
				vecChildren.push_back(child);
				return;
			}

			it = vecChildren.begin();
			for (int i = 0; it < vecChildren.end(); it++)
			{
				if (i == nIndex)
				{
					vecChildren.insert(it, child);
					return;
				}
				i++;
			}
		}
		nCurIndex++;
	}
}

int CNode::GetChildIndex( CNode* pChild )
{
	int i = 0;
	Children::iterator it = vecChildren.begin();
	for (; it < vecChildren.end(); it++)
	{
		if (*it == pChild)
		{
			return i;
		}
		i++;
	}

	return -1;
}