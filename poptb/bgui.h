#pragma once
#define ENABLE_NEW_GUI      1

// new GUI
extern void draw_gui();

// Input
extern BOOL key_press(TbInputKey key, BOOL bDown, void* lpParam);
extern BOOL mouse_change(TbInputKey key, BOOL bdown, SINT x, SINT y, void*);
extern void on_text_input(const char c);

// Actdata
extern bool is_mouse_over_gui();
extern bool is_keyboard_input_required();

// Settings
extern void invert_new_gui();
extern bool is_gui_active();
extern void switch_mode(bool mode);
extern void check_switch();
extern void gui_res_changed();
extern void gui_shutdown();