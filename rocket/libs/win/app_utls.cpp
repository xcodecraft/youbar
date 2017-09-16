#include "common.h"
#include "win_utls/app_utls.h"

yyou::app_sigeton::app_sigeton( const wchar_t *name )
{
	_have_run =false;
	_mutex = CreateMutex(NULL, FALSE,name);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		_have_run =true;
	}
}

yyou::app_sigeton::~app_sigeton()
{
	CloseHandle(_mutex);
}

bool yyou::app_sigeton::have_instance()
{
	return _have_run;
}
