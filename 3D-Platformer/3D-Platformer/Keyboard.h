#pragma once
#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			PRESS,
			RELEASE,
			INVALID
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() :
			type(Type::INVALID),
			code(0u) {}

		Event(Type type, unsigned char code) noexcept :
			type(type),
			code(code) {}

		bool IsPress() const noexcept
		{
			return type == Type::PRESS;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::RELEASE;
		}
		bool IsValid() const
		{
			return type != Type::INVALID;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};


public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// key events
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void ClearKey() noexcept;
	void ClearEvents() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};