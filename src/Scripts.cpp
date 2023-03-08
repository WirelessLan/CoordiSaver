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

    void Install(RE::BSScript::IVirtualMachine* a_vm) {
		a_vm->BindNativeMethod("CoordiSaver"sv, "ShowMenu"sv, ShowMenu);
		a_vm->BindNativeMethod("CoordiSaver"sv, "LoadSlotFromFile"sv, LoadSlotFromFile);
    }
}
