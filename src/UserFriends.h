#ifndef __USER_FRIENDS_H__
#define __USER_FRIENDS_H__

#include <string>
#include "JHMap.h"
#include "mmap.h"

using namespace std;
using namespace oir;


struct Friends{	
	uint32_t id;
	uint32_t n;
	uint32_t friends[];
};

class UserFriends
{
private:
	JHashMap<pair<const Friends*, const Friends*> > jh;
	JHMap jhid;
	JHashMap<string> jhname;

	Mmap<char> mm1d, mm2d;
public:
	UserFriends()
	{
	}

	~UserFriends()
	{
		close();
	}

	void close()
	{
		jh.close();
		jhid.close();
		jhname.close();
		mm1d.close();
		mm2d.close();
	}

	void transfer(const vector<uint32_t>& v, vector<string>& vcomm )
	{
		vcomm.reserve(v.size());
		for( size_t i=0; i<v.size(); ++i )
		{
			string *pstr = jhname.find((const char*)&v[i], sizeof(uint32_t) );
			if( !pstr )
				continue;

			vcomm.push_back( *pstr );
		}
	}

	int getcommonfriends(const string& u1, const string& u2, vector<string>& vcomm)
	{
		vcomm.resize(0);
		pair<const Friends*, const Friends*> *ppair1, *ppair2;
		ppair1=jh.find( u1.c_str(), u1.size() );
		ppair2=jh.find( u2.c_str(), u2.size() );

		if( ppair1 == NULL || ppair2 == NULL || ppair1->first== NULL || ppair2->first == NULL)
			return 0;

		vector<uint32_t> vids;
		set_intersection( ppair1->first->friends, ppair1->first->friends+ppair1->first->n, 
											ppair2->first->friends, ppair2->first->friends+ppair2->first->n, back_inserter( vids ) );
		
		if( * lower_bound( ppair1->first->friends, ppair1->first->friends+ppair1->first->n, ppair2->first->id) == ppair2->first->id )
		{
			transfer(vids, vcomm);
			return 1;	
		}

		if( !vids.empty() )
		{
			transfer(vids, vcomm);
			return 2;
		}

		if( !ppair1->second )
			return 0;

		set_intersection( ppair1->second->friends, ppair1->second->friends + ppair1->second->n, 
											ppair2->first->friends, ppair2->first->friends+ppair2->first->n, back_inserter( vids ) );
		if( !vids.empty() )
		{
			transfer(vids, vcomm);
			return 3;
		}
		
		if( !ppair2->second)
			return 0;
		set_intersection( ppair1->second->friends, ppair1->second->friends + ppair1->second->n, 
											ppair2->second->friends, ppair2->second->friends + ppair2->second->n, back_inserter( vids ) );

		if( !vids.empty() )
		{
			transfer(vids, vcomm);
			return 4;
		}

		return 0;
	}

	bool open( const char* fn1d, const char* fn2d, const char* fnid )
	{
		close();

		TextFile file;
		if( !file.open(fnid) )
			return false;

		TFRecord r;
		while( file.next(r) )
		{
			if( r.size() < 2 )
				continue;

			uint32_t uid = atoi(string(r[1].first, r[1].second).c_str());
			jhid.insert( r[0].first, r[0].second-r[0].first, uid );
			string str(r[0].first, r[0].second);
			jhname.insert( (const char*) &uid, sizeof(uint32_t), string(r[0].first, r[0].second) );
		}
		file.close();

		const uint32_t*p;
		if( !mm1d.open(fn1d, "r") )
			return false;
		if( !mm2d.open(fn2d, "r") )
			return false;
		
		p = (const uint32_t*) mm1d.begin();
		while( p+2 <= (const uint32_t*) mm1d.end() )
		{
			const Friends* pf = (const Friends*) p;
			string* pstr = jhname.find( (const char*) &pf->id, sizeof(uint32_t) );
			if( !pstr )
				continue;

			pair<const Friends*, const Friends*> *ppair = jh.find( pstr->c_str(), pstr->size() );
			if( !ppair )
				jh.insert( pstr->c_str(), pstr->size(), make_pair( pf, (const Friends*) NULL) );
			else
				ppair->first = pf;
			
			p += pf->n + 2;
		}
		
		p = (const uint32_t*) mm2d.begin();
		while( p+2 <= (const uint32_t*) mm2d.end() )
		{
			const Friends* pf = (const Friends*) p;
			string* pstr = jhname.find( (const char*) &pf->id, sizeof(uint32_t) );
			if( !pstr )
				continue;

			pair<const Friends*, const Friends*> *ppair = jh.find( pstr->c_str(), pstr->size() );
			if( !ppair )
				jh.insert( pstr->c_str(), pstr->size(), make_pair( (const Friends*) NULL, pf ) );
			else
				ppair->second = pf;
			
			p += pf->n + 2;
		}
		
		return true;
		
	}

};

#endif
