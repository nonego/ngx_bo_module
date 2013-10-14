/* prototypes */
#ifndef __URL_ENCODE_H__
#define __URL_ENCODE_H__
#include <string>

namespace utils{

std::string urldecode(const char *url);
std::string urlencode(const char *path);

inline std::string urldecode(const std::string& s)
{
	return urldecode( s.c_str() );
}
inline std::string urlencode(const std::string& s)
{
	return urlencode( s.c_str() );
}

}

#endif
