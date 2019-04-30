// Hash.h: interface for the CList class.
//

#if !defined(HASH_H)
#define HASH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HASHELEMENTS 10007


class CHash
{
protected:
	class CHashItem {
	public:
		CHashItem *child;
		CHashItem *parent;
		char *key;
		char *data;
	};

public:
	CHash();
	virtual ~CHash();

	int Add(char *key, char *data);
	void Remove(char *key);
	char * FindValue(char *key);
	int GetCount();

private:
	unsigned int Hash (char *key);
	CHashItem *m_pHashTable[HASHELEMENTS];
	int m_nCount;
};

#endif // HASH_H
