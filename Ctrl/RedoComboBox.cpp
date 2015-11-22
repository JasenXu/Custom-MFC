// RedoComboBox.cpp : 实现文件
//

#include "stdafx.h"
#include "RedoComboBox.h"


// CRedoComboBox

IMPLEMENT_DYNAMIC(CRedoComboBox, CComboBox)

CRedoComboBox::CRedoComboBox()
	: m_clrFore(RGB(0, 0, 0))
	, m_clrBack(RGB(255, 255, 255))
	, m_clrSelFore(RGB(255, 255, 255))
	, m_clrSelBack(RGB(51, 153, 255))
{

}

CRedoComboBox::~CRedoComboBox()
{
}


BEGIN_MESSAGE_MAP(CRedoComboBox, CComboBox)

END_MESSAGE_MAP()



// CRedoComboBox 消息处理程序




void CRedoComboBox::PreSubclassWindow()
{

	CComboBox::PreSubclassWindow();
}

//  ...........................................................................
//  重绘下拉列表
//  ...........................................................................
void CRedoComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	RECT rcItem(lpDrawItemStruct->rcItem);
	UINT nState(lpDrawItemStruct->itemState);

	// 重绘背景
	dc.SetBkMode(TRANSPARENT);
	dc.FillSolidRect(&rcItem, m_clrBack);

	// 设置字体
//	CFont font;
//	font.CreatePointFont(90, _T("微软雅黑"), &dc);
	dc.SelectObject(m_pFont);

	// 显示标题
	CString strLb(_T(""));
	GetLBText(lpDrawItemStruct->itemID, strLb);
	dc.SetTextColor(m_clrFore);
	dc.DrawText(strLb, &rcItem, DT_SINGLELINE | DT_LEFT);

	// 选择状态
	if (nState & ODS_SELECTED)
	{
		dc.FillSolidRect(&rcItem, m_clrSelBack);
		GetLBText(GetCurSel(), strLb);
		dc.SetTextColor(m_clrSelFore);
		dc.DrawText(strLb, &rcItem, DT_SINGLELINE | DT_LEFT);
	}

	dc.Detach();
}

//  ...........................................................................
//  改变宽度高度（对子类化无效）
//  ...........................................................................
void CRedoComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if ((LONG)lpMeasureItemStruct->itemID == -1)
	{
		// 文本框
		lpMeasureItemStruct->itemHeight = 15;
	}
	else
	{
		// 列表
		lpMeasureItemStruct->itemHeight = 40;
	}
}

//  ...........................................................................
//  设置前景色
//  ...........................................................................
void CRedoComboBox::SetForeColor(COLORREF clrFore)
{
	m_clrFore = clrFore;
}

//  ...........................................................................
//  设置背景色
//  ...........................................................................
void CRedoComboBox::SetBkColor(COLORREF clrBack)
{
	m_clrBack = clrBack;
}

//  ...........................................................................
//  获取前景色
//  ...........................................................................
COLORREF CRedoComboBox::GetForeColor(void)
{
	return COLORREF(m_clrFore);
}

//  ...........................................................................
//  获取背景色
//  ...........................................................................
COLORREF CRedoComboBox::GetBkColor(void)
{
	return COLORREF(m_clrBack);
}

//  ...........................................................................
//  设置选择前景色
//  ...........................................................................
void CRedoComboBox::SetSelForeColor(COLORREF clrFore)
{
	m_clrSelFore = clrFore;
}

//  ...........................................................................
//  设置选择背景色
//  ...........................................................................
void CRedoComboBox::SetSelBkColor(COLORREF clrBack)
{
	m_clrSelBack = clrBack;
}

void CRedoComboBox::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}
