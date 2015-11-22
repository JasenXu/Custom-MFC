/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/14 9:34
	FileName: 	FindFile.cpp
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	遍历文件夹查找文件
*********************************************************************/
#include "StdAfx.h"
#include "FindFile.h"


CFindFile::CFindFile(void)
	: m_pFindThread(NULL)
	, m_bFind(FALSE)
	, m_strMainPath(_T(""))
	, m_hComplete(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_bNoExtension(FALSE)
{
}


CFindFile::~CFindFile(void)
{
	// 结束遍历线程
	m_bFind = FALSE;
	if (m_pFindThread)
	{
		if (::WaitForSingleObject(m_pFindThread->m_hThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_pFindThread->m_hThread, 1);
		}
		m_pFindThread = NULL;
	}

	// 清空结果列表
	ClearFileList();
}


//************************************************************************
// Method:    IsRoot
// Purpose:   判断是否为根目录
// Access:    protected 
// Returns:   BOOL		是为TRUE，否为FALSE
// Qualifier:
// Parameter: LPCTSTR lpszPath	要判断的目录
//************************************************************************
BOOL CFindFile::IsRoot(LPCTSTR lpszPath)
{
	TCHAR tszRoot[4] = {0};
	wsprintf(tszRoot, _T("%c:\\"), lpszPath[0]);
	return (lstrcmp(tszRoot, lpszPath) == 0);
}


//************************************************************************
// Method:    FindProc
// Purpose:   遍历线程
// Access:    protected static 
// Returns:   UINT
// Qualifier:
// Parameter: LPVOID lpParam
//************************************************************************
UINT CFindFile::FindProc(LPVOID lpParam)
{
	if (lpParam)
	{
		CFindFile *ptr = (CFindFile *)lpParam;
		ptr->FindLoop();

		SetEvent(ptr->m_hComplete);
	}

	return 0;
}


void CFindFile::FindLoop(void)
{
	FindAllFile(m_strMainPath);
}


//************************************************************************
// Method:    StartFind
// Purpose:   开始遍历
// Access:    public 
// Returns:   BOOL		成功为TRUE，失败为FALSE
// Qualifier:
// Parameter: LPCTSTR lpszPath		遍历的目录
//************************************************************************
BOOL CFindFile::StartFind(LPCTSTR lpszPath)
{
	// 清空结果列表
	ClearFileList();

	m_strMainPath = lpszPath;

	m_bFind = TRUE;
	m_pFindThread = AfxBeginThread(AFX_THREADPROC(FindProc), (LPVOID)this);

	return (m_pFindThread != NULL);
}


//************************************************************************
// Method:    StopFind
// Purpose:   停止遍历
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFindFile::StopFind(void)
{
	m_bFind = FALSE;
	if (m_pFindThread)
	{
		if (::WaitForSingleObject(m_pFindThread->m_hThread, 5000) == WAIT_TIMEOUT)
		{
			::TerminateThread(m_pFindThread->m_hThread, 1);
		}
		m_pFindThread = NULL;
	}
}


//************************************************************************
// Method:    FindAllFile
// Purpose:   递归遍历，查找所有文件
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath		要遍历的目录
//************************************************************************
void CFindFile::FindAllFile(LPCTSTR lpszPath)
{
	m_bFind = TRUE;

	// 开始查找
	TCHAR tszFind[MAX_PATH] = {0};
	lstrcpy(tszFind, lpszPath);

	if (!IsRoot(tszFind))
	{
		lstrcat(tszFind, _T("\\"));           
	}

	// 查找所有文件
	lstrcat(tszFind, _T("*.*"));

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(tszFind, &wfd);

	// 如果没有找到或查找失败
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	} 
	
	do 
	{
		// 过滤这两个目录
		if (wfd.cFileName[0] == '.')
			continue;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR tszFile[MAX_PATH] = {0};
			if (IsRoot(lpszPath))
			{
				wsprintf(tszFile, _T("%s%s"), lpszPath, wfd.cFileName);
			}
			else
			{
				wsprintf(tszFile, _T("%s\\%s"), lpszPath, wfd.cFileName);
			}

			// 如果找到的是目录，则进入此目录进行递归
			FindAllFile(tszFile);
		} 
		else
		{
			// 对文件进行操作
			// 筛选文件
			CString strName(wfd.cFileName);
			int nIndex = strName.ReverseFind('.');
			if (nIndex != -1)
			{
				BOOL bFindOut(FALSE);

				strName = strName.Right(strName.GetLength()-nIndex-1);
				strName.MakeLower();

				POSITION pos = m_listExtension.GetHeadPosition();
				while (pos && m_bFind && bFindOut == FALSE)
				{
					LPCTSTR lpsz = m_listExtension.GetNext(pos);
					CString strExtension(lpsz);
					strExtension.MakeLower();

					if (strName.Compare(strExtension) == 0)
						bFindOut = TRUE;
				}

				if (bFindOut)
				{
					// 添加到结果列表中
					CString strResult(lpszPath);
					strResult += _T("\\");
					strResult += wfd.cFileName;

					char *psz = new char[MAX_PATH]();
					int nLen = ::WideCharToMultiByte(CP_ACP, 0, strResult, -1, NULL, 0, NULL, NULL);
					::WideCharToMultiByte(CP_ACP, 0, strResult, -1, psz, nLen, NULL, NULL);

					m_listFile.AddTail(psz);
				}
			}
			else if (nIndex == -1 && m_bNoExtension == TRUE)
			{
				// 无扩展名的文件
				// 添加到结果列表中
				CString strResult(lpszPath);
				strResult += _T("\\");
				strResult += wfd.cFileName;

				char *psz = new char[MAX_PATH]();
				int nLen = ::WideCharToMultiByte(CP_ACP, 0, strResult, -1, NULL, 0, NULL, NULL);
				::WideCharToMultiByte(CP_ACP, 0, strResult, -1, psz, nLen, NULL, NULL);

				m_listFile.AddTail(psz);
			}
		}

	} while (FindNextFile(hFind, &wfd) && m_bFind == TRUE);

	// 关闭查找句柄
	FindClose(hFind);
}


//************************************************************************
// Method:    ClearResultList
// Purpose:   清空文件列表
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFindFile::ClearFileList(void)
{
	POSITION pos = m_listFile.GetHeadPosition();
	while (pos)
	{
		LPSTR ptr = m_listFile.GetNext(pos);
		delete[] ptr;
		ptr = NULL;
	}

	m_listFile.RemoveAll();
}


//************************************************************************
// Method:    GetFirstPosition
// Purpose:   获取首位置
// Access:    public 
// Returns:   POSITION
// Qualifier:
// Parameter: void
//************************************************************************
POSITION CFindFile::GetFirstPosition(void)
{
	return m_listFile.GetHeadPosition();
}


//************************************************************************
// Method:    GetLastPosition
// Purpose:   获取尾位置
// Access:    public 
// Returns:   POSITION
// Qualifier:
// Parameter: void
//************************************************************************
POSITION CFindFile::GetLastPosition(void)
{
	return m_listFile.GetTailPosition();
}


//************************************************************************
// Method:    GetNextFile
// Purpose:   获取下一文件	
// Access:    public 
// Returns:   TYPE&						文件名
// Qualifier:
// Parameter: POSITION & rPosition		输入要查找的位置，返回下一个位置
//************************************************************************
TYPE& CFindFile::GetNextFile(POSITION& rPosition)
{
	return m_listFile.GetNext(rPosition);
}


//************************************************************************
// Method:    GetPrevFile
// Purpose:   获取上一个文件
// Access:    public 
// Returns:   TYPE&						文件名
// Qualifier:
// Parameter: POSITION & rPosition		输入要查找的位置，返回上一个位置
//************************************************************************
TYPE& CFindFile::GetPrevFile(POSITION& rPosition)
{
	return m_listFile.GetPrev(rPosition);
}


//************************************************************************
// Method:    IsComplete
// Purpose:   是否完成遍历
// Access:    public 
// Returns:   BOOL		完成为TRUE，否则为FALSE
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CFindFile::IsComplete(void)
{
	if (::WaitForSingleObject(m_hComplete, 0) != WAIT_TIMEOUT)
		return TRUE;

	return FALSE;
}


//************************************************************************
// Method:    SetFileExtension
// Purpose:   设置要搜索的文件扩展名
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszExtension		文件扩展名
//************************************************************************
void CFindFile::SetFileExtension(LPCTSTR lpszExtension)
{
	m_listExtension.AddTail(lpszExtension);
}


//************************************************************************
// Method:    IncludeNoExtension
// Purpose:   包括没有扩展名的文件
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: BOOL bNoExtension		包括为TRUE，否则为FALSE
//************************************************************************
void CFindFile::IncludeNoExtension(BOOL bNoExtension)
{
	m_bNoExtension = bNoExtension;
}