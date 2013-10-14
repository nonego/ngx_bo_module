#include <map>
#include <string>
#include <vector>

using namespace std;

namespace utils {
	map<string, string> parseQueryString( const string& s);
	vector<string> split(char delimiter, const string &source);
	bool split( const char* beg, const char* end, char delim, std::vector<std::pair<const char*, const char*> >& v );

}
