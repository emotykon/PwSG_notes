#define WIN32_LEAN_AND_MEAN //wyklucza rzadko u¿ywane czesci biblioteki
#define NOMINMAX //konflikt z c++ wyklucza
#include <windows.h> //bib winapi
#include "app_2048.h" // biblioteka nasza
int WINAPI wWinMain(HINSTANCE instance/*naszainstancja*/,
	HINSTANCE /*starezawszenull*/,
	LPWSTR /*argumentyprogramu*/,
	int show_command/*sposóbwyswietlania*/)
{
	app_2048 app{ instance };//tworzy wszystko
	return app.run(show_command);//przetwarza wiadomosci
}