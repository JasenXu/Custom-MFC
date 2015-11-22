/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/17 19:26
	FileName: 	FileEnum.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	遍历搜索文件目录
*********************************************************************/
#include "StdAfx.h"
#include "FileEnum.h"



/************************************************************************/
/* 文件搜索基本实现类                                                      */
/************************************************************************/
CBaseFileFinder::CBaseFileFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi)
	: m_fileInfo(fdi)
	, m_strPath(_T(""))
	, m_nFolderLen(0)
	, m_hFind(NULL)
{
	Init(lpszDirectory);
}


CBaseFileFinder::~CBaseFileFinder(void)
{
}


//************************************************************************
// Method:    Init
// Purpose:   去除目录后面自带的"\\"
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszDirectory	要遍历的目录
//************************************************************************
void CBaseFileFinder::Init(LPCTSTR lpszDirectory)
{
	m_strPath = lpszDirectory;
	m_nFolderLen = m_strPath.GetLength();

	if (m_strPath[m_nFolderLen-1] == _T('\\'))
	{
		m_strPath.TrimRight(_T('\\'));
		m_nFolderLen = m_strPath.GetLength();
	}
}


//************************************************************************
// Method:    EnumCurDirFirst
// Purpose:   查找当前目录的第一个文件
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::FindCurDirFirst(void)
{
	m_strPath += _T("\\*");
	++m_nFolderLen;

	m_hFind = ::FindFirstFile(m_strPath, &m_fileInfo);
	if (m_hFind != INVALID_HANDLE_VALUE)
	{
		m_strPath = m_strPath.Left(m_nFolderLen) + m_fileInfo.cFileName;
		return TRUE;
	} 

	return FALSE;
}


//************************************************************************
// Method:    FindCurDirNext
// Purpose:   查找当前目录的下一个文件
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::FindCurDirNext(void)
{
	BOOL bRet = ::FindNextFile(m_hFind, &m_fileInfo);
	if (bRet)
	{
		m_strPath = m_strPath.Left(m_nFolderLen) + m_fileInfo.cFileName;
	} 
	else
	{
		::FindClose(m_hFind);
		m_hFind = INVALID_HANDLE_VALUE;
	}

	return bRet;
}


//************************************************************************
// Method:    IsFinished
// Purpose:   是否完成遍历
// Access:    public 
// Returns:   BOOL		完成为TRUE，否则为FALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsFinished(void) const
{
	return (m_hFind == INVALID_HANDLE_VALUE);
}


//************************************************************************
// Method:    GetPath
// Purpose:   获取当前查找的目录
// Access:    public 
// Returns:   LPCTSTR		当前查找有目录
// Qualifier: const
// Parameter: void
//************************************************************************
LPCTSTR CBaseFileFinder::GetPath(void) const
{
	return LPCTSTR(m_strPath);
}


//************************************************************************
// Method:    GetFileInfo
// Purpose:   获取文件信息
// Access:    public 
// Returns:   const FindDataInfo&
// Qualifier: const
//************************************************************************
const FindDataInfo& CBaseFileFinder::GetFileInfo() const
{
	return m_fileInfo;
}


//************************************************************************
// Method:    IsDirectory
// Purpose:   是否为文件目录
// Access:    public 
// Returns:   BOOL		是为TRUE，否则为FALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsDirectory(void) const
{
	return (m_fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}


//************************************************************************
// Method:    IsDot
// Purpose:   是否为.目录
// Access:    public 
// Returns:   BOOL		是为TRUE，否则为FALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsDot(void) const
{
	return (m_fileInfo.cFileName[0] == '.') && ((m_fileInfo.cFileName[1] == '.') || (m_fileInfo.cFileName[1] == '\0'));
}


//************************************************************************
// Method:    IsReparsePoint
// Purpose:   是否为重解析点
// Access:    public 
// Returns:   BOOL		是为TRUE，否则为FALSE
// Qualifier: const
// Parameter: void
//************************************************************************
BOOL CBaseFileFinder::IsReparsePoint(void) const
{
	return (m_fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
}



/************************************************************************/
/* 深度优先搜索基本实现类	                                                */
/************************************************************************/
CDFSFinder::CDFSFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi)
	: CBaseFileFinder(lpszDirectory, fdi)
	, m_nFolderLevel(0)
{
}

CDFSFinder::~CDFSFinder()
{
	for (int i(0); i < m_nFolderLevel; ++i)
	{
		EnumFolderCtx& efc = m_arEnumFolderCtx[i];
		::FindClose(efc.hFind);
	}
}

BOOL CDFSFinder::FindSubDirFirst(void)
{
	if (IsDirectory() && !IsDot())
	{
		int nCurFolderLen = m_nFolderLen;
		m_nFolderLen += _tcslen(m_fileInfo.cFileName);
		BOOL bRet = FindCurDirFirst();
		if (!bRet)
		{
			m_nFolderLen = nCurFolderLen;
		}
		return bRet;
	}

	return FALSE;
}


BOOL CDFSFinder::FindParentDirNext(void)
{
	--m_nFolderLevel;
	if (m_nFolderLevel > 0)
	{
		EnumFolderCtx& efcParent = m_arEnumFolderCtx[m_nFolderLevel-1];
		m_nFolderLen = efcParent.nFolderPathLen;
		m_hFind = efcParent.hFind;
		return FindCurDirNext();
	}
	return FALSE;
}


BOOL CDFSFinder::IsFinished(void)
{
	return (CBaseFileFinder::IsFinished() && m_nFolderLevel == 0);
}


BOOL CDFSFinder::FindCurDirFirst(void)
{
	BOOL bRet = CBaseFileFinder::FindCurDirFirst();
	if (bRet)
	{
		EnumFolderCtx efc(m_hFind, m_nFolderLen);
		if (m_nFolderLevel >= static_cast<int>(m_arEnumFolderCtx.size()))
		{
			m_arEnumFolderCtx.push_back(efc);
		} 
		else
		{
			m_arEnumFolderCtx[m_nFolderLevel] = efc;
		}
		++m_nFolderLevel;
	}

	return bRet;
}



/************************************************************************/
/* 文件遍历基类                                                           */
/************************************************************************/
CFileEnumBase::CFileEnumBase()
{
}

CFileEnumBase::~CFileEnumBase()
{
	while (!m_queueFinder.empty())
	{
		CBaseFileFinder *pFinder = m_queueFinder.front();
		m_queueFinder.pop();
		DELETE_OBJECT(pFinder);
	}
}

BOOL CFileEnumBase::EnumFiles(LPCTSTR lpszDirectory, BOOL bFindSubDir, HANDLE hStopEvent)
{
	if (!lpszDirectory || !*lpszDirectory)
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	FindDataInfo fdi = {0};

#ifdef __RECURSION
	// 递归搜索
	bRet = EnumRecursively(lpszDirectory, fdi, bFindSubDir, hStopEvent);
#else
	if (bFindSubDir)
	{
#ifdef __BFS
		// 广度优先搜索
		bRet = EnumBFS(lpszDirectory, fdi, hStopEvent);
#else
		// 深度优先搜索
		bRet = EnumDFS(lpszDirectory, fdi, hStopEvent);
#endif // __BFS
	}
	else
	{
		// 不搜索子目录
		CBaseFileFinder fileFinder(lpszDirectory, fdi);
		if (!fileFinder.FindCurDirFirst())
		{
			return FALSE;
		} 

		for (; !fileFinder.IsFinished() && !IsStopEventSignaled(); fileFinder.FindCurDirNext())
		{
			const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
			if (CheckUseFile(fileFinder.GetPath(), fileInfo))
			{
				HandleFile(fileFinder.GetPath(), fileInfo);
			}
		}
	}
#endif // __RECURSION

	return bRet;
}


//************************************************************************
// Method:    CheckUseFile
// Purpose:   确认是否是需要的文件
// Access:    virtual protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
BOOL CFileEnumBase::CheckUseFile(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
	return TRUE;
}


//************************************************************************
// Method:    CheckUseDir
// Purpose:   确认是否是需要的目录
// Access:    virtual protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
BOOL CFileEnumBase::CheckUseDir(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
	return TRUE;
}


//************************************************************************
// Method:    HandleFile
// Purpose:   处理搜索到的文件
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
void CFileEnumBase::HandleFile(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
}


//************************************************************************
// Method:    HandleDir
// Purpose:   处理搜索到的目录
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
// Parameter: const FindDataInfo & fdi
//************************************************************************
void CFileEnumBase::HandleDir(LPCTSTR lpszPath, const FindDataInfo& fdi)
{
}


//************************************************************************
// Method:    FinishedDir
// Purpose:   完成当前目录的搜索后的处理
// Access:    virtual protected 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpszPath
//************************************************************************
void CFileEnumBase::FinishedDir(LPCTSTR lpszPath)
{
}

#ifdef __RECURSION
//************************************************************************
// Method:    EnumRecursively
// Purpose:   深度优先递归搜索
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// 要遍历的目录
// Parameter: FindDataInfo & fdi		// 搜索信息
// Parameter: BOOL bFindSubDir			// 是否查找子目录
// Parameter: HANDLE hStopEvent			// 停止信号量
//************************************************************************
BOOL CFileEnumBase::EnumRecursively(LPCTSTR lpszDirectory, FindDataInfo& fdi, BOOL bFindSubDir, HANDLE hStopEvent)
{
	CBaseFileFinder fileFinder(lpszDirectory, fdi);
	
	// 寻找遍历的根节点
	BOOL bRet = fileFinder.FindCurDirFirst();
	if (bRet == FALSE)
	{
		return FALSE;
	} 
	
	while (!fileFinder.IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
		// 如果不是已解析过的节点
		if (!fileFinder.IsReparsePoint())
		{
			// 如果是文件目录
			if (fileFinder.IsDirectory())
			{
				// 如果不是.和..目录
				if (!fileFinder.IsDot() && bFindSubDir)
				{
					if (CheckUseDir(fileFinder.GetPath(), fileInfo))
					{
						// 处理找到的目录
						HandleDir(fileFinder.GetPath(), fileInfo);
						bRet &= EnumRecursively(fileFinder.GetPath(), fdi, bFindSubDir);
					}
				}
			}
			else
			{
				// 如果是文件
				if (CheckUseFile(fileFinder.GetPath(), fileInfo))
				{
					// 处理找到的文件
					HandleFile(fileFinder.GetPath(), fileInfo);
				//	TRACE(_T("%s\n"), fileFinder.GetPath());
				}
			}	
		}

		// 如果找不到下一个目标时，退出查找
		if (!fileFinder.FindCurDirNext())
		{
			FinishedDir(fileFinder.GetPath());
			break;
		}
	}

	return bRet;
}

#else
#ifdef __BFS
//************************************************************************
// Method:    EnumBFS
// Purpose:   非递归广度优先搜索
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// 要遍历的目录
// Parameter: FindDataInfo & fdi		// 搜索信息
// Parameter: HANDLE hStopEvent			// 停止信号量
//************************************************************************
BOOL CFileEnumBase::EnumBFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent)
{
	CBaseFileFinder *pFinder = NULL;
	try
	{
		pFinder = new CBaseFileFinder(lpszDirectory, fdi);
	}
	catch (...)
	{
		DELETE_OBJECT(pFinder);
		return FALSE;
	}

	BOOL bRet = pFinder->FindCurDirFirst();
	if (bRet == FALSE)
	{
		DELETE_OBJECT(pFinder);
		return FALSE;
	}

	while (!pFinder->IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = pFinder->GetFileInfo();
		// 如果不是已解析过的节点
		if (!pFinder->IsReparsePoint())
		{
			// 如果是文件目录
			if (pFinder->IsDirectory())
			{
				// 如果不是.和..目录
				if (!pFinder->IsDot())
				{
					if (CheckUseDir(pFinder->GetPath(), fileInfo))
					{
						// 处理找到的目录
						HandleDir(pFinder->GetPath(), fileInfo);
					//	TRACE(_T("%s\n"), pFinder->GetPath());

						CBaseFileFinder *pNewFinder = NULL;
						try
						{
							//  添加到队列中
							pNewFinder = new CBaseFileFinder(pFinder->GetPath(), fdi);
							m_queueFinder.push(pNewFinder);
						}
						catch (...)
						{
							DELETE_OBJECT(pNewFinder);
						}
					}
				}
			} 
			else
			{
				// 如果是文件
				if (CheckUseFile(pFinder->GetPath(), fileInfo))
				{
					// 处理找到的文件
					HandleFile(pFinder->GetPath(), fileInfo);
					//TRACE(_T("%s\n"), pFinder->GetPath());
				}
			}
		}

		// 如果找不到下一个目标时，退出查找
		if (!pFinder->FindCurDirNext())
		{
			FinishedDir(pFinder->GetPath());
			if (m_queueFinder.empty())
			{
				break;
			}
			else
			{
				// 如果管理队列未空
				while (!IsStopEventSignaled())
				{
					// 取出队首元素
					CBaseFileFinder *pNextFinder = m_queueFinder.front();
					m_queueFinder.pop();

					DELETE_OBJECT(pFinder);
					pFinder = pNextFinder;
					
					// 查找第一个目录
					if (!pFinder->FindCurDirFirst())
					{
						// 查找失败，清空队列
						DELETE_OBJECT(pFinder);
						while (!m_queueFinder.empty())
						{
							pFinder = m_queueFinder.front();
							m_queueFinder.pop();
							DELETE_OBJECT(pFinder);
						}
					}
					else
					{
						break;
					}
				} // 如果管理队列未空
			}
		} // 如果找不到下一个目标时，退出查找
	}

	// 清空队列
	DELETE_OBJECT(pFinder);
	while (!m_queueFinder.empty())
	{
		pFinder = m_queueFinder.front();
		m_queueFinder.pop();
		DELETE_OBJECT(pFinder);
	}

	return bRet;
}

#else
//************************************************************************
// Method:    EnumDFS
// Purpose:   非递归深度优先搜索
// Access:    protected 
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpszDirectory		// 要遍历的目录
// Parameter: FindDataInfo & fdi		// 搜索信息
// Parameter: HANDLE hStopEvent			// 停止信号量
//************************************************************************
BOOL CFileEnumBase::EnumDFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent)
{
	CDFSFinder fileFinder(lpszDirectory, fdi);
	if (!fileFinder.FindCurDirFirst())
	{
		return FALSE;
	}

	while (!fileFinder.IsFinished() && !IsStopEventSignaled())
	{
		const FindDataInfo& fileInfo = fileFinder.GetFileInfo();
		// 如果不是已解析过的节点
		if (!fileFinder.IsReparsePoint())
		{
			// 如果是文件目录
			if (fileFinder.IsDirectory())
			{
				// 如果不是.和..目录
				if (!fileFinder.IsDot())
				{
					if (CheckUseDir(fileFinder.GetPath(), fileInfo))
					{
						// 处理找到的目录
						HandleDir(fileFinder.GetPath(), fileInfo);
						if (!fileFinder.FindSubDirFirst())
						{
							return FALSE;
						}
					}
				}
			} 
			else
			{
				// 如果是文件
				if (CheckUseFile(fileFinder.GetPath(), fileInfo))
				{
					// 处理找到的文件
					HandleFile(fileFinder.GetPath(), fileInfo);
				//	TRACE(_T("%s\n"), fileFinder.GetPath());
				}
			}
		}

		// 如果找不到下一个文件时
		if (!fileFinder.FindCurDirNext())
		{
			// 查找下一个父节点目录
			do 
			{
				FinishedDir(fileFinder.GetPath());

			} while (!fileFinder.FindParentDirNext() && !fileFinder.IsFinished() && !IsStopEventSignaled());
		}
	}

	return TRUE;
}
#endif // __BFS
#endif // __RECURSION
