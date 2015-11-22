/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/14 9:34
	FileName: 	FindFile.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	遍历文件夹查找文件
*********************************************************************/
#pragma once

// 定义列表项数据类型
typedef LPSTR TYPE;

class CFindFile
{
public:
	CFindFile(void);
	virtual ~CFindFile(void);
	

protected:
	CWinThread	*m_pFindThread;

	static UINT FindProc(LPVOID lpParam);
	void FindLoop(void);
public:
	BOOL IsRoot(LPCTSTR lpszPath);
	void FindAllFile(LPCTSTR lpszPath);

	void ClearFileList(void);

public:
	BOOL StartFind(LPCTSTR lpszPath);
	void StopFind(void);

	void SetFileExtension(LPCTSTR lpszExtension);
	void IncludeNoExtension(BOOL bNoExtension);
	
protected:
	CString			m_strMainPath;			// 主路径
	BOOL			m_bFind;				// 遍历标记
	CList<TYPE>		m_listFile;			// 结果列表
	HANDLE			m_hComplete;			// 遍历完成标记
	CList<LPCTSTR>	m_listExtension;		// 要搜索的文件扩展名列表
	BOOL			m_bNoExtension;			// 包括无扩展名的文件

public:	
	BOOL IsComplete(void);

	POSITION GetFirstPosition(void);
	POSITION GetLastPosition(void);
	TYPE& GetNextFile(POSITION& rPosition);
	TYPE& GetPrevFile(POSITION& rPosition);
};

