#ifndef __BO_MODULE_APP__
#define __BO_MODULE_APP__
#include <string>
#include <map>
#include "UserProfile.h"

using namespace std;

namespace bo {

class BOModuleApp{
private:
	UserProfile profile;

public:
	string process( const map<string, string>& args);
	bool init( const map<string, string>& args);
	bool destroy();
};

}

#endif
