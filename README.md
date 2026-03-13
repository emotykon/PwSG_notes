# PwSG_notes
1. Moduł: Tworzenie okienek-cyfr (Child Windows)Aby stworzyć 10 fizycznych przycisków, które będą "okienkami" na tarczy, używasz pętli w komunikacie WM_CREATE.C++// Wewnątrz case WM_CREATE:
for (int i = 0; i < 10; i++) {
    m_digits[i] = CreateWindowExW(
        0, 
        L"BUTTON",                         // Klasa systemowa przycisku [cite: 344]
        std::to_wstring(i).c_str(),        // Cyfra jako tekst
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, // Style okna potomnego [cite: 346]
        0, 0, 40, 40,                      // Początkowa pozycja i rozmiar
        window,                            // Uchwyt rodzica (m_main) [cite: 352]
        (HMENU)(5000 + i),                 // Unikalne ID dla każdej cyfry [cite: 700]
        m_instance, 
        NULL
    );
}
2. Moduł: Matematyka tarczy (Obliczanie pozycji)Używasz funkcji trygonometrycznych do rozmieszczenia okienek na okręgu.C++// Funkcja pomocnicza wywoływana w WM_TIMER
void update_dial_positions(HWND parent, float global_angle) {
    RECT rc;
    GetClientRect(parent, &rc); // Pobierz rozmiar okna [cite: 318]
    int cx = (rc.right - rc.left) / 2;
    int cy = (rc.bottom - rc.top) / 2;
    int radius = 150;

    for (int i = 0; i < 10; i++) {
        // Rozkład co 36 stopni (2*PI / 10)
        float angle = global_angle + (i * (2.0f * 3.14159f / 10.0f));
        int x = cx + static_cast<int>(radius * cos(angle)) - 20; // -20 to połowa szerokości guzika
        int y = cy + static_cast<int>(radius * sin(angle)) - 20;

        // Fizyczne przesunięcie okienka-cyfry [cite: 407, 477]
        MoveWindow(m_digits[i], x, y, 40, 40, TRUE);
    }
}
3. Moduł: Efekt "Sprężynki" (Podążanie za kursorem)Ten moduł realizuje etap drugi, gdzie każde okienko goni poprzednie z opóźnieniem.C++// Wewnątrz case WM_TIMER (jeśli tryb sprężynki):
for (int i = 0; i < 10; i++) {
    // Cel: dla pierwszego okna to mysz, dla reszty poprzednie okno
    int targetX = (i == 0) ? m_mouseX : m_lastX[i - 1];
    int targetY = (i == 0) ? m_mouseY : m_lastY[i - 1];

    // Pobierz obecną pozycję okienka [cite: 451, 464]
    RECT cur;
    GetWindowRect(m_digits[i], &cur);
    MapWindowPoints(HWND_DESKTOP, window, (LPPOINT)&cur, 2); // Zamiana na współrzędne okna

    // Wzór na sprężynkę: NowaPoz = Stara + (Cel - Stara) * siła
    int newX = cur.left + static_cast<int>((targetX - cur.left) * 0.15f);
    int newY = cur.top + static_cast<int>((targetY - cur.top) * 0.15f);

    MoveWindow(m_digits[i], newX, newY, 40, 40, TRUE);
    
    // Zapisz pozycję dla następnego okienka w pętli
    m_lastX[i] = newX;
    m_lastY[i] = newY;
}
4. Moduł: Obsługa Myszy i TimerówNiezbędne do zbierania danych o kursorze i napędzania animacji.C++case WM_MOUSEMOVE:
    m_mouseX = LOWORD(lparam); // Pobierz X myszy
    m_mouseY = HIWORD(lparam); // Pobierz Y myszy
    return 0;

case WM_TIMER:
    if (wparam == ID_TIMER_ANIM) {
        if (m_mode == MODE_DIAL) {
            m_angle += 0.05f; // Zwiększ kąt obrotu
            update_dial_positions(window, m_angle);
        } else {
            update_spring_positions(window); // Logika z modułu 3
        }
    }
    return 0;
5. Moduł: Kolorowanie i GDI (Tarcza w tle)Rysowanie statycznego koła pod ruchomymi przyciskami.C++case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(window, &ps); // Start rysowania [cite: 798]
    
    HBRUSH brush = CreateSolidBrush(RGB(200, 200, 200)); // Szary pędzel [cite: 359, 813]
    SelectObject(dc, brush); // Wybierz pędzel [cite: 811]
    
    // Rysowanie koła tarczy w centrum okna
    Ellipse(dc, centerX - 180, centerY - 180, centerX + 180, centerY + 180);
    
    DeleteObject(brush); // Sprzątanie zasobów [cite: 838]
    EndPaint(window, &ps); // Koniec rysowania [cite: 773]
    return 0;
}




/////////////////////////////

C++
#define ID_GRID_BASE 3000 // ID pierwszego przycisku
2. Kod pętli w app_2048.cpp (sekcja WM_CREATE)
Wstaw ten kod zamiast pojedynczego przycisku, który dodaliśmy wcześniej.

C++
case WM_CREATE: {
    // ... tutaj zostaw kod od Menu ...

    int buttonWidth = 80;
    int buttonHeight = 80;
    int padding = 10; // odstęp między przyciskami
    int startX = 20;  // margines od lewej krawędzi okna
    int startY = 60;  // margines od góry (pod menu i polem tekstowym)

    for (int i = 0; i < 12; i++) {
        // Obliczamy kolumnę (0, 1, 2) i rząd (0, 1, 2, 3)
        int col = i % 3;
        int row = i / 3;

        int x = startX + col * (buttonWidth + padding);
        int y = startY + row * (buttonHeight + padding);

        // Tworzymy przycisk
        CreateWindowW(L"BUTTON", std::to_wstring(i + 1).c_str(),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, y, buttonWidth, buttonHeight,
            window, (HMENU)(ID_GRID_BASE + i), // Każdy dostaje ID: 3000, 3001, 3002...
            (HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE), NULL);
    }
    return 0;
}
3. Obsługa kliknięcia (sekcja WM_COMMAND)
Teraz, zamiast pisać 12 razy case, sprawdzamy, czy kliknięte ID mieści się w naszym zakresie.

C++
case WM_COMMAND: {
    int wmId = LOWORD(wparam);

    // Sprawdzamy czy kliknięto któryś z 12 przycisków
    if (wmId >= ID_GRID_BASE && wmId < ID_GRID_BASE + 12) {
        int buttonIndex = wmId - ID_GRID_BASE; // Wyliczamy numer przycisku (0-11)
        
        std::wstring msg = L"Kliknąłeś w okienko numer: " + std::to_wstring(buttonIndex + 1);
        MessageBoxW(window, msg.c_str(), L"Gra", MB_OK);
        return 0;
    }

    switch (wmId) {
        // ... reszta Twoich case (Exit, About itp.) ...
    }
    return 0;
}


/////////////////////////////////////////


To zadanie wymaga od Ciebie zrozumienia mechanizmu subclassingu lub bardzo precyzyjnej obsługi komunikatu WM_MOVE (lub WM_MOVING). Skoro mamy unikać WM_PAINT, skupiamy się wyłącznie na logice pozycjonowania okien za pomocą funkcji SetWindowPos.

Aby 10 okienek reagowało na siebie nawzajem, musimy stworzyć mechanizm "lidera" i "naśladowców" lub system, w którym każde okno informuje pozostałe o swoim nowym położeniu.

1. Strategia: Tablica uchwytów
Musisz przechowywać uchwyty (HWND) do wszystkich 10 okienek, aby móc nimi sterować z poziomu jednej procedury.

C++
// W klasie app_2048 lub globalnie
HWND m_windows[10];
bool m_is_moving = false; // Flaga zapobiegająca pętli nieskończonej
2. Tworzenie 10 osobnych okien
Zamiast kontrolek (dzieci), musisz stworzyć 10 niezależnych okien typu WS_OVERLAPPEDWINDOW (lub popup), aby każde mogło poruszać się po pulpicie.

C++
// W pętli tworzącej okna
for (int i = 0; i < 10; i++) {
    m_windows[i] = CreateWindowExW(0, s_class_name.c_str(), L"Okno", 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        100 + (i * 40), 100 + (i * 40), 200, 200, 
        nullptr, nullptr, m_instance, this);
}
3. Logika wzajemnego ruchu (WM_MOVE)
Kluczem jest obsługa komunikatu WM_MOVE. Gdy jedno okno się przesuwa, musi ono "pociągnąć" za sobą resztę. Używamy flagi m_is_moving, aby uniknąć sytuacji, w której okno A przesuwa B, a B próbuje przesunąć A (pętla nieskończona).

C++
case WM_MOVE: {
    // Jeśli to przesunięcie zostało wywołane przez inne okno, nic nie rób
    if (m_is_moving) return 0;

    m_is_moving = true; // Blokada

    int xPos = (int)(short)LOWORD(lparam); // Nowa pozycja X
    int yPos = (int)(short)HIWORD(lparam); // Nowa pozycja Y

    // Przesuń wszystkie pozostałe okna o ten sam wektor
    for (int i = 0; i < 10; i++) {
        if (m_windows[i] != window) { // Nie przesuwaj samego siebie drugi raz
            // Tutaj możesz dodać offset, np. żeby okna zachowały dystans
            SetWindowPos(m_windows[i], NULL, 
                xPos + (i * 20), yPos + (i * 20), 
                0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }

    m_is_moving = false; // Zdejmij blokadę
    return 0;
}
4. Dlaczego bez WM_PAINT?
Prowadzący prawdopodobnie chce sprawdzić, czy rozumiesz, że:

Logika położenia dzieje się w WM_MOVE lub WM_MOVING.

SetWindowPos to funkcja, która fizycznie zmienia strukturę okna w systemie, a nie tylko jego wygląd.

WM_PAINT służy wyłącznie do rysowania treści wewnątrz okna (tekst, kolory, grafika), a nie do sterowania jego ramką czy pozycją na ekranie.

Co warto dodać (Pro-tip na PWSG):
Jeśli chcesz, żeby okna "odbijały się" od siebie lub zachowywały jak magnesy, wewnątrz pętli for w WM_MOVE musisz dodać warunki sprawdzające odległość między nimi.

Pytanie dodatkowe: Czy te okna mają się "kleić" do siebie (jak magnetyczne klocki), czy po prostu wszystkie mają się poruszać identycznie jak jeden sztywny obiekt?
