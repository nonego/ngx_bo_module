#ifndef __OIR_MMAP_H__
#define __OIR_MMAP_H__

#include <sys/stat.h>
#include <fcntl.h>
//#include <unistd.h>
#include <sys/mman.h>
#include <string>
#include <iostream>
#include <vector>

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace {
	int open__(const char* name, int flag) { return open(name, flag); }
	int close__(int fd) { return close(fd); }
}

namespace utils{

template <class T> class Mmap {
private:
	T            *text;
	size_t       length;
	std::string  fileName;

	int    fd;
	int    flag;

public:
	T&       operator[](size_t n)       { return *(text + n); }
	const T& operator[](size_t n) const { return *(text + n); }
	T*       begin()           { return text; }
	const T* begin()    const  { return text; }
	T*       end()           { return text + size(); }
	const T* end()    const  { return text + size(); }
	size_t size()               { return length/sizeof(T); }
	const char *file_name()     { return fileName.c_str(); }
	size_t file_size()          { return length; }
	bool empty()                { return(length == 0); }

	bool open(const char *filename, const char *mode = "r") {
		this->close();
		struct stat st;

		if( filename == NULL ) {
			std::cerr << "filename is NULL" << std::endl;
			return false;
		}

		fileName = std::string(filename);

		if      (std::strcmp(mode, "r") == 0)
			flag = O_RDONLY;
		else if (std::strcmp(mode, "r+") == 0)
			flag = O_RDWR;
		else
		{
			std::cerr <<  "unknown open mode: " << mode << std::endl;
			return false;
		}

		fd = open__(filename, flag | O_BINARY);
		if( fd < 0 )
		{
			std::cerr << "open failed: " << filename << std::endl;
			return false;
		}

		if( fstat(fd, &st) < 0 )
		{
			std::cerr << "failed to get file size: " << std::endl;
			return false;
		}

		length = st.st_size;

		int prot = PROT_READ;
		if (flag == O_RDWR) prot |= PROT_WRITE;
		char *p;

		if((p = reinterpret_cast<char *>(mmap(0, length, prot, MAP_SHARED, fd, 0))) == MAP_FAILED ) 
		{
			close__(fd);
			std::cerr << "mmap() failed: " << filename << std::endl;
			return false;
		}

		text = reinterpret_cast<T *>(p);

		close__(fd);
		fd = -1;

		return true;
	}

	void close() {
		if (fd >= 0) {
			close__(fd);
			fd = -1;
		}

		if (text) {
			munmap(reinterpret_cast<char *>(text), length);
			text = 0;
		}

		text = 0;
	}

	Mmap(): text(0), fd(-1) {}

	virtual ~Mmap() { this->close(); }
};

struct TextFile{
	Mmap<char> mmap;
	char rdelim, fdelim;
	const char* cur;
	TextFile(){}
	bool open( const char* fn, char rdelim='\n', char fdelim='\001')
	{
		close();
		this->rdelim = rdelim;
		this->fdelim = fdelim;
		if( mmap.open( fn, "r" ) )
		{
			cur = mmap.begin();
			return true;
		}
		return false;
	}

	void close()
	{
		mmap.close();
	}

	bool next( std::vector<std::pair<const char*, const char*> >& v )
	{
		v.resize(0);
		if( cur >= mmap.end() )
			return false;

		const char* pend = (const char*)memchr( cur, rdelim, mmap.end()-cur );
		if( pend == NULL )
			pend = cur;
		if( pend == cur )
			return false;

		if( pend > mmap.end() )
			pend = mmap.end();
		
		const char* p = (const char*)memchr( cur, fdelim, pend-cur );
		while( p != NULL && p < pend )
		{
			v.push_back( std::make_pair(cur, p) );
			cur = p + 1;
			p = (const char*)memchr( cur, fdelim, pend-cur );
		}
		if( cur < pend )
			v.push_back( std::make_pair(cur, pend) );

		cur = pend + 1;
		return true;
	}
};

typedef std::vector<std::pair<const char*, const char*> > TFRecord;

} // end of namespace

#endif
