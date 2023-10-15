#include "InputMap.h"

#include <Windows.h>
#include <Xinput.h>

namespace InputMap {
	enum MACRO : uint32_t {
		// first 256 for keyboard, then 8 mouse buttons, then mouse wheel up, wheel down, then 16 gamepad buttons
		kMacro_KeyboardOffset = 0,		// not actually used, just for self-documentation
		kMacro_NumKeyboardKeys = 256,

		kMacro_MouseButtonOffset = kMacro_NumKeyboardKeys,	// 256
		kMacro_NumMouseButtons = 8,

		kMacro_MouseWheelOffset = kMacro_MouseButtonOffset + kMacro_NumMouseButtons,	// 264
		kMacro_MouseWheelDirections = 2,

		kMacro_GamepadOffset = kMacro_MouseWheelOffset + kMacro_MouseWheelDirections,	// 266
		kMacro_NumGamepadButtons = 16,

		kMaxMacros = kMacro_GamepadOffset + kMacro_NumGamepadButtons	// 282
	};

	enum GAMEPAD_OFFSET : uint32_t {
		kGamepadButtonOffset_DPAD_UP = MACRO::kMacro_GamepadOffset,	// 266
		kGamepadButtonOffset_DPAD_DOWN,
		kGamepadButtonOffset_DPAD_LEFT,
		kGamepadButtonOffset_DPAD_RIGHT,
		kGamepadButtonOffset_START,
		kGamepadButtonOffset_BACK,
		kGamepadButtonOffset_LEFT_THUMB,
		kGamepadButtonOffset_RIGHT_THUMB,
		kGamepadButtonOffset_LEFT_SHOULDER,
		kGamepadButtonOffset_RIGHT_SHOULDER,
		kGamepadButtonOffset_A,
		kGamepadButtonOffset_B,
		kGamepadButtonOffset_X,
		kGamepadButtonOffset_Y,
		kGamepadButtonOffset_LT,
		kGamepadButtonOffset_RT	// 281
	};

	bool IsValidKeycode(uint32_t a_keyCode) {
		return a_keyCode < InputMap::kMaxMacros;
	}

	uint32_t GetMouseKeycode(int32_t a_idCode) {
		return kMacro_MouseButtonOffset + a_idCode;
	}

	uint32_t GamepadMaskToKeycode(uint32_t a_keyMask) {
		switch (a_keyMask) {
		case XINPUT_GAMEPAD_DPAD_UP:		return kGamepadButtonOffset_DPAD_UP;
		case XINPUT_GAMEPAD_DPAD_DOWN:		return kGamepadButtonOffset_DPAD_DOWN;
		case XINPUT_GAMEPAD_DPAD_LEFT:		return kGamepadButtonOffset_DPAD_LEFT;
		case XINPUT_GAMEPAD_DPAD_RIGHT:		return kGamepadButtonOffset_DPAD_RIGHT;
		case XINPUT_GAMEPAD_START:			return kGamepadButtonOffset_START;
		case XINPUT_GAMEPAD_BACK:			return kGamepadButtonOffset_BACK;
		case XINPUT_GAMEPAD_LEFT_THUMB:		return kGamepadButtonOffset_LEFT_THUMB;
		case XINPUT_GAMEPAD_RIGHT_THUMB:	return kGamepadButtonOffset_RIGHT_THUMB;
		case XINPUT_GAMEPAD_LEFT_SHOULDER:	return kGamepadButtonOffset_LEFT_SHOULDER;
		case XINPUT_GAMEPAD_RIGHT_SHOULDER: return kGamepadButtonOffset_RIGHT_SHOULDER;
		case XINPUT_GAMEPAD_A:				return kGamepadButtonOffset_A;
		case XINPUT_GAMEPAD_B:				return kGamepadButtonOffset_B;
		case XINPUT_GAMEPAD_X:				return kGamepadButtonOffset_X;
		case XINPUT_GAMEPAD_Y:				return kGamepadButtonOffset_Y;
		case 0x9:							return kGamepadButtonOffset_LT;
		case 0xA:							return kGamepadButtonOffset_RT;
		default:							return kMaxMacros; // Invalid
		}
	}

	uint32_t ReplaceKeyCodeForMenu(uint32_t a_keyCode) {
		switch (a_keyCode) {
		case kGamepadButtonOffset_DPAD_UP:
		case 0x57:	// W Key
			return ACTION_KEY::kActionKey_UP;
			break;

		case kGamepadButtonOffset_DPAD_DOWN:
		case 0x53:	// S Key
			return ACTION_KEY::kActionKey_DOWN;
			break;

		case kGamepadButtonOffset_DPAD_LEFT:
		case 0x41:	// A Key
			return ACTION_KEY::kActionKey_LEFT;
			break;

		case kGamepadButtonOffset_DPAD_RIGHT:
		case 0x44:	// D Key
			return ACTION_KEY::kActionKey_RIGHT;
			break;

		case kGamepadButtonOffset_A:
		case 0x45:	// E Key
			return ACTION_KEY::kActionKey_ENTER;
			break;

		case kGamepadButtonOffset_B:
			return ACTION_KEY::kActionKey_TAB;
			break;
		}

		return a_keyCode;
	}

	uint32_t DirectionToKeyCode(RE::DIRECTION_VAL a_dir) {
		switch (a_dir) {
		case RE::DIRECTION_VAL::kUp:
			return ACTION_KEY::kActionKey_UP;
			break;

		case RE::DIRECTION_VAL::kDown:
			return ACTION_KEY::kActionKey_DOWN;
			break;

		case RE::DIRECTION_VAL::kLeft:
			return ACTION_KEY::kActionKey_LEFT;
			break;

		case RE::DIRECTION_VAL::kRight:
			return ACTION_KEY::kActionKey_RIGHT;
			break;
		}

		return 0xFF;
	}
}
