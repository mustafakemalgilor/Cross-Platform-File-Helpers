#pragma once


#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "FileReader.h"

#ifndef WIN32
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/types.h>
#else
	#include <io.h>
#endif

#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

class FileBuilder
{
public:
	FileBuilder(char * ,bool);
	~FileBuilder();
	void PrintError();
	bool Create();
	bool Append(char * , long );
	bool Append(FileReader&);
private:
	bool _overwrite;
	/* File handle */
	int		_fhandle;
	/* File size */
	long	_size;
	/* File path */
	char *	m_szPath;
};

FileBuilder::FileBuilder(char * szPath, bool overwrite = true) :m_szPath(szPath), _size(-1), _fhandle(-1), _overwrite(overwrite)
{
}

bool FileBuilder::Create()
{
	if (!_overwrite)
		_fhandle = open(m_szPath, O_CREAT | O_EXCL | O_RDWR, 0777);
	else
		_fhandle = open(m_szPath, O_CREAT | O_TRUNC | O_RDWR, 0777);
	if (_fhandle > -1)
	{
		
		if (!_overwrite && errno == EEXIST)
			return false;
		return true;
	}
	return false;
	
}

bool FileBuilder::Append(char * val, long size)
{
	if (_fhandle == -1)
		return false;
	lseek(_fhandle, 0, SEEK_END);
	return write(_fhandle, val, size) == size;
}

bool FileBuilder::Append(FileReader& reader)
{
	if (_fhandle == -1)
		return false;

	/* Seek to the end (append) */
	if (lseek(_fhandle, 0, SEEK_END) == -1)
		return false;

	if (!reader.Good())
		return false;

	return write(_fhandle, reader.GetContents(), reader.GetSize()) == reader.GetSize();
}

void FileBuilder::PrintError()
{
	perror("FileBuilder");
}


FileBuilder::~FileBuilder()
{
	if (_fhandle != -1)
		close(_fhandle);
}

