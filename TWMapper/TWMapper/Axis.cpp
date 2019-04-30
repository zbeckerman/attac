// Line.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "Axis.h"
#include "glut.h"	


// CAxis
IMPLEMENT_SERIAL(CAxis, CShape, 2)
//Constructors
CAxis::CAxis()
{
}
CAxis::CAxis(double x, double y, double z, double height,double R,double G,double B)
{
	m_xc=x;
	m_yc=y;
	m_zc=z;

	m_height=height;

	m_RedColor=R;
	m_GreenColor=G;
	m_BlueColor=B;

}
//Destructor
CAxis::~CAxis()
{
}


// CAxis member functions
void CAxis::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_xc<<m_yc<<m_zc<<m_height<<m_GreenColor<<m_BlueColor;
		
	}
	else
	{
		ar >> m_xc>>m_yc>>m_zc>>m_height>>m_GreenColor>>m_BlueColor;
	}
	
}

void CAxis::Draw()
{
	glPushMatrix();
    glColor3ub(255, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(m_xc, m_yc, m_zc);
    glVertex3f(m_height, 0.0, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, -0.25, 0.0);
    glVertex3f(m_height, 0.0, 0.0);
    glVertex3f(0.75, 0.0, 0.25);
    glVertex3f(0.75, 0.0, -0.25);
    glVertex3f(m_height, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(m_xc, m_yc, m_zc);
    glVertex3f(0.0, m_height, 0.0);
    glVertex3f(0.0, 0.75, 0.25);
    glVertex3f(0.0, 0.75, -0.25);
    glVertex3f(0.0, m_height, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glVertex3f(-0.25, 0.75, 0.0);
    glVertex3f(0.0, m_height, 0.0);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glVertex3f(m_xc, m_yc, m_zc);
    glVertex3f(0.0, 0.0, m_height);
    glVertex3f(0.25, 0.0, 0.75);
    glVertex3f(-0.25, 0.0, 0.75);
    glVertex3f(0.0, 0.0, m_height);
    glVertex3f(0.0, 0.25, 0.75);
    glVertex3f(0.0, -0.25, 0.75);
    glVertex3f(0.0, 0.0, m_height);
    glEnd();
  
    glColor3ub(255, 255, 0);
    glRasterPos3f(m_height+.2, 0.0, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
    glRasterPos3f(0.0, m_height+.2, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
    glRasterPos3f(0.0, 0.0, m_height+.2);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
	
	glPopMatrix();

}
CRect CAxis::GetRect()
{
	return m_rect;
}