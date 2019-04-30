// List.cpp: implementation of the CList class.
//

#include "stdafx.h"
#include "TWXSyncServer.h"
#include "List.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CList::CList()
{
	m_pListHead=NULL;
	m_nCount=0;
}

CList::~CList()
{
}

//////////////////////////////////////////////////////////////////////

int CList::Add(void *ptr)
{
	CListItem *pItem;
	CListItem *pItem2;

	pItem=new CListItem();
	pItem->ptr=ptr;
	pItem->next=NULL;

	if (m_pListHead == NULL) {
		m_pListHead=pItem;
	}
	else {
		pItem2=m_pListHead;
		while (pItem2->next != NULL) {
			pItem2=pItem2->next;
		}
		pItem2->next=pItem;
	}

	return m_nCount++;
}

void CList::Remove(void *ptr)
{
	CListItem *pItem;
	CListItem *pItemOld;

	if (m_pListHead == NULL) return;
	pItem=m_pListHead;
	pItemOld=NULL;
	while (pItem != NULL) {
		if (pItem->ptr == ptr) {
			if (pItemOld == NULL) {
				m_pListHead=pItem->next;
			}
			else {
				pItemOld->next=pItem->next;
			}
			delete pItem;
			m_nCount--;
			break;
		}
		pItemOld=pItem;
		pItem=pItem->next;
	}
}

int CList::GetCount()
{
	return m_nCount;
}

void *CList::GetAt(int nIndex)
{
	CListItem *pItem;

	pItem=m_pListHead;
	while (pItem != NULL) {
		if (nIndex == 0) {
			return pItem->ptr;
		}
		pItem=pItem->next;
		nIndex--;
	}
	return NULL;
}
