/**
 * @file JMap.h
 * @brief
 * @author qing.guojq
 * @date 2011-4-12
 */
#ifndef __JMAP_H__
#define __JMAP_H__

#include <vector>
#include "Judy.h"
#define MAXLEN 102400

#define PCU8(x) (const uint8_t*)x
#define PU8(x) (uint8_t*)x

using std::vector;

template<class V>
class JHashMap {
private:
	Pvoid_t m_judy;
	vector<V*> m_v;
public:
	JHashMap():m_judy(NULL){}
	
	~JHashMap(){ 
		close();
	}

	void close() {
		//Word_t w;
		long w=0;
		JHSFA( w, m_judy );
		for( size_t i=0; i< m_v.size(); ++i )
			delete m_v[i];
		m_v.clear();
	}

	/**
	 * @brief erase an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	int erase( const char* k, int count )
	{
		int r;
		JHSD( r, m_judy, PCU8(k), count );
		return r;
	}

	/**
	 * @brief find an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	V* find( const char * k, int count )
	{
		Pvoid_t pvalue;
		JHSG( pvalue, m_judy, PU8(k), count );
		if( pvalue != NULL && *(PWord_t)pvalue < m_v.size() )
			return m_v[ *(PWord_t)pvalue ];
		return NULL;
	}

	/**
	 * @brief insert an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	V* insert( const char* k, int count, const V& v )
	{
		Pvoid_t pvalue;
		JHSI( pvalue, m_judy, PU8(k), count );
		if( pvalue )
		{
			V* pv = new V(v);
			*(PWord_t)pvalue = m_v.size();
			m_v.push_back( pv );
			return pv;
		}
		return NULL;
	}

};

template <>
class JHashMap<Word_t> {
private:
	Pvoid_t judy;
public:
	JHashMap():judy(NULL){}
	
	~JHashMap(){ 
		//Word_t w;
		close();
	}

	void close()
	{
		long w;
		JHSFA( w, judy );
	}

	/**
	 * @brief erase an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	int erase( const char* k, int count )
	{
		int r;
		JHSD( r, judy, PU8(k), count );
		return r;
	}
	
	/**
	 * @brief find an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	PWord_t find( const char * k, int count )
	{
		Pvoid_t pvalue;
		JHSG( pvalue, judy, PU8(k), count );
		return (PWord_t)pvalue;
	}

	/**
	 * @brief insert an element
	 * @param
	 * @author qing.guojq
	 * @date 2011-4-12
	 */
	PWord_t insert( const char* k, int count, Word_t v )
	{
		Pvoid_t pvalue;
		JHSI( pvalue, judy, PU8(k), count );
		if( pvalue )
			*(PWord_t)pvalue = v;
		return (PWord_t)pvalue;
	}
};

typedef JHashMap<Word_t> JHMap;

#endif

