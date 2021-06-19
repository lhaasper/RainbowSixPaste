#include "./includes.hpp"
#include <urlmon.h>
#pragma comment (lib, "urlmon.lib")
#include <Windows.h>
auto cache() -> void {
	while (1) {
		std::vector<entity_t> tmp = {};
		for (auto i = 0U; i < globals::game_manager->get_controller_count(); i++) {
			auto controller = impl::memory->read<std::uintptr_t>(globals::game_manager->get_controller_list() + (i * sizeof(std::uintptr_t)));
			if (!controller) continue;

			auto pawn = _rotl64(_rotl64(impl::memory->read<std::uintptr_t>(controller + 0x48), 56) + 0x24118AA86E8F1338i64, 12);
			if (!pawn) continue;

			entity_t entity{ };
			entity.controller = controller;
			entity.pawn = pawn;

			tmp.push_back(entity);
		}

		globals::entities = tmp;
		th::sleep_for(ch::milliseconds(250));
	}
}

auto aimbot_thread() -> void {
	while (1) {
		aimbot();
		th::sleep_for(ch::milliseconds(10));
	}
}

auto main(int argc, char** argv) -> int {
	system("taskkill /F /IM RainbowSix.ex");
	std::printf(E("Downloading assets...\n"));
	URLDownloadToFile(NULL, "https://lhaasper.de/beta/BEDaisy.sys", "C:\\BEDaisy.sys", 0, NULL);
	std::printf(E("Downloading assets... (1/2)\n"));

	URLDownloadToFile(NULL, "https://lhaasper.de/beta/kdmapper.exe", "C:\\kdmapper.exe", 0, NULL);
	std::printf(E("Downloading assets... (2/2)... wait...\n"));

	Sleep(4000);
	system("C:\\kdmapper.exe C:\\BEDaisy.sys");

	do {
		clear_console();
		std::printf(E("Please open R6s\n"));
		th::sleep_for(ch::seconds(1));
	} while (!FindWindowA(E("R6Game"), 0));

	clear_console();

	impl::memory = std::make_unique<memory_mgr>(E_("xy0xy0xy0xy0xy0EternityFREEEEER6"), E_("RainbowSix.exe"));
	impl::c_base = impl::memory->base_address();

	globals::game_manager = std::make_unique<c_gamemanager>(_rotl64(impl::memory->read<std::uintptr_t>(impl::c_base + 0x72A43C8) + 0x77FF4DDF516EED3Fi64, 36) - 89i64);
	globals::profile_manager = std::make_unique<c_profilemanager>((_rotl64(impl::memory->read<std::uintptr_t>(impl::c_base + 0x613E9E8), 11) ^ 0xBE733C59AB105A9Fui64) - 0x7469A87D6DE958A8i64);
	globals::round_manager = std::make_unique<c_roundmanager>(_rotl64(impl::memory->read<std::uintptr_t>(impl::c_base + 0x74022D0) - 0x64, 47) ^ 0x678986F9FD521FF9);
	globals::camera_manager = std::make_unique<c_cameramanager>(impl::memory->read<std::uintptr_t>(impl::memory->read<std::uintptr_t>(impl::memory->read<std::uintptr_t>(impl::c_base + 0x5EBCE60) + 0x40)));

	std::thread(cache).detach();
	std::thread(aimbot_thread).detach();

	setup_window();
	setup_directx(MyWnd);
	main_loop();
	return 1;
}