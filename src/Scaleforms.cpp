#include "Scaleforms.h"

#include <fstream>

#include "CoordiSlots.h"
#include "InputMap.h"
#include "Utils.h"

namespace Scaleforms {
	const std::string_view MenuName = "CoordiSaverMenu";
	bool g_menuAllowTextInput = false;

	class CoordiSaverMenu : public RE::IMenu {
	public:
		CoordiSaverMenu() : RE::IMenu() {
			Instance = this;
			Instance->menuFlags = (RE::UI_MENU_FLAGS)0;
			Instance->UpdateFlag(RE::UI_MENU_FLAGS::kUsesCursor, true);
			Instance->UpdateFlag(RE::UI_MENU_FLAGS::kPausesGame, true);
			Instance->inputEventHandlingEnabled = false;

			RE::BSScaleformManager* g_scaleformManager = RE::BSScaleformManager::GetSingleton();
			if (g_scaleformManager)
				g_scaleformManager->LoadMovie(*Instance, uiMovie, MenuName.data(), "root1");
		}

		~CoordiSaverMenu() {
			Instance = nullptr;
		}

		bool ShouldHandleEvent(const RE::InputEvent*) override {
			return true;
		}

		void OnButtonEvent(const RE::ButtonEvent* inputEvent) override {
			uint32_t keyCode;

			if (g_menuAllowTextInput)
				return;

			// Mouse
			if (inputEvent->device == RE::INPUT_DEVICE::kMouse)
				keyCode = InputMap::GetMouseKeycode(inputEvent->idCode);
			// Gamepad
			else if (inputEvent->device == RE::INPUT_DEVICE::kGamepad)
				keyCode = InputMap::GamepadMaskToKeycode(inputEvent->idCode);
			// Keyboard
			else
				keyCode = inputEvent->idCode;

			// Valid scancode?
			if (!InputMap::IsValidKeycode(keyCode))
				return;

			keyCode = InputMap::ReplaceKeyCodeForMenu(keyCode);

			bool isDown = inputEvent->value == 1.0f;

			sendKeyEvent(keyCode, isDown);
		}

		void OnThumbstickEvent(const RE::ThumbstickEvent* inputEvent) override {
			if (inputEvent->idCode != RE::ThumbstickEvent::THUMBSTICK_ID::kLeft)
				return;

			uint32_t prevKeyCode = InputMap::DirectionToKeyCode(inputEvent->prevDir);
			uint32_t currKeyCode = InputMap::DirectionToKeyCode(inputEvent->currDir);
			if (currKeyCode != 0xFF) {
				sendKeyEvent(currKeyCode, true);
			}
			else {
				if (currKeyCode == prevKeyCode) {
					sendKeyEvent(0x25, false);
					sendKeyEvent(0x26, false);
					sendKeyEvent(0x27, false);
					sendKeyEvent(0x28, false);
				}
				else {
					sendKeyEvent(prevKeyCode, false);
				}
			}
		}

		static CoordiSaverMenu* GetSingleton() {
			return Instance;
		}

	protected:
		static CoordiSaverMenu* Instance;

	private:
		void sendKeyEvent(uint32_t keyCode, bool isDown) {
			CoordiSaverMenu* g_menuInst = CoordiSaverMenu::GetSingleton();
			if (!g_menuInst)
				return;

			RE::Scaleform::GFx::ASMovieRootBase* movieRoot = g_menuInst->uiMovie->asMovieRoot.get();
			RE::Scaleform::GFx::Value root;
			if (!movieRoot || !movieRoot->GetVariable(&root, "root")) {
				logger::critical("SendKeyEvent: Couldn't get a root");
				return;
			}

			RE::Scaleform::GFx::Value params[2];
			params[0] = keyCode;
			params[1] = isDown;

			root.Invoke("ProcessKeyEvent", nullptr, params, 2);
		}
	};

	CoordiSaverMenu* CoordiSaverMenu::Instance = nullptr;

	class Localizations {
	public:
		Localizations() {}

		static Localizations* GetSingleton() {
			static Localizations loc;
			return &loc;
		}

		std::string_view lang;
		std::unordered_map<std::string, std::string> translationsMap;
	};

	class ThrowHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			if (a_params.argCount > 0)
				logger::critical(FMT_STRING("Throw Message: {}"), a_params.args[0].GetString());

			CloseMenu();
		}
	};

	class CloseHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params&) override {
			CloseMenu();
		}
	};

	class AllowTextInputHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			g_menuAllowTextInput = a_params.args[0].GetBoolean();
			Utils::AllowTextInput(g_menuAllowTextInput);
		}
	};

	class GetInitializationDataHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			RE::IMenu* g_menu = CoordiSaverMenu::GetSingleton();
			g_menu->inputEventHandlingEnabled = true;

			RE::Scaleform::GFx::ASMovieRootBase* movieRoot = a_params.movie->asMovieRoot.get();
			if (!movieRoot) {
				logger::critical("GetInitializationDataHandler: Couldn't get a movieRoot!"sv);
				return;
			}

			Localizations* g_loc = Localizations::GetSingleton();
			if (!g_loc) {
				logger::critical("GetInitializationDataHandler: Couldn't get the localization data!"sv);
				return;
			}

			movieRoot->CreateObject(a_params.retVal);

			RE::Scaleform::GFx::Value locVal;
			movieRoot->CreateObject(&locVal);
			for (const auto& transPair : g_loc->translationsMap)
				locVal.SetMember(transPair.first, RE::Scaleform::GFx::Value(transPair.second.c_str()));

			a_params.retVal->SetMember("Language", g_loc->lang.data());
			a_params.retVal->SetMember("Localizations", locVal);
		}
	};

	class InitializationCompleteHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			RE::Scaleform::GFx::ASMovieRootBase* movieRoot = a_params.movie->asMovieRoot.get();
			RE::Scaleform::GFx::Value root;
			if (!movieRoot || !movieRoot->GetVariable(&root, "root")) {
				logger::critical("InitializationCompleteHandler: Couldn't get a root!"sv);
				return;
			}

			root.Invoke("ShowMenu", nullptr, nullptr, 0);
		}
	};

	class GetSlotListHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			RE::Scaleform::GFx::ASMovieRootBase* movieRoot = a_params.movie->asMovieRoot.get();
			if (!movieRoot) {
				logger::critical("GetSlotListHandler: Couldn't get a movieRoot!"sv);
				return;
			}

			movieRoot->CreateArray(a_params.retVal);

			std::vector<std::string> slotList = CoordiSlots::GetSlotList();
			for (const auto& slot : slotList) {
				RE::Scaleform::GFx::Value slotName(slot.c_str());
				a_params.retVal->PushBack(slotName);
			}
		}
	};

	class AddSlotHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			std::string slotName = a_params.args[0].GetString();
			a_params.retVal->operator=(CoordiSlots::SaveSlot(slotName));
		}
	};

	class DeleteSlotHandler : public RE::Scaleform::GFx::FunctionHandler {
	public:
		virtual void Call(const Params& a_params) override {
			std::string slotName = a_params.args[0].GetString();
			a_params.retVal->operator=(CoordiSlots::DeleteSlot(slotName));
		}
	};

	void RegisterMenu() {
		RE::UI* g_ui = RE::UI::GetSingleton();
		if (g_ui) {
			g_ui->RegisterMenu(MenuName.data(), [](const RE::UIMessage&) -> RE::IMenu* {
				CoordiSaverMenu* menu = CoordiSaverMenu::GetSingleton();
				if (!menu)
					menu = new CoordiSaverMenu();
				return menu;
			});
			logger::info("Menu Registered"sv);
		}
	}

	void LoadLocalizations() {
		Localizations* loc = Localizations::GetSingleton();

		loc->lang = "en";	// Default sLanguage
		RE::INISettingCollection* g_iniSettings = RE::INISettingCollection::GetSingleton();
		if (g_iniSettings) {
			for (RE::Setting* set : g_iniSettings->settings) {
				if (set->GetKey() == "sLanguage:General"sv) {
					loc->lang = set->GetString();
					break;
				}
			}
		}

		std::string transPath = fmt::format(FMT_STRING("Data\\Interface\\Translations\\{}_{}.txt"), MenuName, loc->lang);
		std::ifstream transFile(transPath);
		if (!transFile.is_open()) {
			bool noTrans = false;

			if (loc->lang != "en") {
				logger::warn(FMT_STRING("Cannot open the translation file: {}"), transPath);

				transPath = fmt::format(FMT_STRING("Data\\Interface\\Translations\\{}_en.txt"), MenuName);
				transFile.clear();
				transFile.open(transPath);
				if (!transFile.is_open())
					noTrans = true;
			}
			else
				noTrans = true;

			if (noTrans) {
				logger::warn(FMT_STRING("Cannot find the translation file: {}"), transPath);
				return;
			}
		}

		std::string line;
		std::string name, value;
		while (std::getline(transFile, line)) {
			Utils::Trim(line);
			if (line.empty() || line.starts_with('#'))
				continue;

			uint32_t index = 0;

			name = Utils::GetNextData(line, index, '\t');
			if (name.empty()) {
				logger::warn(FMT_STRING("Cannot read the name: {}"), line);
				continue;
			}

			value = Utils::GetNextData(line, index, 0);
			if (value.empty()) {
				logger::warn(FMT_STRING("Cannot read the value: {}"), line);
				continue;
			}

			loc->translationsMap.insert(std::make_pair(name, value));
		}
	}

	void RegisterFunction(RE::Scaleform::GFx::Movie* a_view, RE::Scaleform::GFx::Value* a_f4se_root, RE::Scaleform::GFx::FunctionHandler* a_handler, F4SE::stl::zstring a_name) {
		RE::Scaleform::GFx::Value fn;
		a_view->CreateFunction(&fn, a_handler);
		a_f4se_root->SetMember(a_name, fn);
	}

	void RegisterFunctions(RE::Scaleform::GFx::Movie* a_view, RE::Scaleform::GFx::Value* a_f4se_root) {
		RegisterFunction(a_view, a_f4se_root, new ThrowHandler(), "Throw"sv);
		RegisterFunction(a_view, a_f4se_root, new CloseHandler(), "Close"sv);
		RegisterFunction(a_view, a_f4se_root, new AllowTextInputHandler(), "AllowTextInput"sv);
		RegisterFunction(a_view, a_f4se_root, new GetInitializationDataHandler(), "GetInitializationData"sv);
		RegisterFunction(a_view, a_f4se_root, new InitializationCompleteHandler(), "InitializationComplete"sv);
		RegisterFunction(a_view, a_f4se_root, new GetSlotListHandler(), "GetSlotList"sv);
		RegisterFunction(a_view, a_f4se_root, new AddSlotHandler(), "AddSlot"sv);
		RegisterFunction(a_view, a_f4se_root, new DeleteSlotHandler(), "DeleteSlot"sv);
	}

	void OpenMenu() {
		RE::UIMessageQueue* g_uiMessageQueue = RE::UIMessageQueue::GetSingleton();
		if (g_uiMessageQueue)
			g_uiMessageQueue->AddMessage(MenuName, RE::UI_MESSAGE_TYPE::kShow);
	}

	void CloseMenu() {
		RE::UIMessageQueue* g_uiMessageQueue = RE::UIMessageQueue::GetSingleton();
		if (g_uiMessageQueue)
			g_uiMessageQueue->AddMessage(MenuName, RE::UI_MESSAGE_TYPE::kHide);
	}
}
