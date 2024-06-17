#pragma once

struct Mouse
{
public:
	void MouseMove(double xpos, double ypos);
	void MouseButtonPress(int button, int action, int mods);
	void MouseScroll(double xoffset, double yoffset);

	auto x_pos();
	auto y_pos();
	auto delta_x();
	auto delta_y();

private:
	float m_last_x, m_last_y, m_delta_x, m_delta_y;
	bool m_mouse_first_moved = false;
};

struct Keys
{
	Keys();
	void Action(int key, int code, int action, int mode);
	bool m_key_code[1024];
};
