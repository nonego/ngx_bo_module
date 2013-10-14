#ifndef __BO_MODULE_INTERFACE_H__
#define __BO_MODULE_INTERFACE_H__

#if defined(DEBUG)

#		define dd(...) fprintf(stdout, "BOModule *** %s: ", __func__); \
            fprintf(stdout, __VA_ARGS__); \
            fprintf(stdout, " at %s line %d.\n", __FILE__, __LINE__)

#else

#		define dd(...)

#endif

#define CONTENT_TYPE_LEN 256

struct ContentType
{
	char str[CONTENT_TYPE_LEN];
	int len;
};

extern struct ContentType g_content_type;


#ifdef __cplusplus
extern "C" {
#endif

int BOInit( const char * conf );
int BODestroy();
void BOFree( char ** p);
int BOProcess( const char* uri, int uri_len, const char* args, int args_len, char** p, int *pn );

#ifdef __cplusplus
}
#endif

#endif
