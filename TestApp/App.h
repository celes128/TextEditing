#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER              // Allow use of features specific to Windows 7 or later.
#define WINVER 0x0700       // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows 7 or later.
#define _WIN32_WINNT 0x0700 // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "TextEditing/TextLine.h"
#include "TextEditing/CommandHistory.h"

template<class Interface>
inline void
SafeRelease(
	Interface **ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class App {
public:
	App();
	~App();

	HRESULT Initialize();

	void RunMessageLoop();

private:
	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();
	void DiscardDeviceResources();

	// Windows message handling
	void on_wm_char(WPARAM wParam);
	void on_wm_keydown(WPARAM wParam);
	void on_resize(UINT width, UINT height);

	// Rendering
	HRESULT on_render();
	void clear_window(const D2D1::ColorF &color);
	void draw_textline();
	void request_redraw();

	D2D1_SIZE_F WindowSize() const;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static const float kFontSize;

private:
	HWND						m_hwnd;
	ID2D1Factory				*m_pD2DFactory;
	IWICImagingFactory			*m_pWICFactory;
	IDWriteFactory				*m_pDWriteFactory;
	ID2D1HwndRenderTarget		*m_pRenderTarget;
	IDWriteTextFormat			*m_pTextFormat;
	ID2D1SolidColorBrush		*m_pSolidBrush;
	ID2D1StrokeStyle			*m_pStrokeStyle;

	ted::CommandHistory<64>		m_cmdHist;
	ted::TextLine				m_textline;
};