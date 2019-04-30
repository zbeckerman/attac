// Line.cpp : implementation file
//

#include "stdafx.h"
#include "Queue.h"


//Constructors
CQueue::CQueue()
{
}

//Destructor
CQueue::~CQueue()
{
}

void CQueue::push (CSector* sector) {
	m_QueueList.InsertAfter(m_QueueList.GetTailPosition(), (CObject*) sector);
}



CSector* CQueue::pop (void) {
	CSector *sector = (CSector *) m_QueueList.GetAt(m_QueueList.GetHeadPosition());
	m_QueueList.RemoveAt(m_QueueList.GetHeadPosition());
	return sector;
}

BOOL CQueue::isQueueEmpty () {
	return m_QueueList.IsEmpty();
}