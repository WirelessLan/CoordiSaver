#include "CoordiSlots.h"

#include <fstream>

#include "Utils.h"

namespace CoordiSlots {
	bool HasItem(RE::Actor* a_actor, RE::TESForm* a_item) {
		if (!a_actor || !a_item)
			return false;

		RE::BGSInventoryList* inventory = a_actor->inventoryList;
		if (!inventory)
			return false;

		uint32_t totalItemCount = 0;
		inventory->rwLock.lock_read();
		for (RE::BGSInventoryItem item : inventory->data) {
			if (item.object == a_item) {
				RE::BGSInventoryItem::Stack* sp = item.stackData.get();
				while (sp) {
					totalItemCount += sp->count;
					sp = sp->nextStack.get();
				}
				break;
			}
		}
		inventory->rwLock.unlock_read();

		return totalItemCount > 0;
	}

	std::vector<RE::TESForm*> GetCurrentEquippedItemList(RE::Actor* a_actor) {
		std::vector<RE::TESForm*> retVec;

		if (!a_actor || !a_actor->biped)
			return retVec;

		// 30 (0) ~ 58 (28) Slots
		for (uint32_t ii = 0; ii <= 28; ii++) {
			if (a_actor->biped->object[ii].parent.object && HasItem(a_actor, a_actor->biped->object[ii].parent.object))
				retVec.push_back(a_actor->biped->object[ii].parent.object);
		}

		return retVec;
	}

	bool SaveSlot(const std::string& slotName) {
		std::string slotPath = fmt::format(FMT_STRING("Data\\F4SE\\Plugins\\{}_Slots\\{}.codss"), Version::PROJECT, slotName);
		logger::info(FMT_STRING("SaveSlot: {}"), slotPath);

		std::ofstream slotFile(slotPath);

		RE::Actor* actor = Utils::GetCurrentConsoleActor();
		if (!actor) {
			actor = RE::PlayerCharacter::GetSingleton();
			if (!actor) {
				logger::critical("Cannot find the actor");
				return false;
			}
		}

		std::vector<RE::TESForm*> currentEquippedItemList = GetCurrentEquippedItemList(actor);
		if (currentEquippedItemList.size() == 0)
			return false;

		if (!slotFile.is_open())
			return false;

		for (RE::TESForm* form : currentEquippedItemList) {
			RE::TESFile* plugin = form->sourceFiles.array->front();
			if (!plugin) {
				logger::critical("No plugin found");
				continue;
			}

			uint32_t formId = Utils::IsLightPlugin(plugin) ? form->formID & 0xFFF : form->formID & 0xFFFFFF;
			slotFile << plugin->filename << "|" << std::uppercase << std::hex << formId << std::endl;
		}

		return true;
	}

	std::vector<RE::TESForm*> LoadSlot(const std::string& slotName) {
		std::string slotPath = fmt::format(FMT_STRING("Data\\F4SE\\Plugins\\{}_Slots\\{}.codss"), Version::PROJECT, slotName);
		logger::info(FMT_STRING("LoadSlot: {}"), slotPath);

		std::ifstream slotFile(slotPath);

		std::vector<RE::TESForm*> retVec;

		if (!slotFile.is_open()) {
			logger::warn(FMT_STRING("Cannot open the slotFile: {}"), slotPath);
			return retVec;
		}

		std::string line;
		std::string pluginName, formId;
		while (std::getline(slotFile, line)) {
			Utils::Trim(line);
			if (line.empty() || line.starts_with('#'))
				continue;

			uint32_t index = 0;

			pluginName = Utils::GetNextData(line, index, '|');
			if (pluginName.empty()) {
				logger::warn(FMT_STRING("Cannot read the pluginName: {}"), line);
				continue;
			}

			formId = Utils::GetNextData(line, index, 0);
			if (formId.empty()) {
				logger::warn(FMT_STRING("Cannot read the formId: {}"), line);
				continue;
			}

			RE::TESForm* form = Utils::GetFormFromIdentifier(pluginName, formId);
			if (!form) {
				logger::warn(FMT_STRING("Invalid form: {}"), line);
				continue;
			}

			retVec.push_back(form);
		}

		return retVec;
	}

	bool DeleteSlot(const std::string& slotName) {
		std::string slotPath = fmt::format(FMT_STRING("Data\\F4SE\\Plugins\\{}_Slots\\{}.codss"), Version::PROJECT, slotName);
		logger::info(FMT_STRING("DeleteSlot: {}"), slotPath);

		return remove(slotPath.c_str()) == 0;
	}
}
