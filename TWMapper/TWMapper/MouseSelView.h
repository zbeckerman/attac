// MouseSelView.h : interface of the CMouseSelView class
//


#pragma once
class CSphere; //Forward Declaration
class CMouseSelView : public CView
{
protected: // create from serialization only
	CMouseSelView();
	DECLARE_DYNCREATE(CMouseSelView)

// Attributes
public:
	CMouseSelDoc* GetDocument() const;
	//Shape
	CShape *m_sph;
	//OpenGL
	BOOL CreateViewGLContext(HDC hDC);
	BOOL SetWindowPixelFormat(HDC hDC);
	HGLRC m_hGLContext;
	int m_GLPixelIndex;

	// Mouse 
	BOOL m_LeftButtonDown;
	BOOL m_RightButtonDown;
	CPoint m_LeftDownPos;
	CPoint m_RightDownPos;

	// Position, rotation ,scaling
	float m_xRotation;
	float m_yRotation;
	float m_zRotation;
	BOOL m_xyRotation;

	float m_xTranslation;
	float m_yTranslation;
	float m_zTranslation;

	float m_SpeedTranslation;
	float m_SpeedRotation;
// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CMouseSelView();
	void UpdateDoc();
	void InitGeometry();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEditProperties();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnViewSectors();
	afx_msg void OnViewWarplines();
	afx_msg void OnViewStardock();
};

#ifndef _DEBUG  // debug version in MouseSelView.cpp
inline CMouseSelDoc* CMouseSelView::GetDocument() const
   { return reinterpret_cast<CMouseSelDoc*>(m_pDocument); }
#endif

