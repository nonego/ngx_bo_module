#include "BOModuleApp.h"

namespace bo {

bool BOModuleApp::init( const map<string, string>& args)
{
	map<string, string>::const_iterator it = args.find("profile_file");
	if( it == args.end() || !profile.open( it->second.c_str() ) )
		return false;
	
	it = args.find("profile_fields");
	vector<string> v = split( ' ', it==args.end()? "" : it->second );
	profile.setfields( v );
			
	return true;
}

bool BOModuleApp::destroy()
{
	return true;
}

string BOModuleApp::process( const map<string, string>& args)
{
	map<string, string>::const_iterator it = args.find("id");
	return profile.queryjson( it==args.end()? "": it->second );
}

}
