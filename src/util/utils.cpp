#include "utils.h"
#include "urlencode.h"

namespace utils {

map<string, string> parseQueryString( const string& s)
{
	map<string, string> args;
	vector<string> v = split( '&', s );
	args.clear();
	for( size_t i=0;i<v.size(); ++i )
	{
		size_t nPos = v[i].find('=');
		string str;
		if( nPos != string::npos  )
		{
			str = utils::urldecode( v[i].c_str() + nPos + 1 );
			args.insert( make_pair( v[i].substr(0, nPos), str ) );
		}
	}

	return args;
}

vector<string> split(char delimiter, const string &source)
{
	size_t prev_pos = 0, pos = 0;

	vector<string> result;
	result.reserve(count(source.begin(), source.end(), delimiter) + 1);
	pos = source.find(delimiter, pos);
	while (pos != string::npos) {
		result.push_back(source.substr(prev_pos, pos - prev_pos));
		prev_pos = ++ pos;
		pos = source.find(delimiter, pos);
	}
	result.push_back(source.substr(prev_pos));

	return result;
}

bool split( const char* beg, const char* end, char delim, std::vector<std::pair<const char*, const char*> >& v )
{
	v.resize(0);
	if(end<beg)
		return false;
	
	int len = end-beg;
	const char* p= NULL;
	while((p = (const char*)memchr( beg, delim, len )) != NULL )
	{
		v.push_back(std::make_pair(beg, p));
		beg=p+1;
		len=end-beg;
	}
	v.push_back(std::make_pair(beg, end));
	
	return true;
}

}
