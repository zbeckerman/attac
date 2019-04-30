// Line.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "Line.h"
#include "glut.h"	


// CLine
IMPLEMENT_SERIAL(CLine, CShape, 2)
//Constructors
CLine::CLine()
{
}
CLine::CLine(double x1, double y1, double z1, double x2, double y2, double z2, double thickness, int stipplePattern, int stippleFactor, double R,double G,double B,bool frontArrow, bool backArrow)
{
	m_x1=x1;
	m_y1=y1;
	m_z1=z1;
	m_x2=x2;
	m_y2=y2;
	m_z2=z2;

	m_thickness=thickness;
	m_stipplePattern=stipplePattern;
	m_stippleFactor=stippleFactor;

	m_RedColor=R;
	m_GreenColor=G;
	m_BlueColor=B;
	m_BackArrow = backArrow;
	m_FrontArrow = frontArrow;

}
//Destructor
CLine::~CLine()
{
}


// CLine member functions
void CLine::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar <<m_x1<<m_y1<<m_z1<<m_x2<<m_y2<<m_z2<<m_stippleFactor<<m_stipplePattern<<m_thickness<<m_BackArrow<<m_FrontArrow<<m_GreenColor<<m_BlueColor;
		
	}
	else
	{
		ar >>m_x1>>m_y1>>m_z1>>m_x2>>m_y2>>m_z2>>m_stippleFactor>>m_stipplePattern>>m_thickness>>m_BackArrow>>m_FrontArrow>>m_GreenColor>>m_BlueColor;
	}
	
}

void CLine::Draw()
{
	glPushMatrix();
	GLfloat fSizes[2];      // Line width range metrics
	GLfloat fCurrSize;      // Save current size

	// Get line size metrics and save the smallest value
	glGetFloatv(GL_LINE_WIDTH_RANGE,fSizes);
	fCurrSize = fSizes[0];

	// Increase the line width
	fCurrSize += 2.0f;

	glLineWidth(fCurrSize);

/*

	glEnable(GL_LINE_STIPPLE);

	glLineStipple(m_stippleFactor,m_stipplePattern);
*/
    glColor3ub(100, 100, 100);

	glBegin(GL_LINES);
		glVertex3f(m_x1, m_y1, m_z1);
		glVertex3f(m_x2, m_y2, m_z2);
	glEnd();

	/*
    glColor3ub(255, 0, 0);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, -0.25, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(0.75, 0.0, 0.25);
    glVertex3f(0.75, 0.0, -0.25);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.75, 0.25);
    glVertex3f(0.0, 0.75, -0.25);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glVertex3f(-0.25, 0.75, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.25, 0.0, 0.75);
    glVertex3f(-0.25, 0.0, 0.75);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.25, 0.75);
    glVertex3f(0.0, -0.25, 0.75);
    glVertex3f(0.0, 0.0, 1.0);
    glEnd();
    
    glColor3ub(255, 255, 0);
    glRasterPos3f(1.1, 0.0, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'x');
    glRasterPos3f(0.0, 1.1, 0.0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'y');
    glRasterPos3f(0.0, 0.0, 1.1);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'z');
	*/
	
	glPopMatrix();

}
CRect CLine::GetRect()
{
	return m_rect;
}

BOOL CLine::Equals(CLine *obj) {
    //Check for null and compare run-time types.
    if (obj == NULL) 
		return false;
    CLine *line = (CLine*) obj;

	return  (m_x1 == line->m_x1) && 
			(m_y1 == line->m_y1) && 
			(m_z1 == line->m_z1) && 
			(m_x2 == line->m_x2) && 
			(m_y2 == line->m_y2) && 
			(m_z2 == line->m_z2);
}
