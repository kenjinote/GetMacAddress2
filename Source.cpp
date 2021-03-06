#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib,"iphlpapi")

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>

TCHAR szClassName[] = TEXT("Window");

void GetMacAddress(HWND hList)
{
	DWORD dwSize = 0;
	GetAdaptersAddresses(0, 0, 0, 0, &dwSize);
	if (!dwSize) return;

	PIP_ADAPTER_ADDRESSES pAdapterAddresses = (PIP_ADAPTER_ADDRESSES)GlobalAlloc(0, dwSize);
	if (GetAdaptersAddresses(0, 0, 0, pAdapterAddresses, &dwSize) == NO_ERROR)
	{
		while (pAdapterAddresses)
		{
			if (pAdapterAddresses->PhysicalAddressLength)
			{
				TCHAR szMacAddress[64];
				wsprintf(szMacAddress, TEXT("%02X-%02X-%02X-%02X-%02X-%02X"),
					pAdapterAddresses->PhysicalAddress[0],
					pAdapterAddresses->PhysicalAddress[1],
					pAdapterAddresses->PhysicalAddress[2],
					pAdapterAddresses->PhysicalAddress[3],
					pAdapterAddresses->PhysicalAddress[4],
					pAdapterAddresses->PhysicalAddress[5]);
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)szMacAddress);
			}
			pAdapterAddresses = pAdapterAddresses->Next;
		}
	}
	GlobalFree(pAdapterAddresses);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	static HWND hList;
	switch (msg)
	{
	case WM_CREATE:
		hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), 0, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOINTEGRALHEIGHT, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("Macアドレスを取得"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		PostMessage(hWnd, WM_COMMAND, IDOK, 0);
		break;
	case WM_SIZE:
		MoveWindow(hList, 10, 10, 256, 128, TRUE);
		MoveWindow(hButton, 10, 148, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			GetMacAddress(hList);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("GetAdaptersAddresses 関数を使用してMacアドレスを取得する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
