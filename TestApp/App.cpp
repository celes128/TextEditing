#include "App.h"

// string to wstring conversion
#include <string>
#include <codecvt>
#include <locale>

#include "TextEditing/CmdInsertChar.h"
#include "TextEditing/CmdDeleteChar.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "TextEditing.lib")

static std::wstring str_to_wstr(const std::string &str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
}

static D2D1::ColorF ColorFrom3i(int red, int green, int blue, int alpha = 255)
{
	return D2D1::ColorF(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
}

int WINAPI WinMain(
	HINSTANCE /*hInstance*/,
	HINSTANCE /*hPrevInstance*/,
	LPSTR /*lpCmdLine*/,
	int /*nCmdShow*/
)
{
	// Ignore the return value because we want to run the program even in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL))) {
		{
			App app;

			if (SUCCEEDED(app.Initialize())) {
				app.RunMessageLoop();
			}
		}

		CoUninitialize();
	}

	return 0;
}


const float App::kFontSize = 28.f;

//
// Initialize members.
//
App::App()
	: m_hwnd(NULL)
	, m_pD2DFactory(NULL)
	, m_pDWriteFactory(NULL)
	, m_pRenderTarget(NULL)
	, m_pTextFormat(NULL)
	, m_pSolidBrush(NULL)
	, m_pStrokeStyle(NULL)
	, m_textline("Enter some text; press Enter to clear")
{
}

//
// Release resources.
//
App::~App()
{
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pSolidBrush);
	SafeRelease(&m_pStrokeStyle);
}

//
// Creates the application window and initializes
// device-independent resources.
//
HRESULT App::Initialize()
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		const auto *kClassName = L"TestAppTextEditing";

		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = App::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = kClassName;

		RegisterClassEx(&wcex);

		// Create the application window.
		//
		// Because the CreateWindow function takes its size in pixels, we
		// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;
		m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		m_hwnd = CreateWindow(
			kClassName,
			L"Plot",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(800.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(600.f * dpiY / 96.f)),
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
		);

		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);

			UpdateWindow(m_hwnd);
		}
	}

	return hr;
}


//
// Create resources which are not bound
// to any device. Their lifetime effectively extends for the
// duration of the app. These resources include the Direct2D and
// DirectWrite factories,  and a DirectWrite Text Format object
// (used for identifying particular font characteristics).
//
HRESULT App::CreateDeviceIndependentResources()
{
	//static const WCHAR msc_fontName[] = L"Verdana";
	//static const WCHAR msc_fontName[] = L"Lucida Console";
	//static const WCHAR msc_fontName[] = L"Lucida Sans Unicode";
	//static const WCHAR msc_fontName[] = L"Andale Mono";
	static const WCHAR msc_fontName[] = L"Consolas";

	HRESULT hr;
	ID2D1GeometrySink *pSink = NULL;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory)
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			App::kFontSize,
			L"", //locale
			&m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	}

	SafeRelease(&pSink);

	return hr;
}

//
//  This method creates resources which are bound to a particular
//  Direct3D device. It's all centralized here, in case the resources
//  need to be recreated in case of Direct3D device loss (eg. display
//  change, remoting, removal of video card, etc).
//
HRESULT App::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{

		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// Create a Direct2D render target.
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);
		if (SUCCEEDED(hr)) {
			// Create an orange brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(ColorFrom3i(209, 117, 25), &m_pSolidBrush);
		}

		if (SUCCEEDED(hr)) {
			hr = m_pD2DFactory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_ROUND,
					D2D1_CAP_STYLE_ROUND,
					D2D1_LINE_JOIN_ROUND,
					0.0f,
					D2D1_DASH_STYLE_SOLID,
					0.0f),
				nullptr,
				0,
				&m_pStrokeStyle
			);
		}

	}

	return hr;
}




//
//  Discard device-specific resources which need to be recreated
//  when a Direct3D device is lost
//
void App::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pSolidBrush);
	SafeRelease(&m_pStrokeStyle);
}

//
// The main window message loop.
//
void App::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


//
//  Called whenever the application needs to display the client
//  window. This method writes "Hello, World"
//
//  Note that this function will not render anything if the window
//  is occluded (e.g. when the screen is locked).
//  Also, this function will automatically discard device-specific
//  resources if the Direct3D device disappears during function
//  invocation, and will recreate the resources the next time it's
//  invoked.
//
HRESULT App::on_render()
{
	HRESULT hr;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr) && !(m_pRenderTarget->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		clear_window(ColorFrom3i(0, 20, 80));

		draw_textline();

		hr = m_pRenderTarget->EndDraw();
		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}
	}

	return hr;
}

void App::clear_window(const D2D1::ColorF &color)
{
	m_pRenderTarget->Clear(color);
}

void App::draw_textline()
{
	// Save the brush color to restore it before the return
	const auto prevColor = m_pSolidBrush->GetColor();

	const auto winSize = WindowSize();
	D2D1_RECT_F rect{
		0.f,
		0.f,
		winSize.width,
		winSize.height
	};

	// Command line text
	const auto wstrText = str_to_wstr(m_textline.String().c_str());
	const auto len = wstrText.length();
	m_pSolidBrush->SetColor(ColorFrom3i(230, 230, 230));

	IDWriteTextLayout *pTextLayout = nullptr;
	auto hr = m_pDWriteFactory->CreateTextLayout(
		wstrText.c_str(),
		len,
		m_pTextFormat,
		rect.right - rect.left,
		rect.bottom - rect.top,
		&pTextLayout
	);
	m_pRenderTarget->DrawTextLayout({ rect.left, rect.top }, pTextLayout, m_pSolidBrush);

	// Draw the caret
	DWRITE_HIT_TEST_METRICS hitTestMetrics;
	float caretX, caretY;
	bool isTrailingHit = false; // Use the leading character edge for simplicity here.

	// Map text position index to caret coordinate and hit-test rectangle.
	pTextLayout->HitTestTextPosition(
		m_textline.Caret(),
		isTrailingHit,
		OUT &caretX,
		OUT &caretY,
		OUT &hitTestMetrics
	);

	// Respect user settings.
	DWORD caretWidth = 1;
	SystemParametersInfo(SPI_GETCARETWIDTH, 0, OUT &caretWidth, 0);

	// Draw a thin rectangle.
	D2D1_RECT_F caretRect = {
		rect.left + caretX - caretWidth / 2u,
		rect.top + hitTestMetrics.top,
		rect.left + caretX + (caretWidth - caretWidth / 2u),
		rect.top + hitTestMetrics.top + hitTestMetrics.height
	};
	m_pRenderTarget->FillRectangle(&caretRect, m_pSolidBrush);

	m_pSolidBrush->SetColor(prevColor);
}

//
//  If the application receives a WM_SIZE message, this method
//  resizes the render target appropriately.
//
void App::on_resize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		// Note: This method can fail, but it's okay to ignore the
		// error here -- it will be repeated on the next call to
		// EndDraw.
		m_pRenderTarget->Resize(size);
	}
}

void App::on_wm_char(WPARAM wParam)
{
	bool changed = true;

	switch (wParam) {
	case 0x08:// Process a backspace.

		// Delete the character on the left of the caret, if it exists.
		if (m_textline.Caret() > 0) {
			m_cmdHist.Execute(new ted::CmdDeleteChar(m_textline.Caret() - 1, m_textline));
			changed = true;
		}
		break;

	case 0x0A:// Process a linefeed.
		break;

	case 0x1B:// Process an escape. 
		break;

	case 0x09:// Process a tab. 
		break;

	case 0x0D:// Process a carriage return.
		changed = m_textline.Clear();
		m_cmdHist.Clear();
		break;

	default:// Filter only displayable ASCII characters
		if (32 <= wParam && wParam < 128) {
			m_cmdHist.Execute(new ted::CmdInsertChar(wParam, m_textline.Caret(), m_textline));
			changed = true;
		}
		else changed = false;
		break;
	}
	
	if (changed) {
		request_redraw();
	}
}

static bool is_key_down(WPARAM wParam)
{
	return GetAsyncKeyState(wParam) & 0x8000;
}

void App::on_wm_keydown(WPARAM wParam)
{
	bool changed = false;

	switch (wParam) {
	case VK_LEFT:
		if (is_key_down(VK_CONTROL)) {
			if (m_textline.CInsideWord()) {
				changed = m_textline.CMoveToWordBegin();
			}
			else {
				changed = m_textline.CMoveToPrevWordBegin();
				if (!changed) {
					changed = m_textline.CMoveToLineBegin();
				}
			}
		}
		else {
			changed = m_textline.CMoveBackward();
		}
		break;

	case VK_RIGHT:
		if (is_key_down(VK_CONTROL)) {
			changed = m_textline.CMoveToNextWordBegin();
			if (!changed) {
				changed = m_textline.CMoveToLineEnd();
			}
		}
		else {
			changed = m_textline.CMoveForward();
		}
		break;

	case VK_HOME:
		changed = m_textline.CMoveToLineBegin();
		break;

	case VK_END:
		changed = m_textline.CMoveToLineEnd();
		break;

	case 'X': case 'x':
		if (is_key_down(VK_CONTROL)) {
			changed = m_textline.Clear();
		}
		break;

	case 'Z': case 'z': {
		if (is_key_down(VK_CONTROL)) {
			changed = m_cmdHist.Undo();
		}
	}break;

	case 'Y': case 'y': {
		if (is_key_down(VK_CONTROL)) {
			changed = m_cmdHist.Redo();
		}
	}break;

	}// END switch( (wParam)

	if (changed) {
		request_redraw();
	}
}

void App::request_redraw()
{
	InvalidateRect(m_hwnd, nullptr, TRUE);
}

D2D1_SIZE_F App::WindowSize() const
{
	RECT rc;
	GetClientRect(m_hwnd, &rc);

	return D2D1::SizeF(
		static_cast<float>(rc.right - rc.left),
		static_cast<float>(rc.bottom - rc.top)
	);
}

//
// The window message handler.
//
LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		App *pApp = (App *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pApp)
		);

		result = 1;
	}
	else
	{
		App *pApp = reinterpret_cast<App *>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			));

		bool wasHandled = false;

		if (pApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pApp->on_resize(width, height);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_PAINT:
			case WM_DISPLAYCHANGE:
			{
				PAINTSTRUCT ps;
				BeginPaint(hwnd, &ps);

				pApp->on_render();
				EndPaint(hwnd, &ps);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_CHAR:
			{
				pApp->on_wm_char(wParam);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_KEYDOWN:
			{
				pApp->on_wm_keydown(wParam);
			}
			wasHandled = true;
			result = 0;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			wasHandled = true;
			result = 1;
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}