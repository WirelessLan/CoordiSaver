#include "Utils.h"

namespace Utils {
	void Trim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	uint8_t GetNextChar(const std::string& line, uint32_t& index) {
		if (index < line.length())
			return line[index++];

		return 0xFF;
	}

	std::string GetNextData(const std::string& line, uint32_t& index, char delimeter) {
		uint8_t ch;
		std::string retVal = "";

		while ((ch = GetNextChar(line, index)) != 0xFF) {
			if (ch == '#') {
				if (index > 0) index--;
				break;
			}

			if (delimeter != 0 && ch == delimeter)
				break;

			retVal += static_cast<char>(ch);
		}

		Trim(retVal);
		return retVal;
	}

	uint8_t AllowTextInput(bool allow) {
		RE::ControlMap* g_controlMap = RE::ControlMap::GetSingleton();
		if (!g_controlMap)
			return 0xFF;

		if (allow) {
			if (g_controlMap->byTextEntryCount == 0xFF)
				logger::warn("InputManager::AllowTextInput: counter overflow");
			else
				g_controlMap->byTextEntryCount++;
		}
		else {
			if (g_controlMap->byTextEntryCount == 0)
				logger::warn("InputManager::AllowTextInput: counter underflow");
			else
				g_controlMap->byTextEntryCount--;
		}

		return g_controlMap->byTextEntryCount;
	}

	const RE::TESFile* LookupModByName(RE::TESDataHandler* dataHandler, const std::string& pluginName) {
		for (RE::TESFile* file : dataHandler->files) {
			if (file->filename == pluginName)
				return file;
		}
		return nullptr;
	}

	bool IsLightPlugin(const RE::TESFile* mod) {
		return (mod->flags & RE::TESFile::RecordFlag::kSmallFile) == RE::TESFile::RecordFlag::kSmallFile;
	}

	uint32_t GetPartialIndex(const RE::TESFile* mod) {
		return !IsLightPlugin(mod) ? mod->compileIndex : (0xFE000 | mod->smallFileCompileIndex);
	}

	uint32_t ToFormId(const std::string& formIdStr) {
		return std::stoul(formIdStr, nullptr, 16) & 0xFFFFFF;
	}

	RE::TESForm* GetFormFromIdentifier(const std::string& pluginName, const uint32_t formId) {
		RE::TESDataHandler* g_dataHandler = RE::TESDataHandler::GetSingleton();
		if (!g_dataHandler)
			return nullptr;

		const RE::TESFile* mod = LookupModByName(g_dataHandler, pluginName);
		if (!mod || mod->compileIndex == -1)
			return nullptr;

		uint32_t actualFormId = formId;
		uint32_t pluginIndex = GetPartialIndex(mod);
		if (!IsLightPlugin(mod))
			actualFormId |= pluginIndex << 24;
		else
			actualFormId |= pluginIndex << 12;

		return RE::TESForm::GetFormByID(actualFormId);
	}

	RE::TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr) {
		uint32_t formID = ToFormId(formIdStr);
		return GetFormFromIdentifier(pluginName, formID);
	}

	RE::TESForm* GetFormFromString(const std::string& formStr) {
		auto delimiter = formStr.find('|');
		if (delimiter == std::string::npos)
			return nullptr;

		std::string pluginName = formStr.substr(0, delimiter);
		std::string formID = formStr.substr(delimiter + 1);

		return Utils::GetFormFromIdentifier(pluginName, formID);
	}
	

	RE::Actor* GetCurrentConsoleActor() {
		RE::ObjectRefHandle handle = RE::Console::GetCurrentPickREFR();
		RE::NiPointer<RE::TESObjectREFR> refr = handle.get();
		if (!refr)
			return nullptr;

		RE::Actor* actor = refr->As<RE::Actor>();
		if (!actor)
			return nullptr;

		return actor;
	}
}
