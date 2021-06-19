#pragma once
#include "./includes.hpp"

inline auto esp() -> void {
	if (!globals::esp) return;

	vec3_t local_head = get_bone_pos_by_id(globals::entities[0].pawn, 11);
	for (auto i = 1; i < globals::entities.size(); i++) {
		auto entity = globals::entities[i];
		if (!entity.controller || !entity.pawn) continue;

		vec3_t head = get_bone_pos_by_id(entity.pawn, 11);
		if (local_head.dist(head) >= 150.f) continue;

		vec3_t foot = get_bone_pos_by_id(entity.pawn, 0);

		vec3_t w2s_head{};
		if (!globals::camera_manager->world_to_screen(head, w2s_head)) continue;

		vec3_t w2s_foot{};
		if (!globals::camera_manager->world_to_screen(foot, w2s_foot)) continue;

		float height = w2s_foot.y - w2s_head.y;
		float width = height / 1.5f;

		ImGui::GetWindowDrawList()->AddRect({ w2s_head.x - (width / 2.f), w2s_head.y - 10.f }, { w2s_head.x + (width / 2.f), w2s_head.y + height }, ImColor(0.f, 0.f, 1.f));
	}
}