#pragma once
#include <windows.h>
#include <string>
#include <vector>
class app_2048
{
private:
	bool register_class(); ///zapisuje klase
	static std::wstring const s_class_name;//nazwa okna
	static LRESULT CALLBACK window_proc_static(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);//statyczna obsluga zdarzen - posrednikC-C++
	LRESULT window_proc(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);//wlasciwa obsluga komunikatow
	HWND create_window();//tworzy okno
	HINSTANCE m_instance;//uchwyt do obecnej instancji aplikacji
	HWND m_main;//uchwyt do okna
	std::vector<HWND> m_digits;
	float m_angle;
	bool m_is_spinning;
	int m_mouseX, m_mouseY;
	int m_lastX[10], m_lastY[10];
public:
	app_2048(HINSTANCE instance);//konstruktor klasy
	int run(int show_command);//petlakomunikatow
};