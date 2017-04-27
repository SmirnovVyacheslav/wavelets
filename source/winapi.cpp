#pragma once

#include"winapi.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
	MSG  message;
	WNDCLASSW wc = { 0 };

	wc.style = CS_HREDRAW|CS_VREDRAW;
	wc.lpszClassName = L"Wavelet";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(WHITE_BRUSH);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassW(&wc);

	WNDCLASSW wc_g = { 0 };

	wc_g.lpszClassName = L"View";
	wc_g.hInstance = hInstance;
	wc_g.hbrBackground = GetSysColorBrush(WHITE_BRUSH);
	wc_g.lpfnWndProc = ChildProc;
	wc_g.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassW(&wc_g);

	WNDCLASSW wc_p = { 0 };

	wc_p.lpszClassName = L"View";
	wc_p.hInstance = hInstance;
	wc_p.hbrBackground = GetSysColorBrush(WHITE_BRUSH);
	wc_p.lpfnWndProc = ChildProc;
	wc_p.hCursor = LoadCursor(0, IDC_ARROW);
	RegisterClassW(&wc_p);

	Main_Window = CreateWindowW(wc.lpszClassName, L"Wavelet", WS_OVERLAPPEDWINDOW|WS_VISIBLE, pX, pY, main_cx, main_cy, NULL, NULL, hInstance, NULL);

	Transformation = CreateWindow(L"Combobox", NULL, WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN, 20, 20, 200, 140, Main_Window, (HMENU)ID_TRANSFORM_NAME, wc.hInstance, NULL);

	Info = CreateWindow(L"Edit", L"", WS_CHILD|WS_VISIBLE|WS_BORDER, 20, 0, 400, 20, Main_Window, (HMENU)ID_INFO, wc.hInstance, NULL);
	Step = CreateWindow(L"Edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 440, 0, 100, 20, Main_Window, (HMENU)ID_STEP, wc.hInstance, NULL);

	Open    = CreateWindow(L"Button", L"Open", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 60, 80, 30, Main_Window, (HMENU)ID_OPEN, wc.hInstance, NULL);
	Save    = CreateWindow(L"Button", L"Save", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 110, 80, 30, Main_Window, (HMENU)ID_SAVE, wc.hInstance, NULL);
	Direct  = CreateWindow(L"Button", L"Direct", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 160, 80, 30, Main_Window, (HMENU)ID_DIRECT, wc.hInstance, NULL);
	Reverse = CreateWindow(L"Button", L"Reverse", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 210, 80, 30, Main_Window, (HMENU)ID_REVERSE, wc.hInstance, NULL);
	Cancel  = CreateWindow(L"Button", L"Cancel", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 260, 80, 30, Main_Window, (HMENU)ID_CANCEL, wc.hInstance, NULL);
	Filter = CreateWindow(L"Button", L"Filter", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 20, 310, 80, 30, Main_Window, (HMENU)ID_FILTER, wc.hInstance, NULL);
	Inc_Step = CreateWindow(L"Button", L"Inc Step", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 360, 80, 30, Main_Window, (HMENU)ID_INC_STEP, wc.hInstance, NULL);
	Dec_Step = CreateWindow(L"Button", L"Dec Step", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 410, 80, 30, Main_Window, (HMENU)ID_DEC_STEP, wc.hInstance, NULL);
	
	Full = CreateWindow(L"Button", L"Full", WS_VISIBLE|WS_CHILD|BS_CHECKBOX, 240, 20, 80, 30, Main_Window, (HMENU)ID_FULL, wc.hInstance, NULL);
	WPack = CreateWindow(L"Button", L"W-Pack", WS_VISIBLE|WS_CHILD|BS_CHECKBOX, 340, 20, 80, 30, Main_Window, (HMENU)ID_WPACK, wc.hInstance, NULL);

	View = CreateWindowW(wc_g.lpszClassName, L"View", WS_CHILD | WS_BORDER | WS_VISIBLE, view_x, view_y, view_cx, view_cy, Main_Window, (HMENU)1025, hInstance, NULL);
	Plot = CreateWindowW(wc_p.lpszClassName, L"View", WS_CHILD | WS_BORDER | WS_VISIBLE, plot_x, plot_y, plot_cx, plot_cy, Main_Window, (HMENU)1026, hInstance, NULL);

	SendMessage(Info, EM_SETREADONLY, TRUE, NULL);
	//SendMessage(Step, EM_SETREADONLY, TRUE, NULL);

	for (auto name : transform_name)
		SendMessage(Transformation, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(name));
	SendMessage(Transformation, CB_SETCURSEL, 0, 0);

	gl.reset(new Open_GL);

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			gl->render();
			gl->render_plot();
		}
	}

	return static_cast<int>(message.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
		case WM_SIZE:
		{
			GetClientRect(hwnd, &rect);
			main_cx = rect.right;
			main_cy = rect.bottom;
			view_cx = (main_cx - 160) / 2;
			view_cy = (main_cy - 100);
			plot_x = view_x + view_cx + 20;
			plot_y = view_y;
			plot_cx = view_cx;
			plot_cy = view_cy;
			SetWindowPos(View, NULL, view_x, view_y, view_cx, view_cy, NULL);
			SetWindowPos(Plot, NULL, plot_x, plot_y, plot_cx, plot_cy, NULL);
			if (gl.get() != nullptr)
				gl->resize(view_cx, view_cy);
		} break;

		case WM_MOUSEWHEEL:
		{
			if (wave.get() != nullptr)
			{
				if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
					++(*wave);
				else
					--(*wave);
			}
			//::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		} break;

		case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			case ID_OPEN:
			{
				file[0] = L'\0';
				file_handler.lStructSize = sizeof(OPENFILENAMEW);
				file_handler.hwndOwner = FindWindow(NULL, L"Wavelet");
				file_handler.lpstrFilter = L"Все файлы\0*.*\0\0";
				file_handler.lpstrCustomFilter = NULL;
				file_handler.nFilterIndex = 1;
				file_handler.lpstrFile = file;
				file_handler.nMaxFile = 1024;
				file_handler.lpstrFileTitle = NULL;
				file_handler.lpstrInitialDir = NULL;
				file_handler.lpstrTitle = NULL;
				file_handler.Flags = OFN_EXPLORER;
				file_handler.lpstrDefExt = NULL;
				file_handler.FlagsEx = 0;
				
				if (GetOpenFileNameW(&file_handler))
				{
					wave.reset(new Wavelet(file_handler.lpstrFile));
					wstring msg = L"Zero: " + to_wstring((*wave)->get_zero_num()) +
						L"; Zero % : " + to_wstring((*wave)->get_zero_prs());
					SetWindowText(Info, msg.c_str());

					gl->create_buffer();
				}

				::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			} break;

			case ID_SAVE:
				if (wave.get() != nullptr)
					wave->write();
				break;

			case ID_DIRECT:
			{
				if (wave.get() != nullptr)
				{
					time_req = wave->transform(DIRECT, SendMessage(Transformation, CB_GETCURSEL, 0, 0));
					wstring msg = L"Time: " + to_wstring(time_req) +
						L"; Zero: " + to_wstring((*wave)->get_zero_num()) +
						L"; Zero % : " + to_wstring((*wave)->get_zero_prs());
					SetWindowText(Info, msg.c_str());

					gl->update_buffer();

					SetWindowText(Step, to_wstring((*wave)->get_filter_value()).c_str());
				}
				::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			} break;

			case ID_REVERSE:
			{
				if (wave.get() != nullptr)
				{
					time_req = wave->transform(REVERSE);
					wstring msg = L"Time: " + to_wstring(time_req) +
						L"; Zero: " + to_wstring((*wave)->get_zero_num()) +
						L"; Zero % : " + to_wstring((*wave)->get_zero_prs());
					SetWindowText(Info, msg.c_str());

					SetWindowText(Step, to_wstring(0.0).c_str());

					gl->update_buffer();
				}
				::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			} break;

			case ID_CANCEL:
			{
				if (wave.get() != nullptr)
				{
					wave->source();
					wstring msg = L"Zero: " + to_wstring((*wave)->get_zero_num()) +
						L"; Zero % : " + to_wstring((*wave)->get_zero_prs());
					SetWindowText(Info, msg.c_str());

					SetWindowText(Step, to_wstring(0.0).c_str());

					gl->update_buffer();
				}
				CheckDlgButton(hwnd, ID_FULL, BST_UNCHECKED);
				CheckDlgButton(hwnd, ID_WPACK, BST_UNCHECKED);
				::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			} break;

			case ID_FILTER:
			{
				if (wave.get() != nullptr)
				{
					wave->filter();
					wstring msg = L"Zero: " + to_wstring((*wave)->get_zero_num()) +
						L"; Zero % : " + to_wstring((*wave)->get_zero_prs());
					SetWindowText(Info, msg.c_str());

					gl->update_buffer();
				}
				::RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
			} break;

			case ID_FULL:
			{
				if (wave.get() != nullptr)
					if ((*wave)->Set_full()) CheckDlgButton(hwnd, ID_FULL, BST_CHECKED);
					else CheckDlgButton(hwnd, ID_FULL, BST_UNCHECKED);
			} break;
			
			case ID_WPACK:
			{
				if (wave.get() != nullptr)
					if ((*wave)->Set_w_pack()) CheckDlgButton(hwnd, ID_WPACK, BST_CHECKED);
					else CheckDlgButton(hwnd, ID_WPACK, BST_UNCHECKED);
			} break;

			case ID_INC_STEP:
			{
				if (wave.get() != nullptr)
				{
					(*wave).inc_filter_value();
					SetWindowText(Step, to_wstring((*wave)->get_filter_value()).c_str());
				}
			} break;

			case ID_DEC_STEP:
			{
				if (wave.get() != nullptr)
				{
					(*wave).dec_filter_value();
					SetWindowText(Step, to_wstring((*wave)->get_filter_value()).c_str());
				}
			} break;

			case ID_STEP:
			{
				if (HIWORD(wParam) == EN_CHANGE && wave.get() != nullptr)
				{
					TCHAR buff[30];
					GetWindowText(Step, buff, 30);
					(*wave)->set_filter_value(stod(buff));
				}
			} break;
		} break;

	case WM_ERASEBKGND:
		return 1;

	case WM_PAINT:
		::RedrawWindow(hwnd, NULL, NULL, RDW_NOINTERNALPAINT);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	if (Message == WM_PAINT)
	{
		gl->render();
	}
	if (Message == WM_DESTROY)
	{
		return 0;
	}
	return DefWindowProc(hwnd, Message, wparam, lparam);
}

LRESULT CALLBACK ChildProc_2(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
	if (Message == WM_PAINT)
	{
		//gl->build_plot();
		//gl->render();
	}
	if (Message == WM_DESTROY)
	{
		return 0;
	}
	return DefWindowProc(hwnd, Message, wparam, lparam);
}