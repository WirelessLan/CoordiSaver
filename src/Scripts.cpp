#include "Scripts.h"
#include "Scaleforms.h"
#include "CoordiSlots.h"

namespace Scripts {
	void ShowMenu(std::monostate) {
		Scaleforms::OpenMenu();
	}

	std::vector<RE::TESForm*> LoadSlotFromFile(std::monostate, std::string slotName) {
		return CoordiSlots::LoadSlot(slotName);
	}

	std::string GetRandomSlot(std::monostate) {
		std::vector<std::string> slotList = CoordiSlots::GetSlotList();
		if (slotList.empty())
			return std::string();

		// Initialize a random number generator
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<std::size_t> distribution(0, slotList.size() - 1);

		// Generate a random index
		std::size_t randomIndex = distribution(gen);

		// Return the random string
		return slotList[randomIndex];
	}

    void Install(RE::BSScript::IVirtualMachine* a_vm) {
		a_vm->BindNativeMethod("CoordiSaver"sv, "ShowMenu"sv, ShowMenu);
		a_vm->BindNativeMethod("CoordiSaver"sv, "LoadSlotFromFile"sv, LoadSlotFromFile);
		a_vm->BindNativeMethod("CoordiSaver"sv, "GetRandomSlot"sv, GetRandomSlot);
    }
}
