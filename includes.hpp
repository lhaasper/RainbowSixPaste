#pragma once
#include <iostream>
#include <intrin.h>
#include "./crypt.hpp"
#include "./mem_class.hpp"
#include "./other_globals.hpp"

struct vec3_t {
	float x, y, z;

	vec3_t() : x(0.f), y(0.f), z(0.f) {}
	vec3_t(float x) : x(x), y(0.f), z(0.f) {}
	vec3_t(float x, float y) : x(x), y(y), z(0.f) {}
	vec3_t(float x, float y, float z) : x(x), y(y), z(z) {}

	vec3_t operator+(vec3_t b) {
		return vec3_t(this->x + b.x, this->y + b.y, this->z + b.z);
	}

	vec3_t operator-(vec3_t b) {
		return vec3_t(this->x - b.x, this->y - b.y, this->z - b.z);
	}

	vec3_t operator*(float b) {
		return vec3_t(this->x * b, this->y * b, this->z * b);
	}

	vec3_t operator*(vec3_t b) {
		return vec3_t(this->x * b.x, this->y * b.y, this->z * b.z);
	}

	float dist(vec3_t b) {
		return sqrtf((this->x - b.x) * (this->x - b.x) + (this->y - b.y) * (this->y - b.y) + (this->z - b.z) * (this->z - b.z));
	}

	float dot(vec3_t b)
	{
		return ((this->x * b.x) + (this->y * b.y) + (this->z * b.z));
	}

	void normalize()
	{
		while (this->x > 89.0f)
			this->x -= 180.f;

		while (this->x < -89.0f)
			this->x += 180.f;

		while (this->y > 180.f)
			this->y -= 360.f;

		while (this->y < -180.f)
			this->y += 360.f;
	}

	void clamp_angle() {
		if (this->x < -89.f) this->x = -89.f;
		else if (this->x > 89.f) this->x = 89.f;
		while (this->y < -180.f) this->y += 360.f;
		while (this->y > 180.f) this->y -= 360.f;
		this->z = 0.f;
	}
};

void clear_console() { /* credits: http://cplusplus.com/forum/general/109841/#msg599344 */
    DWORD n{ 0 }, size{ 0 };
    COORD coord{ 0, 0 };

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE)
        return;

    CONSOLE_SCREEN_BUFFER_INFO csbi{ };
    if(!GetConsoleScreenBufferInfo(h, &csbi))
        return;
    size = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacterA(h, ' ', size, coord, &n);
    GetConsoleScreenBufferInfo(h, &csbi);
    FillConsoleOutputAttribute(h, csbi.wAttributes, size, coord, &n);
    SetConsoleCursorPosition(h, coord);
}

class c_gamemanager;
class c_profilemanager;
class c_roundmanager;
class c_cameramanager;

typedef struct _entity_t {
    std::uintptr_t controller;
    std::uintptr_t pawn;
} entity_t, * p_entity_t;
namespace globals {
    std::unique_ptr<c_gamemanager> game_manager = nullptr;
    std::unique_ptr<c_profilemanager> profile_manager = nullptr;
    std::unique_ptr<c_roundmanager> round_manager = nullptr;
    std::unique_ptr<c_cameramanager> camera_manager = nullptr;

	float width = 1920.f, height = 1080.f;

	bool aimbot = false;
	bool esp = false;

	std::vector<entity_t> entities = { { } };
}

auto transforms_calculation(__int64 bones, __m128* result_pos, __m128* bone_info) -> void
{
	__m128 v5; // xmm2
	__m128 v6; // xmm3
	__m128 v7; // xmm0
	__m128 v8; // xmm4
	__m128 v9; // xmm1

	__m128 v10 = { 0.500f, 0.500f, 0.500f, 0.500f };
	__m128 v11 = { 2.000f, 2.000f, 2.000f, 0.000f };

	__m128 v12 = impl::memory->read<__m128>(bones);
	__m128 v13 = impl::memory->read<__m128>(bones + 0x10);

	v5 = v13;

	v6 = _mm_mul_ps(*(__m128*)bone_info, v5);
	v6.m128_f32[0] = v6.m128_f32[0]
		+ (float)(_mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0x55)) + _mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0xAA)));
	v7 = _mm_shuffle_ps(v13, v5, 0xFF);
	v8 = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(v5, v5, 0xD2), *(__m128*)bone_info),
		_mm_mul_ps(_mm_shuffle_ps(*(__m128*)bone_info, *(__m128*)bone_info, 0xD2), v5));
	v9 = _mm_shuffle_ps(v12, v12, 0x93);
	v9.m128_f32[0] = 0.0;
	*(__m128*)result_pos = _mm_add_ps(
		_mm_shuffle_ps(v9, v9, 0x39),
		_mm_mul_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(_mm_shuffle_ps(v8, v8, 0xD2), v7),
					_mm_mul_ps(_mm_shuffle_ps(v6, v6, 0), v5)),
				_mm_mul_ps(
					_mm_sub_ps(_mm_mul_ps(v7, v7), (__m128)v10),
					*(__m128*)bone_info)),
			(__m128)v11));
}

auto get_bone_pos_by_id(std::uintptr_t pawn, int id) -> vec3_t
{
    __m128 Output{};

    std::uintptr_t pBonesChain2 = (_rotl64(impl::memory->read<std::uintptr_t>(pawn + 0xC90), 0x20) + 0x3E5EB88C03216100i64) ^ 0x45AEBF5654B57B72i64;
    pBonesChain2 = impl::memory->read<std::uintptr_t>(pBonesChain2);

    std::uintptr_t pBones = impl::memory->read<std::uintptr_t>(pBonesChain2 + 0x238);
    std::uintptr_t pBonesData = impl::memory->read<std::uintptr_t>(pBones + 0x58);

	std::uintptr_t idarray = impl::memory->read<std::uintptr_t>(pBonesChain2 + 0xB8);
    id = impl::memory->read<int>(idarray + id * 0x4);

    __m128 BoneInfo = impl::memory->read<__m128>((0x20 * id) + pBonesData);
	transforms_calculation(pBones, &Output, &BoneInfo);

    return { Output.m128_f32[0], Output.m128_f32[1], Output.m128_f32[2] };
}

#include "./sdk/gamemanager.hpp"
#include "./sdk/profilemanager.hpp"
#include "./sdk/roundmanager.hpp"
#include "./sdk/cameramanager.hpp"

#include "./overlay.hpp"

#include "./aimbot.hpp"