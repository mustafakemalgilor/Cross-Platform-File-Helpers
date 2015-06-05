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
#include <stdlib.h>
#include <string>


class FileReader
{
public:
	FileReader(char * path);
	~FileReader();
	bool Open();
	bool Good() { return _fhandle > -1; }
	char * GetContents()	const { return buf; }
	char * GetAt(int index) const { return &buf[index]; }
	bool CopyTo(char * path);
	bool Delete();
	long   GetSize()		const { return _size; }
	void PrintError();
	bool EndOfFile(){ return eof(_fhandle); }
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
	bool Close();
	bool AllocateBuffer(int size);
	
};

/* Constructor */
FileReader::FileReader(char * path) : m_szPath(path),_size(-1),_fhandle(-1) {}

/* Destructor */
FileReader::~FileReader()
{
	Close();
}

bool FileReader::AllocateBuffer(int size)
{
	return (buf = (char*)malloc(size)) != NULL;
}

bool FileReader::Open()
{
	_fhandle = open(m_szPath, O_RDWR);
	if (Good())
		return ReadFileContent();
	
	return false;
}
bool FileReader::Close()
{
	if (Good())
	{
		free(buf);
		_size = -1;

		bool res = close(_fhandle) == 0;
		_fhandle = -1;
		return res;
	}
	else
		return false;
}
bool FileReader::ReadFileContent()
{
	struct stat stat_buf;

	if (!Good())
		return false;

	if (stat(m_szPath, &stat_buf) == 0)
	{
		if (lseek(_fhandle, 0, SEEK_SET) == -1)
			return false;
		_size = stat_buf.st_size;

		/* Check if there is enough memory to read */
		if (!AllocateBuffer(_size))
			return false;

		return read(_fhandle, buf, _size) == _size;
	}
	else
		return false;
}

bool FileReader::CopyTo(char * szPath)
{
	if (Good())
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

bool FileReader::Delete()
{
	if (Good())
	{
	
		std::string x;
		x.append(m_szPath);
		/* Close existing file handle */
		Close();
		return remove(x.c_str()) == 0;
	}
	else
		return false;
}

void FileReader::PrintError()
{
	perror("FileReader");
}







