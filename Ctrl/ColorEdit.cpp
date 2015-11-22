#include "StdAfx.h"
#include "ColorEdit.h"


CColorEdit::CColorEdit()
	: m_pFont(NULL)
{
	m_clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkgr = GetSysColor(COLOR_WINDOW);
	m_brBkgr.CreateSolidBrush(GetSysColor(COLOR_WINDOW));
}


CColorEdit::~CColorEdit()
{
}


BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
//	ON_WM_PAINT()
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
//	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_clrText);		// 修改文本色
	pDC->SetBkColor(m_clrBkgr);			// 修改文本背景色

	// 修改背景色
	return m_brBkgr;
}


//************************************************************************
// Method:    SetBkColor
// Brief:	  设置背景色
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrBk
//************************************************************************
void CColorEdit::SetBkColor(COLORREF clrBk)
{
	m_clrBkgr = clrBk;

	if (m_brBkgr.GetSafeHandle())
	{
		m_brBkgr.DeleteObject();
	}

	m_brBkgr.CreateSolidBrush(clrBk);

	Invalidate(TRUE);
}


//************************************************************************
// Method:    SetTextColor
// Brief:	  设置文本色
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrText
//************************************************************************
void CColorEdit::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;

	Invalidate(TRUE);
}


//************************************************************************
// Method:    SetFont
// Brief:	  设置字体
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CFont * pFont
//************************************************************************
void CColorEdit::SetFont(CFont * pFont)
{
	if (pFont)
	{
		m_pFont = pFont;
		CEdit::SetFont(m_pFont);
		Invalidate(TRUE);
	}
}


BOOL CColorEdit::PreCreateWindow(CREATESTRUCT& cs)
{
//	cs.style &= ~WS_BORDER;

	return CEdit::PreCreateWindow(cs);
}


void CColorEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);
	rc.InflateRect(1, 1, 1, 1);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	memDC.SelectObject(m_pFont);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&memDC, rc.Width(), rc.Height());
	memDC.SelectObject(&memBmp);
	memDC.FillSolidRect(&rc, m_clrBkgr);

	memDC.FrameRect(&rc, &CBrush(RGB(133, 145, 162)));

	CString strLabel(_T(""));
	GetWindowText(strLabel);
	memDC.DrawText(strLabel, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	CEdit::OnPaint();
}


BOOL CColorEdit::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CEdit::OnEraseBkgnd(pDC);
}
