#include "cvars.hpp"

namespace CVars
{
	// Clientside CVars
	CVarWrapper bxt_hud("bxt_hud", "1");
	CVarWrapper bxt_hud_speedometer("bxt_hud_speedometer", "0");
	CVarWrapper bxt_hud_speedometer_offset("bxt_hud_speedometer_offset", "");
	CVarWrapper bxt_hud_speedometer_anchor("bxt_hud_speedometer_anchor", "0.5 1");

	const std::vector<CVarWrapper*> allCVars =
	{
		&bxt_hud,
		&bxt_hud_speedometer,
		&bxt_hud_speedometer_offset,
		&bxt_hud_speedometer_anchor
	};
}