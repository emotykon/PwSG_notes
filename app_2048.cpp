#include "app_2048.h" 
#include <stdexcept>
std::wstring const app_2048::s_class_name{ L"2048 Window" };//nazwa klasy
bool app_2048::register_class() {
	WNDCLASSEXW desc{};//definiuje wlasciwosci okna
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0) return true;//sprawdza czy juz nie ma takiej klasy
	desc = { .cbSize = sizeof(WNDCLASSEXW),
	.lpfnWndProc = window_proc_static,//odbiera komunikaty
	.hInstance = m_instance,
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	.hbrBackground = CreateSolidBrush(RGB(250, 047, 238)),//kolor okna
	.lpszClassName = s_class_name.c_str() };
	return RegisterClassExW(&desc) != 0;//oficjalnie zapisuje
}
HWND app_2048::create_window()
{
	return CreateWindowExW(
		0 /*empty extended styles*/,
		s_class_name.c_str(),
		L"2048",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX,//standardowe okno z menu paskiem i przy zamkniecia
		CW_USEDEFAULT, 0, /*default position*/
		CW_USEDEFAULT, 0, /*default size*/
		nullptr,
		nullptr,
		m_instance,
		this);
}
LRESULT app_2048::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app_2048* app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<app_2048*>(p->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_2048*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr)
	{
		return app->window_proc(window, message,
			wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}
LRESULT app_2048::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam)
{
	switch (message) {
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	case WM_KEYDOWN:
		switch (wparam) {
		case 'X':
			DestroyWindow(window);
			PostQuitMessage(EXIT_SUCCESS);
			return 0;
		}
		/*if (wparam == 'X') {
			DestroyWindow(window);
			PostQuitMessage(EXIT_SUCCESS);
			return 0;
		}*/
	}
	return DefWindowProcW(window, message, wparam, lparam);
}
app_2048::app_2048(HINSTANCE instance)
	: m_instance{ instance }, m_main{}
{
	register_class();
	m_main = create_window();
}
int app_2048::run(int show_command)
{
	ShowWindow(m_main, show_command);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}