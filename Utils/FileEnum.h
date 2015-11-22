/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/17 19:26
	FileName: 	FileEnum.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	遍历搜索文件目录
*********************************************************************/
#pragma once
#include <queue>


// 非递归广度优先搜索
#ifndef __RECURSION
#define __BFS
#endif // !__RECURSION

#define IsStopEventSignaled()		(::WaitForSingleObject(hStopEvent, 0) == WAIT_OBJECT_0)
#define DELETE_OBJECT(_pObject)		{if(_pObject) {delete (_pObject); (_pObject) = NULL;}}

typedef WIN32_FIND_DATA		FindDataInfo, *PFindDataInfo;


/************************************************************************/
/* 文件搜索基本实现类                                                      */
/************************************************************************/
class CBaseFileFinder
{
public:
	CBaseFileFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi);
	virtual ~CBaseFileFinder(void);

protected:
	FindDataInfo&	m_fileInfo;			// 文件信息
	CString			m_strPath;			// 当前目录
	UINT			m_nFolderLen;		// 路径长度
	HANDLE			m_hFind;

protected:
	void Init(LPCTSTR lpszDirectory);

public:
	BOOL FindCurDirFirst(void);
	BOOL FindCurDirNext(void);
	BOOL IsFinished(void) const;
	LPCTSTR GetPath(void) const;
	const FindDataInfo& GetFileInfo() const;
	BOOL IsDirectory(void) const;
	BOOL IsDot(void) const;
	BOOL IsReparsePoint(void) const;
};


/************************************************************************/
/* 深度优先搜索基本实现类	                                                */
/************************************************************************/
class CDFSFinder : public CBaseFileFinder
{
public:
	CDFSFinder(LPCTSTR lpszDirectory, FindDataInfo& fdi);
	~CDFSFinder();

protected:
	struct EnumFolderCtx
	{
		HANDLE	hFind;
		int		nFolderPathLen;

		EnumFolderCtx(HANDLE _hFind = NULL, int _nFolderPathLen = -1)
			: hFind(_hFind)
			, nFolderPathLen(_nFolderPathLen)
		{
		}
	};

protected:
	int							m_nFolderLevel;			// 文件深度
	std::vector<EnumFolderCtx>	m_arEnumFolderCtx;		// 节点管理栈

public:
	BOOL FindSubDirFirst(void);
	virtual BOOL FindParentDirNext(void);
	BOOL IsFinished(void);
	BOOL FindCurDirFirst(void);
};


/************************************************************************/
/* 文件遍历基类                                                           */
/************************************************************************/
class CFileEnumBase
{
public:
	CFileEnumBase();
	virtual ~CFileEnumBase();

protected:
	std::queue<CBaseFileFinder *>	m_queueFinder;		// 查找信息管理队列

public:
	virtual BOOL EnumFiles(LPCTSTR lpszDirectory, BOOL bFindSubDir = TRUE, HANDLE hStopEvent = NULL);

protected:
	virtual BOOL CheckUseFile(LPCTSTR /*lpszPath*/, const FindDataInfo& /*fdi*/);
	virtual BOOL CheckUseDir(LPCTSTR /*lpszPath*/, const FindDataInfo& /*fdi*/);
	virtual void HandleFile(LPCTSTR lpszPath, const FindDataInfo& /*fdi*/);
	virtual void HandleDir(LPCTSTR /*lpszPath*/, const FindDataInfo& /*fdi*/);
	virtual void FinishedDir(LPCTSTR /*lpszPath*/);

#ifdef __RECURSION
	BOOL EnumRecursively(LPCTSTR lpszDirectory, FindDataInfo& fdi, BOOL bFindSubDir = TRUE, HANDLE hStopEvent = NULL);
#else
#ifdef __BFS
	BOOL EnumBFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent = NULL);
#else
	BOOL EnumDFS(LPCTSTR lpszDirectory, FindDataInfo& fdi, HANDLE hStopEvent = NULL);
#endif // __BFS
#endif // __RECURSION	
};

