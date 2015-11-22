#pragma once


typedef enum tagButtonState
{
	BS_Normal = 0, BS_Hot, BS_Disabled

} BUTTON_STATUS;

// CImageButton

class CImageButton : public CButton
{
	DECLARE_DYNAMIC(CImageButton)

public:
	CImageButton();
	virtual ~CImageButton();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CFont			*m_pFont;			// 文本字体

	BOOL			m_bMouseOver;		// 鼠标经过标记
	BOOL			m_bMouseDown;		// 鼠标按下标记

	COLORREF		m_clrGround;		// 按钮底色

	COLORREF		m_clrBkByNormal;	// 正常背景色
	COLORREF		m_clrEdgeByNormal;	// 正常边界色

	COLORREF		m_clrBkByOver;		// 鼠标经过背景色
	COLORREF		m_clrEdgeByOver;	// 鼠标经过边界色

	COLORREF		m_clrBkByDown;		// 鼠标按下背景色
	COLORREF		m_clrEdgeByDown;	// 鼠标按下边界色

	HIMAGELIST		m_hImageList;
	UINT			m_nBitmapID;
	BOOL			m_bDisabled;

	int				m_nWidth;			// 按钮宽度
	int				m_nHeight;			// 按钮高度
	CToolTipCtrl	m_toolTip;			// 提示窗口

public:
	void SetBackColorByNormal(COLORREF clrBk);
	void SetEdgeColorByNormal(COLORREF clrEdge);

	void SetBackColorByOver(COLORREF clrBk);
	void SetEdgeColorByOver(COLORREF clrEdge);

	void SetBackColorByDown(COLORREF clrBk);
	void SetEdgeColorByDown(COLORREF clrEdge);

	void SetGroundColor(COLORREF clrGround);

public:
	BOOL Create(const RECT& rect, CWnd * pParentWnd, UINT nID);

	void SetFont(CFont * pFont);
	BOOL SetImageList(BUTTON_STATUS state, UINT uBitmapID, const SIZE& buttonSize, DWORD numButtons, COLORREF mask);
	void SetBitmap(UINT nBitmapID);

	BOOL EnableWindow(BOOL bEnable = TRUE);
	BOOL IsButtonEnable(void);

	void SetButtonSize(CSize size);
	void SetButtonSize(int cx, int cy);
	
	void SetToolTipText(CString strTip);
};


