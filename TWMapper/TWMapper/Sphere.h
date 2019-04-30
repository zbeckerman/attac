#pragma once


#include "Shape.h"
// CSphere command target

class CSphere : public CShape
{
public:
	CSphere();
	CSphere(double x,double y,double z,double R,double G,double B,double radius, long name);
	virtual ~CSphere();
//Attributes
public:
	GLUquadricObj* pQuadric;
	//Sphere Properties
	double		m_Radius;
	long m_name;
// Operations
BOOL Equals(CSphere *obj);
public:
// Overrides
	DECLARE_SERIAL(CSphere)
	virtual void Serialize(CArchive& ar);
	virtual void Draw();
	virtual CRect GetRect();
};


