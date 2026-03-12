#pragma once
#include <windows.h>
#include <string>
class app_2048
{
private:
	bool register_class(); ///zapisuje klase
	static std::wstring const s_class_name;//nazwa okna
	static LRESULT CALLBACK window_proc_static(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);//statyczna obs³uga zdarzeñ - posrednikC-C++
	LRESULT window_proc(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);//w³asciwa obsluga komunikatow
	HWND create_window();//tworzy okno
	HINSTANCE m_instance;//uchwyt do obecnej instancji aplikacji
	HWND m_main;//uchwyt do okna
public:
	app_2048(HINSTANCE instance);//konstruktor klasy
	int run(int show_command);//petlakomunikatow
};