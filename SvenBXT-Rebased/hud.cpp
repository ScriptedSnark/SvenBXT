#include "ClientDLL.hpp"
#include "hud.hpp"
#include "opengl_utils.hpp"

#include <chrono>

#undef min

float hudTime;
bool DrawTimer = false;
float m_flTurnoff;

namespace CustomHud
{
	static const float FADE_DURATION_JUMPSPEED = 0.7f;
	static bool initialized = false;
	static client_sprite_t* SpriteList;
	static SCREENINFO si;
	static int precision;
	static playerinfo player;
	static float consoleColor[3];
	static int hudColor[3];
	static int SpriteCount;
	static std::array<HSPRITE_HL, 10> NumberSprites;
	static std::array<wrect_t, 10> NumberSpriteRects;
	static std::array<client_sprite_t*, 10> NumberSpritePointers;
	static int NumberWidth;
	static int NumberHeight;
	unsigned char custom_r, custom_g, custom_b;
	bool custom_hud_color_set = false;

	// bxt_hud_timer things
	static bool isTiming = false;
	double elapsedPaused = 0.0;
	bool isPaused = false;
	static std::chrono::high_resolution_clock::time_point startTime;
	static std::chrono::high_resolution_clock::time_point stopTime;

	static void UpdateScreenInfo()
	{
		si.iSize = sizeof(si);
		pEngfuncs->pfnGetScreenInfo(&si);
	}

	const SCREENINFO& GetScreenInfo()
	{
		return si;
	}

	template<typename T, size_t size = 3>
	static inline void vecCopy(const T src[], T dest[])
	{
		for (size_t i = 0; i < size; ++i)
			dest[i] = src[i];
	}

	static inline double sqr(double a)
	{
		return a * a;
	}

	static inline double length(double x, double y)
	{
		return std::hypot(x, y);
	}

	static inline double length(double x, double y, double z)
	{
		return std::sqrt(sqr(x) + sqr(y) + sqr(z));
	}

	static inline int DrawBitmap(int x, int y, const int bitmap[], int width, int height, int r, int g, int b) {
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
				pEngfuncs->pfnFillRGBA(x + j, y + i, 1, 1, r, g, b, bitmap[i * width + j]);

		return width;
	}

	static int DrawString(int x, int y, const char* s, float r, float g, float b, bool console = true)
	{
		pEngfuncs->pfnDrawSetTextColor(r, g, b);

		if (console)
			return pEngfuncs->pfnDrawConsoleString(x, y, const_cast<char*>(s));
		else
			return pEngfuncs->pfnDrawString(x, y, const_cast<char*>(s), r, g, b);
	}

	static inline int DrawString(int x, int y, const char* s, bool console = true)
	{
		return DrawString(x, y, s, consoleColor[0], consoleColor[1], consoleColor[2], console);
	}

	static int DrawMultilineString(int x, int y, std::string s, float r, float g, float b)
	{
		int max_new_x = 0;

		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			int new_x = DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()), r, g, b);
			max_new_x = max(new_x, max_new_x);
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};

		return max_new_x;
	}

	static int DrawMultilineString(int x, int y, std::string s)
	{
		int max_new_x = 0;

		while (s.size() > 0)
		{
			auto pos = s.find('\n');

			int new_x = DrawString(x, y, const_cast<char*>(s.substr(0, pos).c_str()));
			max_new_x = max(new_x, max_new_x);
			y += si.iCharHeight;

			if (pos != std::string::npos)
				s = s.substr(pos + 1, std::string::npos);
			else
				s.erase();
		};

		return max_new_x;
	}

	static void DrawDigit(int digit, int x, int y, int r, int g, int b)
	{
		//assert(digit >= 0 && digit <= 9);

		pEngfuncs->pfnSPR_Set(NumberSprites[digit], r, g, b);
		pEngfuncs->pfnSPR_DrawAdditive(0, x, y, &NumberSpriteRects[digit]);
	}

	static void DrawDot(int x, int y, int r, int g, int b)
	{
		const int Dot320[] = {
			143, 199, 122,
			255, 255, 218,
			120, 169, 95
		};

		const int Dot640[] = {
			21,  114, 128, 128, 83,  21,
			150, 255, 255, 255, 255, 104,
			239, 255, 255, 255, 255, 192,
			226, 255, 255, 255, 255, 165,
			114, 255, 255, 255, 255, 65,
			29,  43,  89,  89,  29,  29
		};

		if (si.iWidth < 640)
			DrawBitmap(x, y, Dot320, 3, 3, r, g, b);
		else
			DrawBitmap(x, y, Dot640, 6, 6, r, g, b);
	}

	static void DrawLine(int x, int y, int length, int r, int g, int b) {
		if ((si.iWidth < 640 && length < 3) || length < 5) {
			// might as well draw a dot
			DrawDot(x, y, r, g, b);
			return;
		}


		const int Line640_left[] = {
			21,  114,
			150, 255,
			239, 255,
			226, 255,
			114, 255,
			29,  43
		};

		const int Line640_repeat[] = {
			128,
			255,
			255,
			255,
			255,
			89
		};

		const int Line640_Right[] = {
			 83,  21,
			255, 104,
			255, 192,
			255, 165,
			255,  65,
			 29,  29
		};

		const int Line320_left[] = {
			143,
			255,
			120,
		};

		const int Line320_repeat[] = {
			199,
			255,
			169,
		};

		const int Line320_Right[] = {
			122,
			218,
			95
		};

		const int* line_left;
		const int* line_repeat;
		const int* line_right;
		int linesprite_height;
		int linesprite_width;

		if (si.iWidth <= 640) {
			line_left = Line320_left;
			line_repeat = Line320_repeat;
			line_right = Line320_Right;
			linesprite_height = 3;
			linesprite_width = 1;
		}
		else {
			line_left = Line640_left;
			line_repeat = Line640_repeat;
			line_right = Line640_Right;
			linesprite_height = 6;
			linesprite_width = 2;
		}

		y -= linesprite_height / 2;

		x += DrawBitmap(x, y, line_left, linesprite_width, linesprite_height, r, g, b);

		for (int xOffset = 0; xOffset < length - linesprite_width * 2; xOffset++) {
			x += DrawBitmap(x, y, line_repeat, 1, linesprite_height, r, g, b);
		}

		DrawBitmap(x, y, line_right, linesprite_width, linesprite_height, r, g, b);
	}

	static void AlignNumber(int alignment, int& x, int& y, int numberWidth, int numberHeight)
	{
		// Align to right
		if (alignment & (1 << 0))
			x -= numberWidth;

		// Align to center
		if (alignment & (1 << 1))
			x -= numberWidth / 2;

		/*
			// Align to left bottom
			if ( alignment & (1 << 2) )
				y += numberHeight / 2;

			// Align to left top
			if ( alignment & (1 << 3) )
				y -= numberHeight / 2;
		*/
	}

	static int DrawNumberXT(int number, int x, int y, int r, int g, int b, int fieldMinWidth = 1)
	{
		if (number < 0)
		{
			if (number == std::numeric_limits<int>::min())
			{
				number = 0;
			}
			else
			{
				number = abs(number);
				DrawLine(x - NumberWidth, y + NumberHeight / 2, NumberWidth, r, g, b);
			}
		}

		static_assert(sizeof(int) >= 4, "Int less than 4 bytes in size is not supported.");

		int digits[10] = { 0 };
		int i;
		for (i = 0; i < 10; ++i)
		{
			if (number == 0)
				break;

			digits[i] = number % 10;
			number /= 10;
		}

		for (; fieldMinWidth > 10; --fieldMinWidth)
		{
			DrawDigit(0, x, y, r, g, b);
			x += NumberWidth;
		}
		if (fieldMinWidth > i)
			i = fieldMinWidth;

		for (int j = i; j > 0; --j)
		{
			DrawDigit(digits[j - 1], x, y, r, g, b);
			x += NumberWidth;
		}

		return x;
	}


	static int DrawNumber(int number, int x, int y, int r, int g, int b, int alignment, int fieldMinWidth = 1)
	{
		bool bNegative = false;

		if (number < 0)
		{
			if (number == -2147483647)
			{
				number = 0;
			}
			else
			{
				number = abs(number);
				bNegative = true;
			}
		}

		static_assert(sizeof(int) >= 4, "Int less than 4 bytes in size is not supported.");

		int digits[10] = { 0 };
		int c = 0;
		int i;
		for (i = 0; i < 10; ++i)
		{
			if (number == 0)
				break;

			digits[i] = number % 10;
			number /= 10;

			c++;
		}

		AlignNumber(alignment, x, y, (fieldMinWidth >= c ? fieldMinWidth : c) * NumberWidth, NumberHeight);

		if (bNegative)
		{
			DrawLine(x - NumberWidth, y + NumberHeight / 2, NumberWidth, r, g, b);
		}

		for (; fieldMinWidth > 10; --fieldMinWidth)
		{
			DrawDigit(0, x, y, r, g, b);
			x += NumberWidth;
		}
		if (fieldMinWidth > i)
			i = fieldMinWidth;

		for (int j = i; j > 0; --j)
		{
			DrawDigit(digits[j - 1], x, y, r, g, b);
			x += NumberWidth;
		}

		return x;
	}

	static inline int DrawNumber(int number, int x, int y, int alignment, int fieldMinWidth = 1)
	{
		return DrawNumber(number, x, y, hudColor[0], hudColor[1], hudColor[2], alignment, fieldMinWidth); // TODO: customize BXT hud color
	}

	static void UpdateColors()
	{
		// Default: taken from con_color of HL 6153.
		consoleColor[0] = 1.0f;
		consoleColor[1] = 180 / 255.0f;
		consoleColor[2] = 30 / 255.0f;

		unsigned r = 0, g = 0, b = 0;
		std::istringstream ss(con_color->string);
		ss >> r >> g >> b;

		consoleColor[0] = r / 255.0f;
		consoleColor[1] = g / 255.0f;
		consoleColor[2] = b / 255.0f;

		// Default Sven Co-op HUD color
		hudColor[0] = 100;
		hudColor[1] = 130;
		hudColor[2] = 200;

		auto colorStr = bxt_hud_color->string;
		if (colorStr != "auto")
		{
			std::istringstream color_ss(colorStr);
			color_ss >> hudColor[0] >> hudColor[1] >> hudColor[2];
		}
	}

	static inline int DrawNumberTimer(int number, int x, int y, int fieldMinWidth = 1)
	{
		return DrawNumberXT(number, x, y, hudColor[0], hudColor[1], hudColor[2], fieldMinWidth);
	}

	static int DrawNumberTimer(int number, int x, int y, int r, int g, int b, int fieldMinWidth = 1)
	{
		return DrawNumberXT(number, x, y, r, g, b, fieldMinWidth);
	}

	static void DrawDecimalSeparator(int x, int y, int r, int g, int b)
	{
		x += (NumberWidth - 6) / 2;
		y += NumberHeight - 5;
		DrawDot(x + 1, y, r, g, b);
	}

	static void DrawDecimalSeparator(int x, int y)
	{
		return DrawDecimalSeparator(x, y, hudColor[0], hudColor[1], hudColor[2]);
	}

	static void DrawColon(int x, int y, int r, int g, int b)
	{
		x += (NumberWidth - 6) / 2;
		DrawDot(x + 1, y + 2, r, g, b);
		y += NumberHeight - 5;
		DrawDot(x + 1, y - 2, r, g, b);
	}

	static void DrawColon(int x, int y)
	{
		return DrawColon(x, y, hudColor[0], hudColor[1], hudColor[2]);
	}

	static void GetPosition(const char* Offset, const char* Anchor, int* x, int* y, int rx = 0, int ry = 0)
	{
		std::istringstream iss;

		iss.str(Offset);
		iss >> rx >> ry;
		iss.str(std::string());
		iss.clear();

		iss.str(Anchor);
		float w = 0, h = 0;
		iss >> w >> h;

		rx += static_cast<int>(w * si.iWidth);
		ry += static_cast<int>(h * si.iHeight);

		if (x) *x = rx;
		if (y) *y = ry;
	}

	static void UpdatePrecision()
	{
		precision = bxt_hud_precision->value;

		if (precision > 16)
			precision = 16;
		else if (precision < 0)
			precision = 6;
	}

	void UpdatePlayerInfo(float vel[3], float org[3], float viewangles[3]) {
		vecCopy(vel, player.velocity);
		vecCopy(org, player.origin);
		vecCopy(viewangles, player.viewangles);
	}

	static void DrawOrigin(float flTime)
	{
		if (bxt_hud_origin->value)
		{
			int x, y;
			GetPosition(bxt_hud_origin_offset->string, bxt_hud_origin_anchor->string, &x, &y, -200, (si.iCharHeight * 6) + 1);

			DrawString(x, y, "Origin:");

			y += si.iCharHeight;

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);

			out << "X: " << player.origin[0] << "\n"
				<< "Y: " << player.origin[1] << "\n"
				<< "Z: " << player.origin[2];

			DrawMultilineString(x, y, out.str());
		}
	}

	float AngleNormalize(float angle)
	{
		angle = fmodf(angle, 360.0f);
		if (angle > 180)
		{
			angle -= 360;
		}
		if (angle < -180)
		{
			angle += 360;
		}
		return angle;
	}

	void DrawViewangles(float flTime)
	{
		if (bxt_hud_viewangles->value)
		{
			int x, y;
			float viewangle[2];
			viewangle[0] = AngleNormalize(player.viewangles[0]);
			viewangle[1] = AngleNormalize(player.viewangles[1]);

			GetPosition(bxt_hud_viewangles_offset->string, bxt_hud_viewangles_anchor->string, &x, &y, -200, (si.iCharHeight * 10) + 2);

			std::ostringstream out;
			out.setf(std::ios::fixed);
			out.precision(precision);
			out << "Pitch: " << viewangle[0] << "\n"
				<< "Yaw: " << viewangle[1];

			DrawMultilineString(x, y, out.str());
		}
	}

	static void DrawSpeedometer()
	{
		if (bxt_hud_speedometer->value)
		{
			int x, y;
			GetPosition(bxt_hud_speedometer_offset->string, bxt_hud_speedometer_anchor->string, &x, &y, 0, -2 * NumberHeight);
			DrawNumber(static_cast<int>(trunc(length(player.velocity[0], player.velocity[1]))), x, y, (1 << 1));
		}
	}

	static void DrawJumpspeed(float flTime)
	{
		static float prevVel[3] = { 0.0f, 0.0f, 0.0f };

		if (bxt_hud_jumpspeed->value)
		{
			static float lastTime = flTime;
			static double passedTime = FADE_DURATION_JUMPSPEED;
			static int fadingFrom[3] = { hudColor[0], hudColor[1], hudColor[2] };
			static double jumpSpeed = 0.0;

			int r = hudColor[0],
				g = hudColor[1],
				b = hudColor[2];

			if (FADE_DURATION_JUMPSPEED > 0.0f)
			{
				if ((player.velocity[2] != 0.0f && prevVel[2] == 0.0f)
					|| (player.velocity[2] > 0.0f && prevVel[2] < 0.0f))
				{
					double difference = length(player.velocity[0], player.velocity[1]) - jumpSpeed;
					if (difference != 0.0f)
					{
						if (difference > 0.0f)
						{
							fadingFrom[0] = 0;
							fadingFrom[1] = 255;
							fadingFrom[2] = 0;
						}
						else
						{
							fadingFrom[0] = 255;
							fadingFrom[1] = 0;
							fadingFrom[2] = 0;
						}

						passedTime = 0.0;
						jumpSpeed = length(player.velocity[0], player.velocity[1]);
					}
				}

				// Can be negative if we went back in time (for example, loaded a save).
				double timeDelta = max(flTime - lastTime, 0.0f);
				passedTime += timeDelta;

				// Check for Inf, NaN, etc.
				if (passedTime > FADE_DURATION_JUMPSPEED || !std::isnormal(passedTime)) {
					passedTime = FADE_DURATION_JUMPSPEED;
				}

				float colorVel[3] = { hudColor[0] - fadingFrom[0] / FADE_DURATION_JUMPSPEED,
									  hudColor[1] - fadingFrom[1] / FADE_DURATION_JUMPSPEED,
									  hudColor[2] - fadingFrom[2] / FADE_DURATION_JUMPSPEED };
				r = static_cast<int>(hudColor[0] - colorVel[0] * (FADE_DURATION_JUMPSPEED - passedTime));
				g = static_cast<int>(hudColor[1] - colorVel[1] * (FADE_DURATION_JUMPSPEED - passedTime));
				b = static_cast<int>(hudColor[2] - colorVel[2] * (FADE_DURATION_JUMPSPEED - passedTime));

				lastTime = flTime;
			}

			int x, y;
			GetPosition(bxt_hud_jumpspeed_offset->string, bxt_hud_jumpspeed_anchor->string, &x, &y, 0, -3 * NumberHeight);
			DrawNumber(static_cast<int>(trunc(jumpSpeed)), x, y, r, g, b, (1 << 1));
		}

		vecCopy(player.velocity, prevVel);
	}

	static void DrawCrosshair(float time)
	{
		if (bxt_cross->value == 0.0f)
			return;

		float old_circle_radius = 0;
		std::vector<Vector2D> circle_points;

		float r = 0.0f, g = 0.0f, b = 0.0f;
		std::istringstream ss(bxt_cross_color->string);
		ss >> r >> g >> b;

		static float crosshairColor[3];
		crosshairColor[0] = r;
		crosshairColor[1] = g;
		crosshairColor[2] = b;

		float alpha = bxt_cross_alpha->value / 255.0f;

		Vector2D center(si.iWidth / 2.0f, si.iHeight / 2.0f);

		GLUtils gl;

		// Draw the outline.
		if (bxt_cross_outline->value > 0.0f) {
			gl.color(0.0f, 0.0f, 0.0f, alpha);
			gl.line_width(bxt_cross_outline->value);

			auto size = bxt_cross_size->value;
			auto gap = bxt_cross_gap->value;
			auto half_thickness = bxt_cross_thickness->value / 2.0f;
			auto half_width = bxt_cross_outline->value / 2.0f;
			auto offset = half_thickness + half_width;

			// Top line
			if (bxt_cross_top_line->value) {
				gl.line(Vector2D(center.x - offset, center.y - gap - size), Vector2D(center.x + offset, center.y - gap - size));
				gl.line(Vector2D(center.x + half_thickness, center.y - gap - size + half_width), Vector2D(center.x + half_thickness, center.y - gap - half_width));
				gl.line(Vector2D(center.x + offset, center.y - gap), Vector2D(center.x - offset, center.y - gap));
				gl.line(Vector2D(center.x - half_thickness, center.y - gap - half_width), Vector2D(center.x - half_thickness, center.y - gap - size + half_width));
			}

			// Bottom line
			if (bxt_cross_bottom_line->value) {
				gl.line(Vector2D(center.x - offset, center.y + gap + size), Vector2D(center.x + offset, center.y + gap + size));
				gl.line(Vector2D(center.x + half_thickness, center.y + gap + size - half_width), Vector2D(center.x + half_thickness, center.y + gap + half_width));
				gl.line(Vector2D(center.x + offset, center.y + gap), Vector2D(center.x - offset, center.y + gap));
				gl.line(Vector2D(center.x - half_thickness, center.y + gap + half_width), Vector2D(center.x - half_thickness, center.y + gap + size - half_width));
			}

			// Left line
			if (bxt_cross_left_line->value) {
				gl.line(Vector2D(center.x - gap - size, center.y - offset), Vector2D(center.x - gap - size, center.y + offset));
				gl.line(Vector2D(center.x - gap - size + half_width, center.y + half_thickness), Vector2D(center.x - gap - half_width, center.y + half_thickness));
				gl.line(Vector2D(center.x - gap, center.y + offset), Vector2D(center.x - gap, center.y - offset));
				gl.line(Vector2D(center.x - gap - half_width, center.y - half_thickness), Vector2D(center.x - gap - size + half_width, center.y - half_thickness));
			}

			// Right line
			if (bxt_cross_right_line->value) {
				gl.line(Vector2D(center.x + gap + size, center.y - offset), Vector2D(center.x + gap + size, center.y + offset));
				gl.line(Vector2D(center.x + gap + size - half_width, center.y + half_thickness), Vector2D(center.x + gap + half_width, center.y + half_thickness));
				gl.line(Vector2D(center.x + gap, center.y + offset), Vector2D(center.x + gap, center.y - offset));
				gl.line(Vector2D(center.x + gap + half_width, center.y - half_thickness), Vector2D(center.x + gap + size - half_width, center.y - half_thickness));
			}

			// Dot
			if (bxt_cross_dot_size->value > 0.0f) {
				auto size = bxt_cross_dot_size->value;
				auto offset = Vector2D(size / 2.0f, size / 2.0f);

				gl.line(Vector2D(center.x - offset.x - half_width, center.y - offset.y), Vector2D(center.x + offset.x + half_width, center.y - offset.y));
				gl.line(Vector2D(center.x + offset.x, center.y - offset.y + half_width), Vector2D(center.x + offset.x, center.y + offset.y - half_width));
				gl.line(Vector2D(center.x - offset.x, center.y - offset.y + half_width), Vector2D(center.x - offset.x, center.y + offset.y - half_width));
				gl.line(Vector2D(center.x - offset.x - half_width, center.y + offset.y), Vector2D(center.x + offset.x + half_width, center.y + offset.y));
			}
		}

		if (bxt_cross_color->string[0]) {
			gl.color(crosshairColor[0], crosshairColor[1], crosshairColor[2], alpha);
		}
		else {
			gl.color(0.0f, 255.0f, 0.0f, alpha);
		}

		// Draw the crosshairs.
		if (bxt_cross_thickness->value > 0.0f) {
			gl.line_width(bxt_cross_thickness->value);

			auto size = bxt_cross_size->value;
			auto gap = bxt_cross_gap->value;

			if (bxt_cross_top_line->value)
				gl.line(Vector2D(center.x, center.y - gap - size), Vector2D(center.x, center.y - gap));
			if (bxt_cross_bottom_line->value)
				gl.line(Vector2D(center.x, center.y + gap + size), Vector2D(center.x, center.y + gap));
			if (bxt_cross_left_line->value)
				gl.line(Vector2D(center.x - gap - size, center.y), Vector2D(center.x - gap, center.y));
			if (bxt_cross_right_line->value)
				gl.line(Vector2D(center.x + gap + size, center.y), Vector2D(center.x + gap, center.y));
		}

		// Draw the circle.
		if (bxt_cross_circle_radius->value > 0.0f) {
			gl.line_width(1.0f);

			auto radius = bxt_cross_circle_radius->value;
			if (old_circle_radius != radius) {
				// Recompute the circle points.
				circle_points = gl.compute_circle(radius);
				old_circle_radius = radius;
			}

			gl.circle(center, circle_points);
		}

		// Draw the dot.
		if (bxt_cross_dot_size->value > 0.0f) {
			float r = 0.0f, g = 0.0f, b = 0.0f;
			std::istringstream ss(bxt_cross_dot_color->string);
			ss >> r >> g >> b;

			static float crosshairDotColor[2];
			crosshairDotColor[0] = r;
			crosshairDotColor[1] = g;
			crosshairDotColor[2] = b;

			if (bxt_cross_dot_color->string[0]) {
				gl.color(crosshairDotColor[0], crosshairDotColor[1], crosshairDotColor[2], alpha);
			}
			else {
				gl.color(255.0f, 0.0f, 0.0f, alpha);
			}

			auto size = bxt_cross_dot_size->value;
			auto offset = Vector2D(size / 2.0f, size / 2.0f);

			gl.rectangle(center - offset, center + offset);
		}
	}

	/*
	void ScaleColors(int* r, int* g, int* b, int a)
	{
		if (custom_hud_color_set) {
			*r = custom_r;
			*g = custom_g;
			*b = custom_b;
		}

		if (bxt_hud_game_alpha.GetInt() >= 1 && bxt_hud_game_alpha.GetInt() <= 255)
			a = bxt_hud_game_alpha.GetInt(); // unused due to hud_alpha_default/_max in Sven Co-op
	}
	*/

	void DrawCustomTimer(float time)
	{
		if (!DrawTimer)
			return;

		if (m_flTurnoff < hudTime)
		{
			pEngfuncs->pfnPlaySoundByName("fvox/bell.wav", 1);
			DrawTimer = false;
			return;
		}

		char szText[32];
		float diff = m_flTurnoff - hudTime;
		sprintf_s(szText, "Timer: %.01f", diff);

		DrawString(si.iWidth / 2.1, si.iCharHeight * 4, szText);
	}

	double GetElapsedTime()
	{
		if (isTiming)
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		}
		else
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count();
		}
	}

	void DrawTime()
	{
		if (!bxt_hud_timer->value)
			return;

		double elapsed = GetElapsedTime();

		int days = elapsed / (1000 * 60 * 60 * 24);
		elapsed -= days * (1000 * 60 * 60 * 24);

		int hours = elapsed / (1000 * 60 * 60);
		elapsed -= hours * (1000 * 60 * 60);

		int minutes = elapsed / (1000 * 60);
		elapsed -= minutes * (1000 * 60);

		int seconds = elapsed / 1000;
		elapsed -= seconds * 1000;

		int milliseconds = elapsed;

		int x, y;
		GetPosition(bxt_hud_timer_offset->string, bxt_hud_timer_anchor->string, &x, &y, 0, 0);

		if (hours)
		{
			x = DrawNumberTimer(hours, x, y);
			DrawColon(x, y);
			x += NumberWidth;
		}

		if (hours || minutes)
		{
			int fieldMinWidth = (hours && minutes < 10) ? 2 : 1;
			x = DrawNumberTimer(minutes, x, y, fieldMinWidth);
			DrawColon(x, y);
			x += NumberWidth;
		}

		int fieldMinWidth = ((hours || minutes) && seconds < 10) ? 2 : 1;
		x = DrawNumberTimer(seconds, x, y, fieldMinWidth);

		DrawDecimalSeparator(x, y);
		x += NumberWidth;

		DrawNumberTimer(milliseconds, x, y, 3);
	}

	void StartTimer()
	{
		if (!isTiming)
		{
			if (isPaused)
			{
				startTime = std::chrono::high_resolution_clock::now() - std::chrono::milliseconds(static_cast<long long>(elapsedPaused));
				elapsedPaused = 0.0;
				isPaused = false;
			}
			else
			{
				startTime = std::chrono::high_resolution_clock::now();
			}
			isTiming = true;
		}
	}

	void StopTimer()
	{
		if (isTiming)
		{
			isTiming = false;
			stopTime = std::chrono::high_resolution_clock::now();
			elapsedPaused = GetElapsedTime();
			isPaused = true;
		}
	}

	void ResetTimer()
	{
		if (isTiming)
		{
			isTiming = false;
			isPaused = false;
		}
		elapsedPaused = 0.0;
		startTime = std::chrono::high_resolution_clock::now();
		stopTime = std::chrono::high_resolution_clock::now();
	}

	void Init()
	{
		SpriteList = nullptr;
		initialized = true;
	}

	void InitIfNecessary()
	{
		if (!initialized)
			Init();
	}

	void VidInit()
	{
		UpdateScreenInfo();

		int SpriteRes = (si.iWidth < 640) ? 320 : 640;

		// Based on a similar procedure from hud.cpp.
		if (!SpriteList)
		{
			SpriteList = pEngfuncs->pfnSPR_GetList(const_cast<char*>("sprites/hud.txt"), &SpriteCount);
			if (SpriteList)
			{
				for (client_sprite_t* p = SpriteList; p < (SpriteList + SpriteCount); ++p)
				{
					// If we have a sprite of the correct resolution which is named "number_x" where x is a digit
					if (p->iRes == SpriteRes
						&& strstr(p->szName, "number_") == p->szName
						&& *(p->szName + 8) == 0
						&& isdigit(*(p->szName + 7)))
					{
						int digit = *(p->szName + 7) - '0';
						NumberSpritePointers[digit] = p;
						NumberSpriteRects[digit] = p->rc;

						std::string path("sprites/");
						path += p->szSprite;
						path += ".spr";
						NumberSprites[digit] = pEngfuncs->pfnSPR_Load(path.c_str());

						if (!digit)
						{
							NumberWidth = p->rc.right - p->rc.left;
							NumberHeight = p->rc.bottom - p->rc.top;
						}

						pEngfuncs->Con_DPrintf("[client dll] Loaded the digit %d sprite from \"%s\".\n", digit, path.c_str());
					}
				}
			}
		}
		else
		{
			size_t i = 0;
			for (auto it = NumberSpritePointers.cbegin(); it != NumberSpritePointers.cend(); ++i, ++it)
			{
				std::string path("sprites/");
				path += (*it)->szSprite;
				path += ".spr";
				NumberSprites[i] = pEngfuncs->pfnSPR_Load(path.c_str());

				// pEngfuncs->Con_DPrintf("[client dll] Reloaded the digit %d sprite from \"%s\".\n", i, path.c_str());
			}
		}
	}

	void Draw(float flTime)
	{
		hudTime = flTime;

		if (bxt_hud->value == 0.0f)
			return;

		UpdateColors();
		UpdatePrecision();

		DrawSpeedometer();
		DrawViewangles(flTime);
		DrawJumpspeed(flTime);
		DrawOrigin(flTime);
		DrawCrosshair(flTime);
		DrawCustomTimer(flTime);
		DrawTime();
	}

	void V_CalcRefdef(struct ref_params_s* pparams)
	{
		UpdatePlayerInfo(pparams->simvel, pparams->simorg, pparams->cl_viewangles);
	}
}