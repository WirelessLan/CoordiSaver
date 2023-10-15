#pragma once

namespace InputMap {
	enum ACTION_KEY : uint32_t {
		kActionKey_TAB = 0x09,
		kActionKey_ENTER = 0x0D,
		kActionKey_LEFT = 0x25,
		kActionKey_UP = 0x26,
		kActionKey_RIGHT = 0x27,
		kActionKey_DOWN = 0x28,
	};

	bool IsValidKeycode(uint32_t keyCode);
	uint32_t GetMouseKeycode(int32_t idCode);
	uint32_t GamepadMaskToKeycode(uint32_t keyMask);
	uint32_t ReplaceKeyCodeForMenu(uint32_t keyCode);
	uint32_t DirectionToKeyCode(RE::DIRECTION_VAL dir);
}
