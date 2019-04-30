// MouseSelView.cpp : implementation of the CMouseSelView class
//

#include "stdafx.h"
#include "MouseSel.h"

#include "MouseSelDoc.h"
#include "MouseSelView.h"
#include "Sphere.h"
#include "Stardock.h"
#include "SpherePropDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMouseSelView

IMPLEMENT_DYNCREATE(CMouseSelView, CView)

BEGIN_MESSAGE_MAP(CMouseSelView, CView)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnEditProperties)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_COMMAND(ID_VIEW_SECTORS, OnViewSectors)
	ON_COMMAND(ID_VIEW_WARPLINES, OnViewWarplines)
	ON_COMMAND(ID_VIEW_STARDOCK, OnViewStardock)
END_MESSAGE_MAP()


// CMouseSelView construction/destruction

CMouseSelView::CMouseSelView()
{
	// OpenGL
	m_hGLContext = NULL;
	m_GLPixelIndex = 0;
	
	// Mouse
	m_LeftButtonDown = FALSE;
	m_RightButtonDown = FALSE;
	m_sph=NULL;

}

CMouseSelView::~CMouseSelView()
{
}

BOOL CMouseSelView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);
}

// CMouseSelView drawing

void CMouseSelView::OnDraw(CDC* /*pDC*/)
{
	CMouseSelDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


// CMouseSelView diagnostics

#ifdef _DEBUG
void CMouseSelView::AssertValid() const
{
	CView::AssertValid();
}

void CMouseSelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMouseSelDoc* CMouseSelView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMouseSelDoc)));
	return (CMouseSelDoc*)m_pDocument;
}
#endif //_DEBUG


// CMouseSelView message handlers

void CMouseSelView::OnEditProperties()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	if(m_sph!=NULL)
	{
	CSpherePropDlg m_propDlg;	
	m_propDlg.m_Red=(int)m_sph->m_RedColor*255;
	m_propDlg.m_Green=(int)m_sph->m_GreenColor*255;
	m_propDlg.m_Blue=(int)m_sph->m_BlueColor*255;
	if (m_sph->IsKindOf( RUNTIME_CLASS( CSphere )))
		m_propDlg.m_dRadius=(( CSphere*)m_sph)->m_Radius;
	
	if(m_propDlg.DoModal()==IDOK)
	{
		m_sph->m_RedColor=(double) m_propDlg.m_Red/255.0;
		m_sph->m_GreenColor= (double)m_propDlg.m_Green/255.0;
		m_sph->m_BlueColor= (double)m_propDlg.m_Blue/255.0;
		if (m_sph->IsKindOf( RUNTIME_CLASS( CSphere )))
			(( CSphere*)m_sph)->m_Radius=m_propDlg.m_dRadius;
		Invalidate();
		pDoc->SetModifiedFlag(TRUE);
	}
		
		
	}
	else
	{
		AfxMessageBox("Select sphere!",MB_OK);
	}
}

void CMouseSelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_LeftButtonDown = TRUE;
	m_LeftDownPos = point;
	SetCapture();
	if(m_sph!=NULL)
	m_sph->m_Select=FALSE;
	m_sph=NULL;
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	ASSERT(pDoc);
    CTypedPtrList<CObList,CShape*>& shapeList =pDoc->m_ShapeList;
	POSITION pos = shapeList.GetHeadPosition();
	while (pos != NULL)
	{
       CShape* shape = shapeList.GetNext(pos);
		shape->m_Select=FALSE;
		if(shape->GetRect().PtInRect(point))
		{
			m_sph=shape;
			m_sph->m_Select=TRUE;
			break;
		}
	}
		
	Invalidate();
	CView::OnLButtonDown(nFlags, point);
}

void CMouseSelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

void CMouseSelView::OnMouseMove(UINT nFlags, CPoint point)
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	// Both : rotation
	if(m_LeftButtonDown && m_RightButtonDown)
	{
		if(m_xyRotation)
		{
			m_yRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		else
		{
			m_zRotation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedRotation;
			m_xRotation -= (float)(m_LeftDownPos.y - point.y) * m_SpeedRotation;
		}
		m_LeftDownPos = point;
		m_RightDownPos = point;
	Invalidate();
		
	}

	else

	// Left : x / y translation
	if(m_LeftButtonDown)
	{
		m_xTranslation -= (float)(m_LeftDownPos.x - point.x) * m_SpeedTranslation;
		m_yTranslation += (float)(m_LeftDownPos.y - point.y) * m_SpeedTranslation;
		m_LeftDownPos = point;
		Invalidate();
	}

	else

	// Right : z translation
	if(m_RightButtonDown)
	{
		m_zTranslation += (float)(m_RightDownPos.y - point.y) * m_SpeedTranslation;
		m_RightDownPos = point;
		Invalidate();
	}

	CView::OnMouseMove(nFlags, point);
}

void CMouseSelView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	m_xyRotation=m_xyRotation ? FALSE : TRUE;
	
	CView::OnRButtonDblClk(nFlags, point);
}

void CMouseSelView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_RightButtonDown = TRUE;
	m_RightDownPos = point;
	SetCapture();

	CView::OnRButtonDown(nFlags, point);
}

void CMouseSelView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_RightButtonDown = FALSE;
	m_LeftButtonDown = FALSE;
	ReleaseCapture();
	CView::OnRButtonUp(nFlags, point);
}

int CMouseSelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);

	if(SetWindowPixelFormat(hDC)==FALSE)
		return 0;
	
	if(CreateViewGLContext(hDC)==FALSE)
		return 0;

	
    glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	
	glClearColor(0,0,0,1.0f);
    glClearDepth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	// Perspective
	CRect rect;
	GetClientRect(&rect);
	double	aspect = (rect.Height() == 0) ? rect.Width() : (double)rect.Width()/(double)rect.Height();
	gluPerspective(45,aspect,0.1,70.0);

	glEnable(GL_DEPTH_TEST);

	return 1;
}

BOOL CMouseSelView::OnEraseBkgnd(CDC* pDC)
{
	return 1;//Prevent flicker
	//return CView::OnEraseBkgnd(pDC);
}

void CMouseSelView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,m_hGLContext);

	double	 aspect = (cy == 0) ? cx : (double)cx/(double)cy;
	
	glViewport(0,0,cx,cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,aspect,0.1,170.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);

	// Release
	::ReleaseDC(hWnd,hDC);
}

void CMouseSelView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,m_hGLContext);
	::ReleaseDC(hWnd,hDC);
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CMouseSelView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	InitGeometry();
}

void CMouseSelView::OnDestroy()
{
	if(wglGetCurrentContext() != NULL)
		wglMakeCurrent(NULL,NULL);
	
	if(m_hGLContext != NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}

	CView::OnDestroy();
}

void CMouseSelView::OnPaint()
{
	// Device context for painting
	CPaintDC dc(this); 
	
	// pointer to Doc
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	ASSERT_VALID(pDoc);
    
	// for MDI 
	HWND hWnd = GetSafeHwnd();
	HDC hDC = ::GetDC(hWnd);
	wglMakeCurrent(hDC,m_hGLContext);
	
	//Painting background
	glClearColor(0,0,0,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	// Position / translation / scale
	glTranslated(m_xTranslation,m_yTranslation,m_zTranslation);
	glRotatef(m_xRotation, 1.0, 0.0, 0.0);
	glRotatef(m_yRotation, 0.0, 1.0, 0.0);
	glRotatef(m_zRotation, 0.0, 0.0, 1.0);

	
	UpdateDoc();
	// Start rendering...
	BeginWaitCursor();
	pDoc->RenderScene();
	EndWaitCursor();
	
	glPopMatrix();

	// Double buffer
	SwapBuffers(dc.m_ps.hdc);
	glFlush();

	// Release
	::ReleaseDC(hWnd,hDC);
}
BOOL CMouseSelView::SetWindowPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;
	
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 32;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	m_GLPixelIndex = ChoosePixelFormat(hDC,&pixelDesc);
	if(m_GLPixelIndex == 0) // Choose default
	{
		m_GLPixelIndex = 1;
		if(DescribePixelFormat(hDC,m_GLPixelIndex,
			sizeof(PIXELFORMATDESCRIPTOR),&pixelDesc)==0)
			return FALSE;
	}
	
	if(!SetPixelFormat(hDC,m_GLPixelIndex,&pixelDesc))
		return FALSE;
	return TRUE;

}
BOOL CMouseSelView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	
	if(m_hGLContext==NULL)
		return FALSE;
	
	if(wglMakeCurrent(hDC,m_hGLContext)==FALSE)
		return FALSE;
	
	return TRUE;

}

void CMouseSelView::InitGeometry()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	m_xRotation = pDoc->m_xRotation;
	m_yRotation = pDoc->m_yRotation;
	m_zRotation =pDoc-> m_zRotation;

	m_xTranslation = pDoc->m_xTranslation;
	m_yTranslation = pDoc->m_yTranslation;
	m_zTranslation = pDoc->m_zTranslation;

    m_SpeedRotation = pDoc->m_SpeedRotation;
	m_SpeedTranslation =pDoc->m_SpeedTranslation ;

	m_xyRotation =pDoc->m_xyRotation;

}

void CMouseSelView::UpdateDoc()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();
	//ASSERT_VALID(pDoc);
	pDoc->m_xRotation=m_xRotation;
	pDoc->m_yRotation=	m_yRotation;
	pDoc->m_zRotation=m_zRotation;

	pDoc->m_xTranslation=m_xTranslation;
	pDoc->m_yTranslation=	m_yTranslation ;
	pDoc->m_zTranslation=m_zTranslation ;

	pDoc->m_SpeedRotation=m_SpeedRotation ;
	pDoc->m_SpeedTranslation=m_SpeedTranslation  ;

	pDoc->m_xyRotation=m_xyRotation ;


}


void CMouseSelView::OnViewSectors()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();

    CTypedPtrList<CObList,CShape*>& t_shapeList =pDoc->m_ShapeList;

	CWnd* main = AfxGetMainWnd();

	CMenu* menu = main->GetMenu();
	
	UINT state = menu->GetMenuState(ID_VIEW_SECTORS, MF_BYCOMMAND);

	ASSERT(state != 0xFFFFFFFF);

	if (state & MF_CHECKED) {
		menu->CheckMenuItem(ID_VIEW_SECTORS, MF_UNCHECKED | MF_BYCOMMAND);
		//hide all sectors
		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CSphere)) &&
				!(shape->IsKindOf(RUNTIME_CLASS(CStardock)))) {
				CSphere *objFromList = (CSphere*) shape;
				shape->m_visible = FALSE;
			}
		}
	}
	else {
		menu->CheckMenuItem(ID_VIEW_SECTORS, MF_CHECKED | MF_BYCOMMAND);
		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CSphere)) &&
				!(shape->IsKindOf(RUNTIME_CLASS(CStardock)))) {
				CSphere *objFromList = (CSphere*) shape;
				shape->m_visible = TRUE;
			}
		}
	}
	Invalidate();
	pDoc->SetModifiedFlag(TRUE);
}

void CMouseSelView::OnViewWarplines()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();

    CTypedPtrList<CObList,CShape*>& t_shapeList =pDoc->m_ShapeList;

	CWnd* main = AfxGetMainWnd();

	CMenu* menu = main->GetMenu();
	
	UINT state = menu->GetMenuState(ID_VIEW_WARPLINES, MF_BYCOMMAND);

	ASSERT(state != 0xFFFFFFFF);

	if (state & MF_CHECKED) {
		menu->CheckMenuItem(ID_VIEW_WARPLINES, MF_UNCHECKED | MF_BYCOMMAND);
		//hide all sectors

		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CLine))) {
				CLine *objFromList = (CLine*) shape;
				shape->m_visible = FALSE;
			}
		}


	}
	else {
		menu->CheckMenuItem(ID_VIEW_WARPLINES, MF_CHECKED | MF_BYCOMMAND);

		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CLine))) {
				CLine *objFromList = (CLine*) shape;
				shape->m_visible = TRUE;
			}
		}

	}
	Invalidate();
	pDoc->SetModifiedFlag(TRUE);
}

void CMouseSelView::OnViewStardock()
{
	CMouseSelDoc* pDoc = (CMouseSelDoc*)GetDocument();

    CTypedPtrList<CObList,CShape*>& t_shapeList =pDoc->m_ShapeList;

	CWnd* main = AfxGetMainWnd();

	CMenu* menu = main->GetMenu();
	
	UINT state = menu->GetMenuState(ID_VIEW_SECTORS, MF_BYCOMMAND);

	ASSERT(state != 0xFFFFFFFF);

	if (state & MF_CHECKED) {
		menu->CheckMenuItem(ID_VIEW_STARDOCK, MF_UNCHECKED | MF_BYCOMMAND);
		//hide all sectors
		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CStardock))) {
				CStardock *objFromList = (CStardock*) shape;
				shape->m_visible = FALSE;
			}
		}
	}
	else {
		menu->CheckMenuItem(ID_VIEW_STARDOCK, MF_CHECKED | MF_BYCOMMAND);
		POSITION pos = t_shapeList.GetHeadPosition();
		while (pos != NULL)
		{
			CShape* shape = t_shapeList.GetNext(pos);
			if (shape->IsKindOf(RUNTIME_CLASS(CStardock))) {
				CStardock *objFromList = (CStardock*) shape;
				shape->m_visible = TRUE;
			}
		}
	}
	Invalidate();
	pDoc->SetModifiedFlag(TRUE);
}
