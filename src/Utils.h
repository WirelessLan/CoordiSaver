#pragma once

namespace Utils {
	void Trim(std::string& s);
	std::string GetNextData(const std::string& line, uint32_t& index, char delimeter);
	uint8_t AllowTextInput(bool allow);
	bool IsLightPlugin(const RE::TESFile* mod);
	RE::TESForm* GetFormFromIdentifier(const std::string& pluginName, const uint32_t formId);
    RE::TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr);
	RE::TESForm* GetFormFromString(const std::string& formStr);
	RE::Actor* GetCurrentConsoleActor();
}
