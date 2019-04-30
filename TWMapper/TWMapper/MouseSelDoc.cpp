// MouseSelDoc.cpp : implementation of the CMouseSelDoc class
//

#include "stdafx.h"
#include "Util.h"
#include "Sector.h"
#include "Stardock.h"
#include "MouseSel.h"
#include "Shape.h"

#include "MouseSelDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMouseSelDoc

IMPLEMENT_DYNCREATE(CMouseSelDoc, CDocument)

BEGIN_MESSAGE_MAP(CMouseSelDoc, CDocument)
END_MESSAGE_MAP()


// CMouseSelDoc construction/destruction

CMouseSelDoc::CMouseSelDoc()
{
	// TODO: add one-time construction code here

}

CMouseSelDoc::~CMouseSelDoc()
{
}

BOOL CMouseSelDoc::isInShapeList (CSphere *obj) {

	for (int j=0; j<m_ShapeList.GetCount(); j++) {
		CShape *s = (CShape*) m_ShapeList.GetAt(m_ShapeList.FindIndex(j));
		if (s->IsKindOf(RUNTIME_CLASS(CSphere))) {
			CSphere *objFromList = (CSphere*) s;
			if (objFromList->Equals(obj)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CMouseSelDoc::isInShapeList (CLine *obj) {

	for (int j=0; j<m_ShapeList.GetCount(); j++) {
		CLine *s = (CLine*) m_ShapeList.GetAt(m_ShapeList.FindIndex(j));
		if (s->IsKindOf(RUNTIME_CLASS(CLine))) {
			CLine *objFromList = (CLine*) s;
			if (objFromList->Equals(obj)) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

CSphere* CMouseSelDoc::getSphereFromList (long name) {

	for (int j=0; j<m_ShapeList.GetCount(); j++) {
		CShape *s = (CShape*) m_ShapeList.GetAt(m_ShapeList.FindIndex(j));
		if (s->IsKindOf(RUNTIME_CLASS(CSphere))) {
			CSphere *objFromList = (CSphere*) s;
			if (objFromList->m_name == name) {
				return objFromList;
			}
		}
	}
	return NULL;
}

BOOL CMouseSelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

		srand( (unsigned)time( NULL ) );

		InitDoc();
		m_count=5;
		double currentX=0;
		double currentY=0;
		double currentZ=0;
		double lastX=0;
		double lastY=0;
		double lastZ=0;

//CSphere* m_sphere1 = new CSphere(x-20,y+3,z,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, 100);
//CSphere* m_sphere2 = new CSphere(x-20,y,z,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, 200);
//BOOL x2 = m_sphere1->Intersects(m_sphere2);

//		CStardock* stardock = new CStardock(currentX,currentY,currentZ,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, 100);
//		m_ShapeList.AddTail(stardock);

//		return TRUE;

		//add the axis
		CAxis* m_axis = new CAxis(currentX,currentY, currentZ,1, m_RedColor,m_GreenColor,m_BlueColor);
		currentX+=3;	//make sure we dont overlap axis

		m_ShapeList.AddTail(m_axis);

		CUtil *cUtil = new CUtil();

		CObList *sectorList = cUtil->loadMapFromSector(1, 5);
	
		delete cUtil;
		//get sphere list
		for( int i=0;i<sectorList->GetCount(); i++)
		{
			CSector* sector = (CSector*) sectorList->GetAt(sectorList->FindIndex(i));
CSphere* m_sphereSector =NULL;

			if (sector->stardock)
				m_sphereSector = new CStardock(currentX,currentY,currentZ,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, sector->sectorNumber);
			else
				m_sphereSector = new CSphere(currentX,currentY,currentZ,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, sector->sectorNumber);

			BOOL alreadyInList = isInShapeList(m_sphereSector);
			if (alreadyInList) {
				m_sphereSector = getSphereFromList(sector->sectorNumber);
				currentX = m_sphereSector->m_xc;
				currentY = m_sphereSector->m_yc;
				currentZ = m_sphereSector->m_zc;
			} else {
				m_ShapeList.AddTail(m_sphereSector);
			}

			for (int j=0; j<sector->sectorCount; j++) {
getAnotherPlacementThatDoesntIntersect:
				int xNegPos = ((rand()>RAND_MAX/2)?1:-1);
				int yNegPos = ((rand()>RAND_MAX/2)?1:-1);
				int zNegPos = ((rand()>RAND_MAX/2)?1:-1);
				
				double randX = rand();
				double randY = rand();
				double randZ = rand();

				double posX = currentX+(randX/(double)RAND_MAX)*10*xNegPos;
				double posY = currentY+(randY/(double)RAND_MAX)*10*yNegPos;
				double posZ = currentZ+(randZ/(double)RAND_MAX)*10*zNegPos;

				CSphere* m_sphereWarp = new CSphere(posX,posY,posZ,m_RedColor,m_GreenColor, m_BlueColor,m_dRadius, sector->warps[j]);

				BOOL alreadyInList = isInShapeList(m_sphereWarp);
				if (alreadyInList) {
					m_sphereWarp = getSphereFromList(sector->sectorNumber);
					posX = m_sphereWarp->m_xc;
					posY = m_sphereWarp->m_yc;
					posZ = m_sphereWarp->m_zc;
				} else {

					for (int j=0; j<m_ShapeList.GetCount(); j++) {
						CShape *s = (CShape*) m_ShapeList.GetAt(m_ShapeList.FindIndex(j));
						BOOL x = m_sphereWarp->Intersects(s);
						if (x != 0) {
							goto getAnotherPlacementThatDoesntIntersect;
						}
					}

					m_ShapeList.AddTail(m_sphereWarp);
				}

				CLine * m_line = new CLine (currentX, currentY, currentZ, posX, posY, posZ, 1, 1, 1, m_RedColor, m_GreenColor, m_BlueColor, true, true);
				if (!isInShapeList(m_line)) {
					m_ShapeList.AddTail(m_line);
				}
			}

			lastX = currentX;
		}

	return TRUE;
}




// CMouseSelDoc serialization

void CMouseSelDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	ar<<m_SpeedTranslation;
	ar<<m_SpeedRotation;
	ar<<m_xRotation;
	ar<<m_yRotation;
	ar<<m_zRotation;
	ar<<m_xyRotation;
	ar<<m_xTranslation;
	ar<<m_yTranslation;
	ar<<m_zTranslation;
	}
	else
	{

	ar>>m_SpeedTranslation;
	ar>>m_SpeedRotation;
	ar>>m_xRotation;
	ar>>m_yRotation;
	ar>>m_zRotation;
	ar>> m_xyRotation;
	ar>>m_xTranslation;
	ar>>m_yTranslation;
	ar>>m_zTranslation;


	}
	m_ShapeList.Serialize(ar);
}


// CMouseSelDoc diagnostics

#ifdef _DEBUG
void CMouseSelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMouseSelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
void CMouseSelDoc::DeleteContents()
{
	while (!m_ShapeList.IsEmpty())
	{
		delete m_ShapeList.RemoveHead();
	}
	CDocument::DeleteContents();
}

BOOL CMouseSelDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  Add your specialized creation code here

	return TRUE;
}
void CMouseSelDoc::RenderScene()
{
	POSITION pos = m_ShapeList.GetHeadPosition();
	while (pos != NULL)
	{
		CShape * shape = (CShape *)m_ShapeList.GetNext(pos);
		if (shape->m_visible) {
			glGetIntegerv(GL_VIEWPORT, shape->m_viewport);
			glGetDoublev(GL_MODELVIEW_MATRIX, shape->m_modelMatrix);
			glGetDoublev(GL_PROJECTION_MATRIX, shape->m_projMatrix);
			gluProject(shape->m_xc,shape->m_yc,shape->m_zc,shape->m_modelMatrix, 
						shape->m_projMatrix,shape->m_viewport,
						&shape->m_winx,&m_y,&shape->m_winz);
			shape->m_winy=shape->m_viewport[3] - (GLint) m_y - 1;
			shape->Draw(); 
		}
	}
}

void CMouseSelDoc::InitDoc()
{
	m_RedColor=0.0;
	m_GreenColor=0.0;
	m_BlueColor=1.0;
	m_dRadius=1.0;
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_zRotation = 0.0f;

	m_xTranslation = 0.0f;
	m_yTranslation = 0.0f;
	m_zTranslation = -40.0f;

	m_SpeedRotation = 1.0f / 3.0f;
	m_SpeedTranslation = 1.0f / 50.0f;

	m_xyRotation = TRUE;

}

