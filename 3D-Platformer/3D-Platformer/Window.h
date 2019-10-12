#pragma once
#include <Windows.h>
#include "ExceptionHandler.h"
#include "Keyboard.h"
#include <optional>

class Window
{
public:
	class Exception : public ExceptionHandler
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};

private:
	class WindowClass
	{
	public:
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;								//Get the window handle 
	private:
		WindowClass() noexcept;													//Create window
		~WindowClass();															//Destroy window
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Direct X 3D Platformer";
		static WindowClass wndClass;											//Windows Class singleton
		HINSTANCE hInst;														//Instance of the windows application 
	};
public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	static std::optional<int> ProcessMessages() noexcept;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgAdapter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard keyboard;
private:
	int width;
	int height;
	HWND hWnd;																	//Window Handle
};