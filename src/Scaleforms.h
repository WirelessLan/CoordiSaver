#pragma once

namespace Scaleforms {
	void RegisterMenu();
	void LoadLocalizations();
	void RegisterFunctions(RE::Scaleform::GFx::Movie* a_view, RE::Scaleform::GFx::Value* a_value);
	void RegisterMenuInputHandler(bool bReg);
	void OpenMenu();
	void CloseMenu();
}
