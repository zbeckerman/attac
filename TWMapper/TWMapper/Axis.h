#pragma once


#include "Shape.h"
// CAxis command target

class CAxis : public CShape
{
public:
	CAxis();
	CAxis(double x, double y, double z,double height,double R,double G,double B);
	virtual ~CAxis();
//Attributes
public:
	GLUquadricObj* pQuadric;
	//Sphere Properties
	double		m_RedColor,m_GreenColor,m_BlueColor;
	double		m_height;

// Operations
public:
// Overrides
	DECLARE_SERIAL(CAxis)
	virtual void Serialize(CArchive& ar);
	virtual void Draw();
	virtual CRect GetRect();
};


