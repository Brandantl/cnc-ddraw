#include    "Game.h"
#include    "bgui.h"
#include    "engine3.h"
#include    "fenewmlt.h"

#if !D3D_VERSION
#include    "CNCDDraw.h"
#include    "imgui.h"
#include    "imgui_impl_win32.h"
#include    "imgui_impl_dx9.h"
#endif

enum class windows_available
{
    Main_Menu,
    Debug_Ouput,
    END
};

struct bgui_window windows[] =
{
    { "Main Menu",      false, nullptr,  bgui::draw_menu,           { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, ImGuiCond_Appearing,     true, true, true, true, true, false, true },
    { "Debug Ouput",    false, nullptr,  bgui::draw_debug_output,   { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, ImGuiCond_FirstUseEver,  true, true, true, true, true, true,  true },
    { nullptr, 0, 0, 0, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, 0, 0, 0, 0, 0, 0, 0 } // END
};

bool bgui::is_initd;
bgui_state bgui::state;
eastl::vector<struct bgui_window*> bgui::all_windows;

void bgui::add_window_array_to_vect(struct bgui_window * w)
{
    for (auto curr = w; curr->name; curr++)
        all_windows.push_back(curr);
}

void bgui::init()
{
#if !D3D_VERSION
    if (identify_poptb_renderer() != Renderers::DIRECTX9)
        return; // No OpenGL support

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(nullptr);

    if (identify_poptb_renderer() == Renderers::DIRECTX9)
        ImGui_ImplDX9_Init((*poptb_d3d_device));
    ImGui::StyleColorsDark();

    // Setup window vectors
    add_window_array_to_vect(&windows[0]);
    add_window_array_to_vect(engine_draw_getWindows());
    add_window_array_to_vect(fenewmlt_getWindows());
    add_window_array_to_vect(gsi.Script3.getWindows());
    is_initd = true;
#endif
}

void bgui::deinit()
{
#if !D3D_VERSION
    if (is_initd)
    {
        is_initd = false;
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
#endif
}

bool bgui::bgui::is_mouse_over_gui()
{
#if !D3D_VERSION
    if (is_initd)
    {
        return ImGui::GetIO().WantCaptureMouse;
    }
#endif
    return false;
}

bool bgui::bgui::is_keyboard_input_required()
{
#if !D3D_VERSION
    if (is_initd)
    {
        return ImGui::GetIO().WantCaptureKeyboard;
    }
#endif
    return false;
}

BOOL bgui::handle_mouse_button(TbInputKey key, BOOL bdown, SINT x, SINT y, void*)
{
#if !D3D_VERSION
    if (is_initd)
    {
        auto & io = ImGui::GetIO();
        io.MousePos.x = x;
        io.MousePos.y = y;

        switch (key)
        {
        case LB_KEY_MOUSE_LEFT:
            io.MouseDown[0] = bdown;
            break;
        case LB_KEY_MOUSE_MIDDLE:
            io.MouseDown[2] = bdown;
            break;
        case LB_KEY_MOUSE_RIGHT:
            io.MouseDown[1] = bdown;
            break;
        }
    }
#endif
    return 0;
}

BOOL bgui::handle_keyboard(TbInputKey key, BOOL bDown, void * lpParam)
{
#if !D3D_VERSION
    if (is_initd)
    {
        auto & io = ImGui::GetIO();

        io.KeyCtrl = CONTROL_ON;
        io.KeyShift = SHIFT_ON;
        io.KeyAlt = ALT_ON;

        io.KeysDown[key] = bDown;
    }
#endif
    return 0;
}

void bgui::handle_text_input(const char c)
{
#if !D3D_VERSION
    ImGui::GetIO().AddInputCharacter(c);
#endif
}

void bgui::reset_render_engine()
{
#if !D3D_VERSION
    if (is_initd)
    {
        // Reinit DX9
        if (identify_poptb_renderer() == Renderers::DIRECTX9)
        {
            ImGui_ImplDX9_Shutdown();
            ImGui_ImplDX9_Init((*poptb_d3d_device));
        }
    }
#endif
}

void bgui::open_main_menu()
{
    auto & window = windows[(int)windows_available::Main_Menu];
    window.visible = true;
}

bool bgui::can_i_draw_window(const bgui_window & window)
{
#if !D3D_VERSION
    if (!window.visible)
        return false;

    return can_i_add_button_to_menu(window);
#else
    return false;
#endif
}

bool bgui::can_i_add_button_to_menu(const bgui_window & window)
{
#if !D3D_VERSION
    if (!window.allowed_online && (gnsi.Flags & GNS_NETWORK))
        return false;

    if (!window.allowed_single_player && !prnet)
        return false;

    if (!window.allowed_inside_main_game && (gnsi.GameMode.Mode != GM_MAIN_GAME))
        return false;

    if (!window.allowed_outside_main_game && (gnsi.GameMode.Mode == GM_MAIN_GAME))
        return false;

#if !CM_DEVELOPMENT
    if (!window.allowed_online_in_release && (gnsi.Flags & GNS_NETWORK))
        return false;
#endif

    return true;
#else
    return false;
#endif
}
Poco::FastMutex _mu;

static bool first_frame_ready = false;

void bgui::render_gui()
{
#if !D3D_VERSION
    if (is_initd)
    {
        Poco::ScopedLock l(_mu);
        // Update mouse
        auto & io = ImGui::GetIO();
        const auto & mpos = Pop3Input::getMouseXY();
        io.MousePos.x = mpos->X;
        io.MousePos.y = mpos->Y; 

        // Draw pointer
        if (bgui::is_mouse_over_gui())
            io.MouseDrawCursor = true;
        else io.MouseDrawCursor = false;

        // Screen resolution X
        if (poptb_window_rect->right)
            io.DisplaySize.x = poptb_window_rect->right;
        else io.DisplaySize.x = gnsi.ScreenW;

        // Screen resolution Y
        if (poptb_window_rect->bottom)
            io.DisplaySize.y = poptb_window_rect->bottom;
        else io.DisplaySize.y = gnsi.ScreenH;

        // Adjust mouse with respect to resolution window
        auto diff_x = static_cast<float>(io.DisplaySize.x) / gnsi.ScreenW;
        auto diff_y = static_cast<float>(io.DisplaySize.y) / gnsi.ScreenH;
        io.MousePos.x *= diff_x;
        io.MousePos.y *= diff_y;

        // Start the Dear ImGui frame
        if (identify_poptb_renderer() == Renderers::DIRECTX9)
            ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // This enables and disabled textmode depending if the mouse is 
        // over a textbox or not.
        if (bgui::is_keyboard_input_required() && !state.main.is_keyboard_active)
        {
            Pop3Input::EnterTextMode();
            state.main.is_keyboard_active = true;
            gnsi.ControlMode = GCM_INPUT;
        }
        else if (!bgui::is_keyboard_input_required() && state.main.is_keyboard_active)
        {
            Pop3Input::ExitTextMode();
            state.main.is_keyboard_active = false;
            gnsi.ControlMode = GCM_NORMAL;
        }

        // Draw all guis
        draw_all_guis();
        ImGui::EndFrame();

        // Render UI
        ImGui::Render();

        first_frame_ready = true;
    }
#endif
}

void bgui::directx_render()
{
#if !D3D_VERSION
    Poco::ScopedLock l(_mu);
    if (is_initd && first_frame_ready)
    {
        if (identify_poptb_renderer() == Renderers::DIRECTX9)
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
#endif
}

void bgui::draw_all_guis()
{
#if !D3D_VERSION
    if (is_initd)
    {
        render_gui_windows();
        gsi.Script3.draw(state);
    }
#endif
}

void bgui::draw_menu(bgui_window & window, bgui_state & state)
{
#if !D3D_VERSION
    for (auto w : all_windows)
        if (w != &window) // Dont add yourself
            if (w->allowed_in_main_menu)
                if (can_i_add_button_to_menu(*w))
                    if (ImGui::Button(w->name))
                        w->visible = true;
#endif
}

void bgui::draw_debug_output(bgui_window & window, bgui_state & state)
{
#if !D3D_VERSION
    const auto & debugoutput = Pop3Debug::getOutputVect();
    ImGuiWindowFlags window_flags = ImGuiInputTextFlags_ReadOnly;
    ImGui::BeginChild("", ImVec2(-1, ImGui::GetWindowHeight() - 35.0f), false, window_flags);
    auto s = debugoutput.size();
    for (int i = 0; i < s; i++)
    {
        ImGui::TextWrapped("%02d: %s", i, debugoutput[i].c_str());
        ImGui::SetScrollHereY();
    }
    ImGui::EndChild();
#endif
}

void bgui::render_gui_windows()
{
    for (auto w : all_windows)
    {
        auto & window = *w;
        if (can_i_draw_window(window))
        {
            ASSERT(window.draw_function != nullptr);

            ImGui::SetNextWindowSize(window.inital_size, window.condition);

            if (window.position.x || window.position.y)
                ImGui::SetNextWindowPos(window.position);

            if (window.draw_predraw)
                window.draw_predraw(window, state);

            if (window.close_button)
                ImGui::Begin(window.name, &window.visible, window.flags);
            else ImGui::Begin(window.name, nullptr, window.flags);

            window.draw_function(window, state);
            ImGui::End();
        }
    }
}
