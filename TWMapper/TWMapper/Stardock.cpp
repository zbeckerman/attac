// Sphere.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "Stardock.h"
#include "glut.h"	
#include "Sphere.h"

// CStardock
IMPLEMENT_SERIAL(CStardock, CSphere, 2)
//Constructors
CStardock::CStardock()
{
}
CStardock::CStardock(double x,double y,double z,double R,double G,double B,double radius, long name)
{
	m_xc=x;
	m_yc=y;
	m_zc=z;
	m_RedColor=R;
	m_GreenColor=G;
	m_BlueColor=B;
	m_Radius=radius*10;
	m_name = name;


}
//Destructor
CStardock::~CStardock()
{
}


// CStardock member functions
void CStardock::Serialize(CArchive& ar)
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

void CStardock::Draw()
{
	char buffer[30];
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
	gluDisk (pQuadric,0, 3,12,12);
	gluCylinder (pQuadric,3, 3,2,12,12);
	glTranslatef(0.0f,0.0f,2.0f);			// Center The Cylinder
	gluDisk (pQuadric,0, 3,12,12);
	gluCylinder (pQuadric,2, 2,3,12,12);
	glTranslatef(0.0f,0.0f,5.0f);			// Center The Cylinder
	gluSphere (pQuadric,3,12,12);

	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.0f,-10.0f,.0f);			// Center The Cylinder
    glColor3ub(255, 255, 0);
	glRasterPos3f(m_xc, m_yc+m_Radius, m_zc);
	sprintf (buffer, "[STARDOCK] %ld", m_name);
	for (i=0; i<(int)strlen(buffer); i++) {
	    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
	}
	glPopMatrix();

}
CRect CStardock::GetRect()
{
	return m_rect;
}

BOOL CStardock::Equals(CStardock *obj) {
    //Check for null and compare run-time types.
    if (obj == NULL) 
		return false;
    CStardock *sphere = (CStardock*) obj;

	return  (m_name == sphere->m_name);
}
