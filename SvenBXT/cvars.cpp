#include "cvars.hpp"

namespace CVars
{
	// Clientside CVars
	CVarWrapper con_color;

	CVarWrapper bxt_cross("bxt_cross", "0");
	CVarWrapper bxt_cross_color("bxt_cross_color", "");
	CVarWrapper bxt_cross_alpha("bxt_cross_alpha", "255");
	CVarWrapper bxt_cross_thickness("bxt_cross_thickness", "2");
	CVarWrapper bxt_cross_size("bxt_cross_size", "10");
	CVarWrapper bxt_cross_gap("bxt_cross_gap", "3");
	CVarWrapper bxt_cross_outline("bxt_cross_outline", "0");
	CVarWrapper bxt_cross_circle_radius("bxt_cross_circle_radius", "0");
	CVarWrapper bxt_cross_dot_color("bxt_cross_dot_color", "");
	CVarWrapper bxt_cross_dot_size("bxt_cross_dot_size", "0");
	CVarWrapper bxt_cross_top_line("bxt_cross_top_line", "1");
	CVarWrapper bxt_cross_bottom_line("bxt_cross_bottom_line", "1");
	CVarWrapper bxt_cross_left_line("bxt_cross_left_line", "1");
	CVarWrapper bxt_cross_right_line("bxt_cross_right_line", "1");

	CVarWrapper bxt_hud("bxt_hud", "1");
	CVarWrapper bxt_hud_color("bxt_hud_color", "100 130 200");
	CVarWrapper bxt_hud_precision("bxt_hud_precision", "6");
	CVarWrapper bxt_hud_speedometer("bxt_hud_speedometer", "1");
	CVarWrapper bxt_hud_speedometer_offset("bxt_hud_speedometer_offset", "");
	CVarWrapper bxt_hud_speedometer_anchor("bxt_hud_speedometer_anchor", "0.5 1");
	CVarWrapper bxt_hud_jumpspeed("bxt_hud_jumpspeed", "0");
	CVarWrapper bxt_hud_jumpspeed_offset("bxt_hud_jumpspeed_offset", "");
	CVarWrapper bxt_hud_jumpspeed_anchor("bxt_hud_jumpspeed_anchor", "0.5 1");
	CVarWrapper bxt_hud_viewangles("bxt_hud_viewangles", "0");
	CVarWrapper bxt_hud_viewangles_offset("bxt_hud_viewangles_offset", "");
	CVarWrapper bxt_hud_viewangles_anchor("bxt_hud_viewangles_anchor", "1 0");
	CVarWrapper bxt_hud_origin("bxt_hud_origin", "0");
	CVarWrapper bxt_hud_origin_offset("bxt_hud_origin_offset", "");
	CVarWrapper bxt_hud_origin_anchor("bxt_hud_origin_anchor", "1 0");

	const std::vector<CVarWrapper*> allCVars =
	{
		&con_color,
		&bxt_cross,
		&bxt_cross_color,
		&bxt_cross_alpha,
		&bxt_cross_thickness,
		&bxt_cross_size,
		&bxt_cross_gap,
		&bxt_cross_outline,
		&bxt_cross_circle_radius,
		&bxt_cross_dot_color,
		&bxt_cross_dot_size,
		&bxt_cross_top_line,
		&bxt_cross_bottom_line,
		&bxt_cross_left_line,
		&bxt_cross_right_line,
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
		&bxt_hud_viewangles_anchor,
		&bxt_hud_origin,
		&bxt_hud_origin_offset,
		&bxt_hud_origin_anchor
	};
}