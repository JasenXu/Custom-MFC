#pragma once
#include "afxwin.h"


//  =======================================================================
//	类名		CRedoComboBox
//	描述		用于绘制自定义的ComboBox控件
//  =======================================================================

class CRedoComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CRedoComboBox)

public:
	CRedoComboBox();
	virtual ~CRedoComboBox();

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	DECLARE_MESSAGE_MAP()

public:
	//  =======================================================================
	//  设置前景色
	//  =======================================================================
	void SetForeColor(COLORREF clrFore);

	//  =======================================================================
	//  设置背景色
	//  =======================================================================
	void SetBkColor(COLORREF clrBack);

	//  =======================================================================
	//  获取前景色
	//  =======================================================================
	COLORREF GetForeColor(void);

	//  =======================================================================
	//  获取背景色
	//  =======================================================================
	COLORREF GetBkColor(void);

	//  =======================================================================
	//  设置选择前景色
	//  =======================================================================
	void SetSelForeColor(COLORREF clrFore);

	//  =======================================================================
	//  设置选择背景色
	//  =======================================================================
	void SetSelBkColor(COLORREF clrBack);

private:
	COLORREF m_clrFore;		// 前景色
	COLORREF m_clrBack;		// 背景色

	COLORREF m_clrSelFore;	// 选择前景色
	COLORREF m_clrSelBack;	// 选择背景色

	CFont	*m_pFont;

public:
	void SetFont(CFont * pFont);

};


