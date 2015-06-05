#pragma once

#ifndef WIN32
	#include <stdio.h>
	#include <unistd.h>
	#include <sys/types.h>
#else
	#include <io.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>


class FileReader
{
public:
	FileReader(char * path);
	~FileReader();
	bool Open();
	char * GetContents()	const { return buf; }
	char * GetAt(int index) const { return &buf[index]; }
	bool CopyTo(char * path);
	long   GetSize()		const { return _size; }
	void PrintError();
private:
	/* File handle */
	int		_fhandle;
	/* File size */
	long	_size;
	/* File path */
	char *	m_szPath;
	/* File content */
	char *	buf;
	bool ReadFileContent();
};

/* Constructor */
FileReader::FileReader(char * path) : m_szPath(path),_size(-1),_fhandle(-1) {}

/* Destructor */
FileReader::~FileReader()
{
	if (_fhandle != -1)
	{
		delete[] buf;
		close(_fhandle);
	}
}

bool FileReader::Open()
{
	_fhandle = open(m_szPath, O_RDWR);

	if (_fhandle > -1)
	{
		if (!ReadFileContent())
			return false;
		return true;
	}
	return false;
}

bool FileReader::ReadFileContent()
{
	if (_fhandle == -1)
		return false;
	struct stat stat_buf;
	int rc = stat(m_szPath, &stat_buf);
	if (rc == 0)
	{
		_size = stat_buf.st_size;
		buf = new char[_size];
		read(_fhandle, &buf[0], _size);
		return true;
	}
	else
		return false;
}

bool FileReader::CopyTo(char * szPath)
{
	if (_fhandle > -1)
	{
		int _fhandle2 = open(szPath, O_CREAT | O_EXCL | O_RDWR);
		if (_fhandle2 > -1)
		{
			if (write(_fhandle2, GetContents(), GetSize()) == GetSize())
			{
				close(_fhandle2);
				return true;
			}
			close(_fhandle2);
		}
		return false;
	}
	return false;
}

void FileReader::PrintError()
{
	perror("FileReader");
}







