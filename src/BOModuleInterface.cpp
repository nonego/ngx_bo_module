#include <string>
#include <iostream>
#include "BOModuleApp.h"
#include "BOModuleInterface.h"
#include "utils.h"
#include "ConfigReader.h"

using namespace bo;
using namespace std;
using namespace utils;

ContentType  g_content_type;
BOModuleApp boapp;

int BOInit( const char * conf)
{
	string strConf(conf);
	dd("BOInit conf=%s", strConf.c_str());

	ConfigReader config(conf);
	
	map<string, string> &m = config["BO"];
	string strContentType = m["content_type"];
	if( !strContentType.empty() && strContentType.size() < CONTENT_TYPE_LEN )
	{
		strncpy( g_content_type.str, strContentType.c_str(), strContentType.size() );
		g_content_type.len = strContentType.size();
	}
	else
	{
		strcpy( g_content_type.str, "text/html" );
		g_content_type.len=sizeof("text/html")-1;
	}

	if( ! boapp.init( m ) )
		return -1;

	return 0;
}


int BODestroy()
{
	dd("BODestroy");
	if( ! boapp.destroy() )
		return -1;
	
	return 0;
}

void BOFree( char ** p)
{
	dd("BOFree");
	if(*p)
	{
		free(*p);
		*p=NULL;
	}
}

int BOProcess( const char* uri, int uri_len, const char* args, int args_len, char** p, int *pn )
{
	string strUri( uri, uri+uri_len), strArgs( args, args+args_len);
	map<string, string> margs = parseQueryString(strArgs);
	string strContent = boapp.process( margs );

	*pn = strContent.size();
	*p = (char*)malloc( strContent.size() + 1 );
	if( *p == NULL )
		return -1;
	
	strncpy( *p, strContent.c_str(), strContent.size());
	(*p)[strContent.size()]=0;

	return 0;	
}


