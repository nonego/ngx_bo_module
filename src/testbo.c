#include "BOModuleInterface.h"
#include "stdio.h"

int main( int argc, char* argv[])
{
	BOInit( "test");
	printf("%s\n%d", g_content_type.str, g_content_type.len);

	return 0;
}
