#pragma once
#include <EASTL/functional.h>

typedef eastl::function<void(struct bgui_window & window, struct bgui_state & state)> bgui_callback;

#define MAX_FILEPATH_LENGTH 200

struct bgui_state_script4
{
    bgui_state_script4() :      current_script_sel(nullptr),
                                script4_lua_windows_visible(true),
                                add_script_get_filepath(false),
                                add_script_get_filepath_no_load(false),
                                filename(new char[POP3NETWORK_MAX_PLAYER_NAME_LENGTH] {}),
                                filepath(new char[MAX_FILEPATH_LENGTH] {}) ,
                                script_buff_size(0),
                                script_memory_size(0),
                                script_memory(new char[script_memory_size] {}) {}

    const char*                 current_script_sel;
    bool                        script4_lua_windows_visible;
    bool                        add_script_get_filepath;
    bool                        add_script_get_filepath_no_load;
    eastl::unique_ptr<char[]>   filename;
    eastl::unique_ptr<char[]>   filepath;
    size_t                      script_buff_size;
    size_t                      script_memory_size;
    eastl::unique_ptr<char[]>   script_memory;
};

struct bgui_state_main
{
    bgui_state_main() : is_keyboard_active(false) {}

    bool is_keyboard_active;
};

struct bgui_state
{
    bgui_state_main main;
    bgui_state_script4 script;
};

struct bgui_start_position
{
    float x;
    float y;
};

struct bgui_window
{
    char*               name;
    bool                visible; 
    bgui_callback       draw_predraw;
    bgui_callback       draw_function;
    ImVec2              position;
    ImVec2              inital_size;
    ImGuiWindowFlags    flags;
    ImGuiCond           condition;
    bool                allowed_online;
    bool                allowed_single_player;
    bool                allowed_outside_main_game;
    bool                allowed_inside_main_game;
    bool                allowed_online_in_release;
    bool                allowed_in_main_menu;
    bool                close_button;
};

extern Poco::FastMutex _render_mu;

class bgui
{
public:
    static void init();
    static void deinit();

    static void render_gui();

    static bool is_mouse_over_gui();
    static bool is_keyboard_input_required();

    static BOOL handle_mouse_button(TbInputKey key, BOOL bdown, SINT x, SINT y, void*);
    static BOOL handle_keyboard(TbInputKey key, BOOL bDown, void* lpParam);
    static void handle_text_input(const char c);
    static void reset_render_engine();
    static void open_main_menu();

    static bool can_i_draw_window(const struct bgui_window & window);
    static bool can_i_add_button_to_menu(const struct bgui_window & window);

    static void draw_menu(struct bgui_window & window, struct bgui_state & state);
    static void draw_debug_output(struct bgui_window & window, struct bgui_state & state);
    static void draw_game_mode(struct bgui_window & window, struct bgui_state & state);

    static void render_gui_windows();

    static void directx_render();
private:
    static void draw_all_guis();
    static void add_window_array_to_vect(struct bgui_window * w);

    static bool is_initd;
    static bgui_state state;
    static eastl::vector<struct bgui_window*> all_windows;
};