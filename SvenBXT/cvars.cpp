#include "cvars.hpp"

namespace CVars
{
	// Clientside CVars
	CVarWrapper con_color;

	CVarWrapper bxt_hud("bxt_hud", "1");
	CVarWrapper bxt_hud_color("bxt_hud_color", "100 130 200");
	CVarWrapper bxt_hud_precision("bxt_hud_precision", "6");
	CVarWrapper bxt_hud_speedometer("bxt_hud_speedometer", "0");
	CVarWrapper bxt_hud_speedometer_offset("bxt_hud_speedometer_offset", "");
	CVarWrapper bxt_hud_speedometer_anchor("bxt_hud_speedometer_anchor", "0.5 1");
	CVarWrapper bxt_hud_jumpspeed("bxt_hud_jumpspeed", "0");
	CVarWrapper bxt_hud_jumpspeed_offset("bxt_hud_jumpspeed_offset", "");
	CVarWrapper bxt_hud_jumpspeed_anchor("bxt_hud_jumpspeed_anchor", "0.5 1");
	CVarWrapper bxt_hud_viewangles("bxt_hud_viewangles", "0");
	CVarWrapper bxt_hud_viewangles_offset("bxt_hud_viewangles_offset", "");
	CVarWrapper bxt_hud_viewangles_anchor("bxt_hud_viewangles_anchor", "1 0");

	const std::vector<CVarWrapper*> allCVars =
	{
		&con_color,
		&bxt_hud,
		&bxt_hud_color,
		&bxt_hud_precision,
		&bxt_hud_speedometer,
		&bxt_hud_speedometer_offset,
		&bxt_hud_speedometer_anchor,
		&bxt_hud_jumpspeed,
		&bxt_hud_jumpspeed_offset,
		&bxt_hud_jumpspeed_anchor,
		&bxt_hud_viewangles,
		&bxt_hud_viewangles_offset,
		&bxt_hud_viewangles_anchor
	};
}