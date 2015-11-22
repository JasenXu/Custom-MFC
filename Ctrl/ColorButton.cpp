// ColorButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorButton.h"


// CColorButton

IMPLEMENT_DYNAMIC(CColorButton, CButton)

CColorButton::CColorButton()
	: m_clrBack(RGB(25, 149, 248))
	, m_clrFore(RGB(255, 255, 255))
	, m_clrDisabled(RGB(155, 155, 155))
	, m_bMouseOver(FALSE)
	, m_bCheck(FALSE)
	, m_pFont(NULL)
{

}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CColorButton 消息处理程序




void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	CRect rc(lpDrawItemStruct->rcItem);

	CString strCaption(_T(""));
	GetWindowText(strCaption);

	dc.SetBkMode(TRANSPARENT);
//	dc.FillSolidRect(&rc, m_clrBack);
	
//	CRect rcOffset(rc);
//	rcOffset.OffsetRect(2, 2);
//	dc.FillSolidRect(&rcOffset, RGB(172, 172, 172));

	DrawMouseOverStatus(&dc, rc, strCaption);

	// 按钮被按下
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		DrawButtonDown(&dc, rc, strCaption);
	}

	if (m_bCheck)
	{
		DrawButtonDown(&dc, rc, strCaption);
	} 

	// 按钮失效
	if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{
		DrawDisabledStatus(&dc, rc, strCaption);
	}

	dc.Detach();
}

//  ...........................................................................
// 设置背景色
//  ...........................................................................
void CColorButton::SetBkColor(COLORREF clrBack)
{
	m_clrBack = clrBack;
}

//  ...........................................................................
// 获取背景色
//  ...........................................................................
COLORREF CColorButton::GetBkColor(void)
{
	return COLORREF(m_clrBack);
}

//  ...........................................................................
// 设置前景色
//  ...........................................................................
void CColorButton::SetFeColor(COLORREF clrFore)
{
	m_clrFore = clrFore;
}

//  ...........................................................................
// 获取前景色
//  ...........................................................................
COLORREF CColorButton::GetFeColor(void)
{
	return COLORREF(m_clrFore);
}

//  ...........................................................................
//  修改控件风格
//  ...........................................................................
void CColorButton::PreSubclassWindow()
{
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
	CButton::PreSubclassWindow();
}

//  ...........................................................................
//  鼠标经过消息处理
//  ...........................................................................
void CColorButton::OnMouseMove(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CWnd* wndUnderMouse(WindowFromPoint(point));
	TRACKMOUSEEVENT csTME = {0};

	// 点击按钮无效
	if (nFlags & MK_LBUTTON && m_bMouseOver == FALSE) return;

	if (wndUnderMouse && wndUnderMouse->m_hWnd == m_hWnd)
	{
		if (!m_bMouseOver)
		{
			m_bMouseOver = TRUE;

			Invalidate(FALSE);

			// 跟踪鼠标离开的行为
			csTME.cbSize = sizeof(csTME);
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = m_hWnd;
			::_TrackMouseEvent(&csTME);
		}
	} 

	CButton::OnMouseMove(nFlags, point);
}

//  ...........................................................................
//  鼠标离开消息处理
//  ...........................................................................
void CColorButton::OnMouseLeave()
{
	if (m_bMouseOver)
	{
		m_bMouseOver = FALSE;
	}
	Invalidate(FALSE);

	CButton::OnMouseLeave();
}

//  ...........................................................................
// 设置按下状态
//  ...........................................................................
void CColorButton::SetCheck(BOOL bCheck)
{
	m_bCheck = bCheck;
	Invalidate(FALSE);
}

//  ...........................................................................
// 绘制按钮按下状态
//  ...........................................................................
void CColorButton::DrawButtonDown(CDC* pDC, CRect rc, CString strCaption)
{
	// 加深背景色
	int r(GetRValue(m_clrBack));
	int g(GetGValue(m_clrBack));
	int b(GetBValue(m_clrBack));
	r = max(r-30, 0);
	g = max(g-30, 0);
	b = max(b-30, 0);

#if 0
	CBrush brushBk(RGB(r, g, b));
	CBrush* pOldBrush = pDC->SelectObject(&brushBk);

	CPen penBlack;
	penBlack.CreatePen(PS_SOLID, 1, RGB(r, g, b));
	CPen* pOldPen = pDC->SelectObject(&penBlack);
	POINT pt;
	pt.x = 4;
	pt.y = 4;
	pDC->RoundRect(&rc, pt);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
#else
	pDC->FillSolidRect(&rc, RGB(r, g, b));
#endif

	// 显示标题
	if (m_pFont)
	{
		pDC->SelectObject(m_pFont);
	}
	pDC->SetTextColor(m_clrFore);
	pDC->DrawText(strCaption, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

//  ...........................................................................
// 绘制鼠标经过状态
//  ...........................................................................
void CColorButton::DrawMouseOverStatus(CDC* pDC, CRect rc, CString strCaption)
{
// 	CPen pen(PS_SOLID, 1, RGB(149, 153, 156));
// 	pDC->SelectObject(&pen);

	int r(GetRValue(m_clrBack));
	int g(GetGValue(m_clrBack));
	int b(GetBValue(m_clrBack));
	if (m_bMouseOver)
	{
		r = min(r+10, 255);
		g = min(g+10, 255);
		b = min(b+10, 255);
	} 
	else
	{
		r = max(r-10, 0);
		g = max(g-10, 0);
		b = max(b-10, 0);
	}
	pDC->FillSolidRect(&rc, RGB(r, g, b));

	CPen pen(PS_SOLID, 1, RGB(r+20, g+20, b+20));
	pDC->SelectObject(&pen);

	pDC->MoveTo(rc.left+1, rc.bottom-1);
	pDC->LineTo(rc.right-1, rc.bottom-1);
	pDC->LineTo(rc.right-1, rc.top);

	if (m_pFont)
	{
		pDC->SelectObject(m_pFont);
	}
	pDC->SetTextColor(m_clrFore);
	pDC->DrawText(strCaption, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

//  ...........................................................................
// 绘制按钮失效状态
//  ...........................................................................
void CColorButton::DrawDisabledStatus(CDC* pDC, CRect rc, CString strCaption)
{
	pDC->FillSolidRect(&rc, m_clrDisabled);

	pDC->SetTextColor(m_clrFore);
	if (m_pFont)
	{
		pDC->SelectObject(m_pFont);
	}
	pDC->DrawText(strCaption, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}


void CColorButton::DrawSolidBackgroud(CDC* pDC, CRect rc, int r, int g, int b)
{
	for (int i(0); i <= rc.bottom; ++i)
	{
		rc.bottom = rc.top + 1;
		pDC->FillRect(&rc, &CBrush(RGB(r, g, b)));

		r = max(r-m_nGradient, 0);
		g = max(g-m_nGradient, 0);
		b = max(b-m_nGradient, 0);

		rc.top = rc.bottom;
	}
}


void CColorButton::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}
