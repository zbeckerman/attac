// List.h: interface for the CList class.
//

#if !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_)
#define AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CList
{
protected:
	class CListItem {
	public:
		CListItem *next;
		void *ptr;
	};

public:
	CList();
	virtual ~CList();

	int Add(void *ptr);
	void Remove(void *ptr);
	int GetCount();
	void *GetAt(int nIndex);

private:
	CListItem *m_pListHead;
	int m_nCount;
};

#endif // !defined(AFX_LIST_H__66142D85_C322_428E_A76D_EB38C9EE2028__INCLUDED_)
