// Shape.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "Shape.h"
#include "Sphere.h"


// CShape
//IMPLEMENT_SERIAL(CShape, CObject, 2)

CShape::CShape()
{
	m_winx=0;
	m_winz=0;
	m_winy=0;
	m_Select=FALSE;
	m_visible=TRUE;
}

CShape::~CShape()
{
}

BOOL CShape::Intersects(CShape *s) {
	if (this->IsKindOf(RUNTIME_CLASS(CSphere)) && s->IsKindOf(RUNTIME_CLASS(CSphere))) {
		double x1 = this->m_xc;
		double y1 = this->m_yc;
		double z1 = this->m_zc;

		double x2 = s->m_xc;
		double y2 = s->m_yc;
		double z2 = s->m_zc;

		double distance = sqrt( ((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)) + ((z1-z2)*(z1-z2)) );
		
		double radiusSum = ((CSphere*)this)->m_Radius + ((CSphere*)s)->m_Radius;

		if (distance<radiusSum)
			return 1;
		else 
			return 0;
	}

	return  0;
}

