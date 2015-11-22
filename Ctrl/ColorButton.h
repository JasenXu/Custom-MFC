#pragma once


// CColorButton

class CColorButton : public CButton
{
	DECLARE_DYNAMIC(CColorButton)

public:
	CColorButton();
	virtual ~CColorButton();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();

private:
	COLORREF	m_clrBack;		// 背景色
	COLORREF	m_clrFore;		// 前景色
	COLORREF	m_clrDisabled;
	BOOL		m_bMouseOver;
	BOOL		m_bCheck;
	int			m_nGradient;
	CFont		*m_pFont;

public:
	//  =======================================================================
	//	设置背景色
	//  =======================================================================
	void SetBkColor(COLORREF clrBack);
	
	//  =======================================================================
	//	获取背景色
	//  =======================================================================
	COLORREF GetBkColor(void);
	
	//  =======================================================================
	//	设置前景色
	//  =======================================================================
	void SetFeColor(COLORREF clrFore);
	
	//  =======================================================================
	//	获取前景色
	//  =======================================================================
	COLORREF GetFeColor(void);

	//  =======================================================================
	//	设置按下状态
	//  =======================================================================
	void SetCheck(BOOL bCheck);
	
private:
	//  =======================================================================
	// 绘制按钮按下状态
	//  =======================================================================
	void DrawButtonDown(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	绘制鼠标经过状态
	//  =======================================================================
	void DrawMouseOverStatus(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	绘制按钮失效状态
	//  =======================================================================
	void DrawDisabledStatus(CDC* pDC, CRect rc, CString strCaption);

	//  =======================================================================
	//	绘制按钮立体背景
	//  =======================================================================
	void DrawSolidBackgroud(CDC* pDC, CRect rc, int r, int g, int b);
public:
	void SetFont(CFont * pFont);
};


