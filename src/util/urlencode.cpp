/* the original code was taken from wget-1.10.2 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <glib.h>
#include <ctype.h>
#include "urlencode.h"

using namespace std;

namespace utils{

enum {
  /* rfc1738 reserved chars + "$" and ",".  */
  urlchr_reserved = 1,

  /* rfc1738 unsafe chars, plus non-printables.  */
  urlchr_unsafe   = 2
};

#define urlchr_test(c, mask) (urlchr_table[(unsigned char)(c)] & (mask))
#define URL_RESERVED_CHAR(c) urlchr_test(c, urlchr_reserved)
#define URL_UNSAFE_CHAR(c) urlchr_test(c, urlchr_unsafe)
#define XNUM_TO_DIGIT(x) ("0123456789ABCDEF"[x] + 0)
#define ISXDIGIT(x) (isxdigit((unsigned char)(x)))
#define X2DIGITS_TO_NUM(h1, h2) ((XDIGIT_TO_NUM (h1) << 4) + XDIGIT_TO_NUM (h2))
#define XDIGIT_TO_NUM(h) ((h) < 'A' ? (h) - '0' : toupper (h) - 'A' + 10)

/* Shorthands for the table: */
#define R  urlchr_reserved
#define U  urlchr_unsafe
#define RU R|U

static const unsigned char urlchr_table[256] =
{
  U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
  U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
  U,  0,  U, RU,   R,  U,  R,  0,   /* SP  !   "   #    $   %   &   '   */
  0,  0,  0,  R,   R,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
  0,  0, RU,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
 RU,  0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
  0,  0,  0, RU,   U, RU,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
  U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
  0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
  0,  0,  0,  U,   U,  U,  0,  U,   /* x   y   z   {    |   }   ~   DEL */

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,

  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
  U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};
#undef R
#undef U
#undef RU

/* URL-unescape the string S.

   This is done by transforming the sequences "%HH" to the character
   represented by the hexadecimal digits HH.  If % is not followed by
   two hexadecimal digits, it is inserted literally.

   The transformation is done in place.  If you need the original
   string intact, make a copy before calling this function.  */

string urldecode (const char *s)
{
    char *copy = strdup(s);
    char *t = copy;                     /* t - tortoise */
    char *h = copy;                     /* h - hare     */

    for (; *h; h++, t++)
    {
        if (*h != '%')
        {
        copychar:
            *t = *h;
        }
        else
        {
            char c;
            /* Do nothing if '%' is not followed by two hex digits. */
            if (!h[1] || !h[2] || !(ISXDIGIT (h[1]) && ISXDIGIT (h[2])))
                goto copychar;
            c = X2DIGITS_TO_NUM (h[1], h[2]);
            /* Don't unescape %00 because there is no way to insert it
               into a C string without effectively truncating it. */
            if (c == '\0')
                goto copychar;
            *t = c;
            h += 2;
        }
    }
    *t = '\0';
	string rlt = copy;
	free( copy );
	
  return rlt;
}

/* The core of url_escape_* functions.  Escapes the characters that
   match the provided mask in urlchr_table.

   If ALLOW_PASSTHROUGH is non-zero, a string with no unsafe chars
   will be returned unchanged.  If ALLOW_PASSTHROUGH is zero, a
   freshly allocated string will be returned in all cases.  */

string url_escape_1 (const char *s, unsigned char mask, int allow_passthrough)
{
  const char *p1;
  char *p2, *newstr;
  int newlen;
  int addition = 0;

  for (p1 = s; *p1; p1++)
    if (urlchr_test (*p1, mask))
      addition += 2;            /* Two more characters (hex digits) */

  if (!addition)
	return string(s);
    //return allow_passthrough ? (char *)s : strdup (s);

  newlen = (p1 - s) + addition;
  newstr = (char *)malloc (newlen + 1);

  p1 = s;
  p2 = newstr;
  while (*p1)
    {
      /* Quote the characters that match the test mask. */
      if (urlchr_test (*p1, mask))
        {
          unsigned char c = *p1++;
          *p2++ = '%';
          *p2++ = XNUM_TO_DIGIT (c >> 4);
          *p2++ = XNUM_TO_DIGIT (c & 0xf);
        }
      else
        *p2++ = *p1++;
    }
  //g_return_if_fail (p2 - newstr == newlen);
  *p2 = '\0';

  string rlt = newstr;
  free( newstr );
  return rlt;
}

/* URL-escape the unsafe characters (see urlchr_table) in a given
   string, returning a freshly allocated string.  */

string urlencode (const char *s)
{
  return url_escape_1 (s, urlchr_unsafe, 0);
}

}//end of namespace utils

#ifdef TEST_URLENCODE
#include <string>
#include <iostream>

using namespace std;
using namespace utils;
int main( int argc, char* argv[])
{
	string s;
	while( getline(cin, s ) )
		cout << urlencode( s.c_str() ) << endl;
	
	return 0;
}

#endif

#ifdef TEST_URLDECODE
#include <string>
#include <iostream>

using namespace std;
using namespace utils;
int main( int argc, char* argv[])
{
	string s;
	while( getline( cin, s ) )
		cout << urldecode( s.c_str() ) << endl;
	
	return 0;
}

#endif
