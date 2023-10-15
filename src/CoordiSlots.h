#pragma once

namespace CoordiSlots {
	bool SaveSlot(const std::string& slotName);
	std::vector<RE::TESForm*> LoadSlot(const std::string& slotName);
	std::vector<std::string> GetSlotList();
	bool DeleteSlot(const std::string& slotName);
}
