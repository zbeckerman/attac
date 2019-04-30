#pragma once


#include "Queue.h"
#include "Sector.h"
// CQueue command target

typedef CTypedPtrList<CObList, CSector*> CQueueList;

class CQueue  : public CObject
{
public:
	CQueue();
	virtual ~CQueue();
//Attributes
public:
    CQueueList m_QueueList; 

// Operations
void push (CSector *);
CSector* pop ();
BOOL isQueueEmpty ();

public:
};


