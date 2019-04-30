#pragma once


#include "Sphere.h"
// CStardock command target

class CStardock : public CSphere
{
public:
	CStardock();
	CStardock(double x,double y,double z,double R,double G,double B,double radius, long name);
	virtual ~CStardock();
//Attributes
public:
// Operations
BOOL Equals(CStardock *obj);
public:
// Overrides
	DECLARE_SERIAL(CStardock)
	virtual void Serialize(CArchive& ar);
	virtual void Draw();
	virtual CRect GetRect();
};


