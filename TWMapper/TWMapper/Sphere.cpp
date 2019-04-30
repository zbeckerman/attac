// Sphere.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "Sphere.h"
#include "glut.h"	

// CSphere
IMPLEMENT_SERIAL(CSphere, CShape, 2)
//Constructors
CSphere::CSphere()
{
}
CSphere::CSphere(double x,double y,double z,double R,double G,double B,double radius, long name)
{
	m_xc=x;
	m_yc=y;
	m_zc=z;
	m_RedColor=R;
	m_GreenColor=G;
	m_BlueColor=B;
	m_Radius=radius;
	m_name = name;


}
//Destructor
CSphere::~CSphere()
{
}


// CSphere member functions
void CSphere::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_xc<<m_yc<<m_zc<<m_RedColor<<m_GreenColor<<m_BlueColor<<m_name;
		ar <<m_Radius;
		
	}
	else
	{
		ar >> m_xc>>m_yc>>m_zc>>m_RedColor>>m_GreenColor>>m_BlueColor>>m_name;
	    ar >>m_Radius;
	}
}

/*
GLUT_BITMAP_8_BY_13
GLUT_BITMAP_9_BY_15
GLUT_BITMAP_TIMES_ROMAN_10
GLUT_BITMAP_TIMES_ROMAN_24
GLUT_BITMAP_HELVETICA_10
GLUT_BITMAP_HELVETICA_12
GLUT_BITMAP_HELVETICA_18
*/

void CSphere::Draw()
{
	char buffer[10];
	int i=0;
	pQuadric = gluNewQuadric();

	glPushMatrix();
	if(m_Select==TRUE)
	{
		glColor3d(1.0,1.0,0);
		glRectd(m_xc-m_Radius,m_yc-m_Radius,m_xc+m_Radius,m_yc+m_Radius);
	}
	m_rect.SetRect((int)m_winx-10,(int)m_winy-10,(int)m_winx+10,(int)m_winy+10);
	glTranslated(m_xc,m_yc,m_zc);
	glColor3d(m_RedColor,m_GreenColor,m_BlueColor);
	gluSphere (pQuadric,m_Radius,12,12);
	glPopMatrix();

	glPushMatrix();
    glColor3ub(255, 255, 0);
	glRasterPos3f(m_xc, m_yc+m_Radius, m_zc);
	sprintf (buffer, "%ld", m_name);
	for (i=0; i<(int)strlen(buffer); i++) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
	}
	glPopMatrix();

}
CRect CSphere::GetRect()
{
	return m_rect;
}

BOOL CSphere::Equals(CSphere *obj) {
    //Check for null and compare run-time types.
    if (obj == NULL) 
		return false;
    CSphere *sphere = (CSphere*) obj;

	return  (m_name == sphere->m_name);
}
