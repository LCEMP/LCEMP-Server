#pragma once
#include <cwchar>

#ifndef _T
#define _T(x) L##x
#endif
#ifndef _TCHAR_DEFINED
typedef wchar_t TCHAR;
#define _TCHAR_DEFINED
#endif
#ifndef _tcslen
#define _tcslen wcslen
#endif
#ifndef _tcscpy
#define _tcscpy wcscpy
#endif
#ifndef _tcsncpy
#define _tcsncpy wcsncpy
#endif
