#pragma once
#include "afxwin.h"

class CColorEdit : public CEdit
{
public:
	CColorEdit();
	virtual ~CColorEdit();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

public:
	void SetBkColor(COLORREF clrBk);
	void SetTextColor(COLORREF clrText);
	void SetFont(CFont * pFont);

protected:
	COLORREF		m_clrText;
	COLORREF		m_clrBkgr;
	CFont			*m_pFont;
	CBrush			m_brBkgr;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

