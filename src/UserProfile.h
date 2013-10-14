#ifndef __BBL_USER_PROFILE_H__
#define __BBL_USER_PROFILE_H__
//#include <pair>
#include <string>
#include <sstream>
#include "JHMap.h"
#include "mmap.h"
#include "utils.h"

using namespace std;
using namespace utils;

class UserProfile {
private:
	JHashMap<pair<const char*, const char*> > jh;
	Mmap<char> mmap;
	vector<string> vfields;
public:
	UserProfile()
	{
	}

	~UserProfile()
	{
		mmap.close();
	}

	void setfields( const vector<string>& v)
	{
		vfields.resize(0);
		vfields.assign(v.begin(), v.end());
	}

	bool open( const char* fn )
	{
		if( !mmap.open(fn, "r") )
			return false;

		const char*p = mmap.begin(), *pend, *p0;
		while( p<mmap.end() ) {
			pend=(const char*)memchr(p, '\n', mmap.end()-p);
			if( pend == NULL )
				pend=mmap.end();

			p0 = (const char*)memchr(p, '\001', pend-p);
			if( p0 == NULL )
				continue;
							
			jh.insert( p, p0-p, make_pair(p, pend) );
			p=pend+1;
		}

		return true;
	}

	bool query( const string& s, vector<pair<const char*, const char*> >& v )
	{
		v.resize(0);
		pair<const char*, const char*> *ppair = jh.find(s.c_str(), s.size());
		if( ppair )
		{
			split( ppair->first, ppair->second, '\001', v );
			return true;
		}
		else
			return false;
	}

	string queryjson( const string& s )
	{
		vector<pair<const char*, const char*> > v;
		pair<const char*, const char*> *ppair = jh.find(s.c_str(), s.size());
		ostringstream oss;
		oss << "{ r: {";
		if( ppair )
		{
			split( ppair->first, ppair->second, '\001', v );
			for(size_t i=0; i<v.size() && i<vfields.size(); ++i )
			{
				if( i )
					oss << ",\n";
				oss << vfields[i] << ": \"" << string(v[i].first, v[i].second) << "\"";
			}
		}

		oss << "  }\n}";
		return oss.str();

	}

};

#endif
