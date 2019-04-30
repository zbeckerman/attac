// SpherePropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MouseSel.h"
#include "SpherePropDlg.h"


// CSpherePropDlg dialog

IMPLEMENT_DYNAMIC(CSpherePropDlg, CDialog)
CSpherePropDlg::CSpherePropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpherePropDlg::IDD, pParent)
{
	m_dRadius=1.;
	m_Red = 0;
	m_Blue = 0;
	m_Green = 255;
}

CSpherePropDlg::~CSpherePropDlg()
{
}

void CSpherePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_SPIN_RED, m_spinRed);
	DDX_Control(pDX, IDC_SPIN_GREEN, m_spinGreen);
	DDX_Control(pDX, IDC_SPIN_BLUE, m_spinBlue);
	DDX_Control(pDX, IDC_SLIDER_RED, m_sldRed);
	DDX_Control(pDX, IDC_SLIDER_GREEN, m_sldGreen);
	DDX_Control(pDX, IDC_SLIDER_BLUE, m_sldBlue);
	DDX_Text(pDX, IDC_RADIUS, m_dRadius);
	DDV_MinMaxDouble(pDX, m_dRadius, 0., 1000.);
}


BEGIN_MESSAGE_MAP(CSpherePropDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CSpherePropDlg message handlers
BOOL CSpherePropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	
	m_sldRed.SetRange(0,255);	//Sliders
	m_sldGreen.SetRange(0,255);
	m_sldBlue.SetRange(0,255);


	m_spinRed.SetRange(0,255);	//Spinners
	m_spinGreen.SetRange(0,255);
	m_spinBlue.SetRange(0,255);


	m_sldRed.SetTic(85); 
	m_sldGreen.SetTic(85);
	m_sldBlue.SetTic(85);


	m_sldRed.SetTicFreq(10); 
	m_sldGreen.SetTicFreq(10); 
	m_sldBlue.SetTicFreq(10); 

	
	m_sldRed.SetLineSize(10); // Arrows
	m_sldGreen.SetLineSize(10);
	m_sldBlue.SetLineSize(10);


	m_sldRed.SetPageSize(50); // Pages
	m_sldGreen.SetPageSize(50);
	m_sldBlue.SetPageSize(50);

	UpDateRGBValues();
	m_sldRed.SetPos(m_Red);
	m_sldGreen.SetPos(m_Green);
	m_sldBlue.SetPos(m_Blue);


	GetDlgItem(IDC_COLOR_NEW)->GetWindowRect(&m_RectNew);
	ScreenToClient(&m_RectNew);
	
	// Calculate the area borders and divide by 8 
	int Border = (m_RectNew.right - m_RectNew.left) / 22;
	
	m_RectNew.InflateRect (-Border, -Border);

	

	return TRUE;  

}

void CSpherePropDlg::OnPaint() 
{
	CPaintDC dc(this); 
		
	m_Red		=	m_sldRed.GetPos();
	m_Green		=	m_sldGreen.GetPos();
	m_Blue		=	m_sldBlue.GetPos();
	
	CBrush Brush(RGB(m_Red,m_Green,m_Blue));
	CBrush dcClr(RGB(m_Red,m_Green,m_Blue));

	dc.FillRect(&m_RectNew,&Brush);
			
	UpDateRGBValues();

	CDialog::OnPaint();
}

void CSpherePropDlg::OnOK() 
{
	CPaintDC dc(this); // device context for painting

	UpdateData();

	dc.SetBkColor(RGB(m_Red,m_Green,m_Blue));
	
	CDialog::OnOK();
}

void CSpherePropDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
		CPaintDC dc(this); // device context for painting

	CSliderCtrl* m_sldRed	= (CSliderCtrl*) pScrollBar;
	CSliderCtrl* m_sldGreen	= (CSliderCtrl*) pScrollBar;
	CSliderCtrl* m_sldBlue	= (CSliderCtrl*) pScrollBar;
	
	m_Red	=	m_sldRed->GetPos();
	m_Green	=	m_sldGreen->GetPos();
	m_Blue	=	m_sldBlue->GetPos();
	
	UpDateRGBValues();

	CBrush Brush(RGB(m_Red,m_Green,m_Blue));
	dc.FillRect(&m_RectNew,&Brush);
	
	Invalidate(FALSE);
	UpdateWindow();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

}
BOOL CSpherePropDlg::OnEraseBkgnd(CDC* pDC) 
{
  
	return CDialog::OnEraseBkgnd(pDC);
}


void CSpherePropDlg::UpDateRGBValues()
{

	m_spinRed.SetPos(m_sldRed.GetPos()); 
	m_spinGreen.SetPos(m_sldGreen.GetPos());
	m_spinBlue.SetPos(m_sldBlue.GetPos());
	
	
	UpdateData();

}
void CSpherePropDlg::SetColor(COLORREF& color)
{

	CPaintDC dc(this);
	color = dc.GetBkColor();
}
