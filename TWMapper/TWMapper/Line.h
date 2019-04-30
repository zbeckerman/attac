#pragma once


#include "Shape.h"
// CLine command target

class CLine : public CShape
{
public:
	CLine();
	CLine(double x1, double y1, double z1, double x2, double y2, double z2, double thickness, int stipplePattern, int stippleFactor, double R,double G,double B,bool frontArrow, bool backArrow);
	virtual ~CLine();
//Attributes
public:
	GLUquadricObj* pQuadric;
	//Sphere Properties
	double		m_RedColor,m_GreenColor,m_BlueColor;
	bool		m_FrontArrow;
	bool		m_BackArrow;
	double		m_thickness;
	int			m_stipplePattern;
	int			m_stippleFactor;
	double		m_x1,m_y1,m_z1;
	double		m_x2,m_y2,m_z2;

// Operations
BOOL CLine::Equals(CLine *obj);


public:
// Overrides
	DECLARE_SERIAL(CLine)
	virtual void Serialize(CArchive& ar);
	virtual void Draw();
	virtual CRect GetRect();
};


