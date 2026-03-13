#include "app_2048.h" 
#include <stdexcept>
#include <vector>
#include <cmath>

// ui definitions
#define ID_MENU_NEWGAME 2001
#define ID_MENU_EXIT    2002
#define ID_MENU_ABOUT   2003
#define ID_BUTTON_RESET 2004
#define ID_EDIT_NAME    2005
#define ID_TEST         2006
#define ID_TIMER_ANIM   3001

std::wstring const app_2048::s_class_name{ L"2048 Window" };
// class
bool app_2048::register_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0)
		return true;

	desc = {
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = window_proc_static,
		.hInstance = m_instance,
		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground = CreateSolidBrush(RGB(250, 47, 238)),
		.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}
// class creation
HWND app_2048::create_window()
{
	return CreateWindowExW(
		0,
		s_class_name.c_str(),
		L"2048",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX,
		CW_USEDEFAULT, 0,
		800, 800,
		nullptr,
		nullptr,
		m_instance,
		this);
}
// strange do not touch
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
		SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_2048*>(GetWindowLongPtrW(window, GWLP_USERDATA));
	}

	if (app != nullptr)
	{
		return app->window_proc(window, message, wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}
// main input
LRESULT app_2048::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam)
{
	switch (message) {

	case WM_CREATE: {
		// menu
		HMENU hMenu = CreateMenu();
		HMENU hFileMenu = CreatePopupMenu();
		AppendMenuW(hFileMenu, MF_STRING, ID_MENU_NEWGAME, L"Nowa Gra");
		AppendMenuW(hFileMenu, MF_SEPARATOR, 0, NULL);
		AppendMenuW(hFileMenu, MF_STRING, ID_MENU_EXIT, L"Wyjście");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"Plik");

		HMENU hHelpMenu = CreatePopupMenu();
		AppendMenuW(hHelpMenu, MF_STRING, ID_MENU_ABOUT, L"O autorze2");
		AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Pomoc1");

		AppendMenuW(hMenu, MF_STRING, ID_TEST, L"1test");

		SetMenu(window, hMenu);

		// button
		CreateWindowW(L"BUTTON", L"Resetuj",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			10, 10, 80, 30, window, (HMENU)ID_BUTTON_RESET,
			(HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE), NULL);

		// text field
		CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Gracz",
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			100, 10, 120, 30, window, (HMENU)ID_EDIT_NAME,
			(HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE), NULL);

		CreateWindowW(L"BUTTON", L"Check",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER | WS_SYSMENU,
			200, 200, 70, 30, window, (HMENU)ID_BUTTON_RESET,
			(HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE), NULL);

		for (int i = 0; i < 10; i++) {
			HWND hDigit = CreateWindowW(L"BUTTON", std::to_wstring(i).c_str(),
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
				0, 0, 40, 40, window, (HMENU)(5000 + i), m_instance, NULL);
			m_digits.push_back(hDigit);
		}
		SetTimer(window, ID_TIMER_ANIM, 16, NULL);

		return 0;
	}
				  //
	case WM_COMMAND: {
		int wmId = LOWORD(wparam);

		if (wmId >= 5000 && wmId <= 5009) {
			m_is_spinning = !m_is_spinning;
			return 0;
		}

		switch (wmId) {
		case ID_MENU_NEWGAME:
			break;
		case ID_TEST:
		{
			int result = MessageBoxW(window, L"Tekst", L"Tytuł", MB_ABORTRETRYIGNORE | MB_ICONQUESTION |
				MB_SETFOREGROUND);
			if (result == IDIGNORE) {
				MessageBoxW(window, L"Kliknąłeś Ignoruj!", L"Info", MB_OK);
			}
			else if (result == IDABORT) {
			}

			break;
		}
		case ID_BUTTON_RESET:
		{
			wchar_t name[50];
			GetDlgItemTextW(window, ID_EDIT_NAME, name, 50);

			std::wstring msg = L"Gra zresetowana dla: ";
			msg += name;

			MessageBoxW(window, msg.c_str(), L"Info", MB_OK);
			break;
		}
		case ID_MENU_EXIT:
			DestroyWindow(window);
			break;

		case ID_MENU_ABOUT:
			MessageBoxW(window, L"Gra 2048 - Projekt PWSG\nAutor: [Twoje Imię]", L"O programie", MB_ABORTRETRYIGNORE | MB_OK | MB_ICONINFORMATION);
			break;
		}
		return 0;
	}
	case WM_MOUSEMOVE: {
		m_mouseX = LOWORD(lparam);
		m_mouseY = HIWORD(lparam);
		return 0;
	}
	case WM_TIMER: {
		if (wparam == ID_TIMER_ANIM) {
			RECT rc;
			GetClientRect(window, &rc);
			int cx = (rc.right - rc.left) / 2;
			int cy = (rc.bottom - rc.top) / 2;

			if (m_is_spinning) {
				m_angle += 0.05f;
				for (int i = 0; i < 10; i++) {
					float a = m_angle + (i * (2.0f * 3.14159f / 10.0f));
					int x = cx + static_cast<int>(150 * cos(a)) - 20;
					int y = cy + static_cast<int>(150 * sin(a)) - 20;
					MoveWindow(m_digits[i], x, y, 40, 40, TRUE);
				}
			}
			else {
				for (int i = 0; i < 10; i++) {
					int targetX = (i == 0) ? m_mouseX : m_lastX[i - 1];
					int targetY = (i == 0) ? m_mouseY : m_lastY[i - 1];

					RECT cur;
					GetWindowRect(m_digits[i], &cur);
					MapWindowPoints(HWND_DESKTOP, window, (LPPOINT)&cur, 2);

					int newX = cur.left + static_cast<int>((targetX - cur.left) * 0.1f);
					int newY = cur.top + static_cast<int>((targetY - cur.top) * 0.1f);

					MoveWindow(m_digits[i], newX, newY, 40, 40, TRUE);
					m_lastX[i] = newX;
					m_lastY[i] = newY;
				}
			}
		}
		return 0;
	}
				 //
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;

	case WM_DESTROY:
		KillTimer(window, ID_TIMER_ANIM);
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;

	case WM_KEYDOWN:
		switch (wparam) {
		case 'X':
			DestroyWindow(window);
			return 0;
		}
		break;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

// builder
app_2048::app_2048(HINSTANCE instance)
	: m_instance{ instance }, m_main{}, m_angle{ 0 }, m_is_spinning{ true }, m_mouseX{ 0 }, m_mouseY{ 0 }
{
	for (int i = 0; i < 10; i++) { m_lastX[i] = 0; m_lastY[i] = 0; }
	register_class();
	m_main = create_window();
}

// main loop
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