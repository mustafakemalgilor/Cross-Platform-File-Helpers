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
#include <vector>

class FileWriter
{
public:
	FileWriter(char *, bool);
	~FileWriter();
	bool Open();
	bool Good(){ return _fhandle > -1; }
	bool Append(char *, int);
	bool Append(FileReader& reader);
	bool Append(int);

	bool Write(char *,int ,int,bool);
	bool Delete();
	void PrintError();
private:
	/* File handle */
	int		_fhandle;
	/* File path */
	char *	m_szPath;
	bool createFlag;
	int GetSize();
	bool Close();
};

FileWriter::FileWriter(char * szPath, bool createIfNotExist = false) : m_szPath(szPath), _fhandle(-1), createFlag(createIfNotExist)
{
	
}


FileWriter::~FileWriter()
{
	Close();
}

bool FileWriter::Close()
{
	if (_fhandle > -1)
	{
		
		m_szPath = nullptr;
		bool res = close(_fhandle) == 0;
		_fhandle = -1;
		return res;
	}
	else
		return false;
}

bool FileWriter::Open()
{
	/* File is already open */
	if (Good())
		return false;
	return createFlag ? (_fhandle = open(m_szPath, O_CREAT | O_EXCL)) : (_fhandle = open(m_szPath, O_WRONLY)) > -1;
}

int FileWriter::GetSize()
{
	if (!Good())
		return false;
	struct stat stat_buf;
	return stat(m_szPath, &stat_buf) == 0 ? stat_buf.st_size : -1;
}

bool FileWriter::Append(FileReader& reader)
{
	if (!Good())
		return false;

	/* Seek to the end (append) */
	if (lseek(_fhandle, 0, SEEK_END) == -1)
		return false;

	if (!reader.Good())
		return false;

	return write(_fhandle, reader.GetContents(), reader.GetSize()) == reader.GetSize();
}

bool FileWriter::Append(int fd)
{
	if (!Good() || fd == -1)
		return false;

	/* Seek to the end (append) */
	if (lseek(_fhandle, 0, SEEK_END) == -1)
		return false;

	struct stat stat_buf;
	int rc = stat(m_szPath, &stat_buf);
	if (rc == 0)
	{
		int _size = lseek(fd, 0, SEEK_END);
		char *buf = new char[_size];
		lseek(fd, 0, SEEK_SET);
		if (read(_fhandle, &buf[0], _size) != _size)
		{
			delete[] buf;
			return false;
		}
		
		bool result = Append(buf, _size);
		delete[] buf;
		return result;
	}
	else
		return false;
	


}

bool FileWriter::Append(char * val, int len)
{
	if (!Good())
		return false;

	/* Seek to the end (append) */
	if (lseek(_fhandle, 0, SEEK_END) == -1)
		return false;

	return write(_fhandle, val, len) == len;
}

bool FileWriter::Write(char *val, int len, int startIndex,bool canExceed = true)
{
	if (!Good())
		return false;

	int fileLen = GetSize();

	/* Check either if
	*		- Start index exceeds file boundaries
	*		- End index exceeds file boundaries	(if canExceed is false)
	*/
	if (fileLen - 1 > startIndex || (!canExceed & (startIndex + len > (fileLen - 1))))
		return false;

	/* Check if we can seek to the specified starting index */
	if (lseek(_fhandle, startIndex, SEEK_SET) == -1)
		return false;

	return write(_fhandle, val, len) == len;
}

bool FileWriter::Delete()
{
	if (_fhandle > -1)
	{
		/* Close existing file handle */
		Close();
		return remove(m_szPath) == 0;
	}
	else
		return false;
}

void FileWriter::PrintError()
{
	perror("FileWriter");
}



