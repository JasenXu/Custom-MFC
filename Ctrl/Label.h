/********************************************************************
 * Copyright (C) 2014 All Right Reserved
 * \date 	 2014/11/21 16:18
 * \file 	 Label.h
 * \version  1.0.0.0
 * \author 	 JasenXu
 * \brief 	 自定义标签的实现
*********************************************************************/
#pragma once
#include "afxwin.h"

class CLabel : public CWnd
{
public:
	CLabel();
	virtual ~CLabel();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CString			m_strLabel;		// 标签内容
	CFont			*m_pFont;		// 自定义字体
	COLORREF		m_clrText;		// 文本色
	COLORREF		m_clrBkgr;		// 背景色
	CFont			m_font;			// 默认字体
	BOOL			m_bNoBorder;	// 无边框标志
	COLORREF		m_clrBorder;	// 边框色
	CString			m_strPrefix;	// 前缀文本
	CToolTipCtrl	m_toolTip;		// 提示窗口
	UINT			m_nFormat;		// 对齐方式

public:
	void SetLabelText(CString strLabel);
	void SetLabelInt(int nLabel);

	void SetTextColor(COLORREF clrText);
	void SetBkColor(COLORREF clrBkgr);

	BOOL Create(const RECT& rect, CWnd * pParentWnd);

	void SetNoBorder(BOOL bNoBorder);
	void SetBorderColor(COLORREF clrBorder);
	void SetFont(CFont * pFont);
	void SetPrefixString(CString strPrefix);	
	void SetTextFormat(UINT nFormat);
};

