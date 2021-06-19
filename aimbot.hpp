#pragma once
#include "./includes.hpp"

inline auto move_to(float x, float y) -> void {
	float center_x = (ImGui::GetIO().DisplaySize.x / 2);
	float center_y = (ImGui::GetIO().DisplaySize.y / 2);

	float smooth = 2.f;

	float target_x = 0.f;
	float target_y = 0.f;

	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(target_x), static_cast<DWORD>(target_y), 0, 0);
}

inline auto get_best_target() -> entity_t {
	float best_dist = FLT_MAX;
	entity_t best_target = {};

	for (auto i = 1; i < globals::entities.size(); i++) {
		auto entity = globals::entities[i];
		if (!entity.controller || !entity.pawn) continue;

		vec3_t head = get_bone_pos_by_id(entity.pawn, 11);

		vec3_t w2s_head{};
		if (!globals::camera_manager->world_to_screen(head, w2s_head)) continue;

		float x = w2s_head.x - globals::width / 2.f;
		float y = w2s_head.y - globals::height / 2.f;
		float crosshair_dist = sqrtf((x * x) + (y * y));

		if (crosshair_dist <= FLT_MAX && crosshair_dist <= best_dist) {
			if (crosshair_dist > 150.f)
				continue;

			best_dist = crosshair_dist;
			best_target = entity;
		}
	}

	return best_target;
}

inline auto aimbot() -> void {
	if (!globals::aimbot) return;

	if (GetAsyncKeyState(VK_RBUTTON) && GetForegroundWindow() == FindWindowA(E("R6Game"), 0)) {
		auto target = get_best_target();
		if (!target.controller || !target.pawn) return;
		
		vec3_t head = get_bone_pos_by_id(target.pawn, 11);

		vec3_t w2s_head{};
		if (!globals::camera_manager->world_to_screen(head, w2s_head)) return;

		move_to(w2s_head.x, w2s_head.y); // this method is extremely shitty, but it works and my brain is fried for tonight
	}
}
