#pragma once

class HUDManager
{
public:

	bool SmoothCamInstalled = false;

	double maxOpacity = 100;
	double fadeSpeed = 1.0;  // seconds

	bool visible = true;

	double alpha = 0.0;
	double sneakAlpha = 0.0;
	double compassAlpha = 0.0;

	double prevDelta = 0.0;

	double fadeMult = 1.0;
	double prevFadeMult = 0.0;
	
	[[nodiscard]] static HUDManager* GetSingleton()
	{
		static HUDManager singleton;
		return std::addressof(singleton);
	}

	static void InstallHooks()
	{
		Hooks::Install();
	}

	bool ValidPickType();
	bool ValidCastType(RE::ActorMagicCaster* magicCaster);
	bool ValidAttackType(RE::PlayerCharacter* player);

	void UpdateCrosshair();
	void UpdateStealthAnim(RE::GFxValue sneakAnim);
	void UpdateHUD(RE::PlayerCharacter* player, double detectionLevel, RE::GFxValue sneakAnim);


protected:

	struct Hooks
	{
		struct PlayerCharacter_Update
		{
			static void thunk(RE::PlayerCharacter* player, float delta)
			{
				func(player, delta);
				auto manager = GetSingleton();
				manager->prevDelta = delta;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		struct StealthMeter_Update
		{
			static char thunk(RE::StealthMeter* a1, int64_t a2, int64_t a3, int64_t a4)
			{
				auto result = func(a1, a2, a3, a4);

				auto detectionLevel = static_cast<double>(a1->unk88);
				auto sneakAnim = a1->sneakAnim;

				auto player = RE::PlayerCharacter::GetSingleton();
				if (player) {
					SKSE::GetTaskInterface()->AddUITask([player, detectionLevel, sneakAnim]() {
						GetSingleton()->UpdateHUD(player, detectionLevel, sneakAnim);
					});
				}
				return result;
			}
			static inline REL::Relocation<decltype(thunk)> func;
		};

		static void Install()
		{
			stl::write_vfunc<RE::PlayerCharacter, 0xAD, PlayerCharacter_Update>();
			stl::write_vfunc<0x1, StealthMeter_Update>(RE::VTABLE_StealthMeter[0]);
		}
	};


private:
	constexpr HUDManager() noexcept = default;
	HUDManager(const HUDManager&) = delete;
	HUDManager(HUDManager&&) = delete;

	~HUDManager() = default;

	HUDManager& operator=(const HUDManager&) = delete;
	HUDManager& operator=(HUDManager&&) = delete;


};
