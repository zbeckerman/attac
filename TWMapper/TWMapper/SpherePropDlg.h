#pragma once
// CSpherePropDlg dialog

class CSpherePropDlg : public CDialog
{
	DECLARE_DYNAMIC(CSpherePropDlg)

public:
	CSpherePropDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpherePropDlg();
	void SetColor(COLORREF&);
	void UpDateRGBValues();

// Dialog Data
	enum { IDD = IDD_COLOR };
	CSpinButtonCtrl	m_spinRed;
	CSpinButtonCtrl	m_spinGreen;
	CSpinButtonCtrl	m_spinBlue;
	CSliderCtrl	m_sldRed;
	CSliderCtrl	m_sldGreen;
	CSliderCtrl	m_sldBlue;
	double	m_dRadius;
    int m_Red, m_Green, m_Blue;
	CRect m_RectNew;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
