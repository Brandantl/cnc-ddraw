#include    "Game.h"
#include    "bgui.h"

#if !D3D_VERSION
#include    "CNCDDraw.h"
#include    "imgui.h"
#include    "imgui_impl_win32.h"
#include    "imgui_impl_dx9.h"
#endif

bool in_new_gui = false;
bool enable_draw = false;
static bool first_init = true;

void draw_gui_windows(bool & terminate);

void on_text_input(const char c)
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    auto & io = ImGui::GetIO();
    io.AddInputCharacter(c);
#endif
}

bool is_mouse_over_gui()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
    {
        const auto & io = ImGui::GetIO();
        return io.WantCaptureMouse;
    }
#endif
    return false;
}

bool is_keyboard_input_required()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
    {
        const auto & io = ImGui::GetIO();
        return io.WantCaptureKeyboard;
    }
#endif
    return false;
}

BOOL key_press(TbInputKey key, BOOL bDown, void* lpParam)
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
        return TRUE;

    auto & io = ImGui::GetIO();

    io.KeyCtrl = CONTROL_ON;
    io.KeyShift = SHIFT_ON;
    io.KeyAlt = ALT_ON;

    io.KeysDown[key] = bDown;
#endif
    return TRUE;
}

BOOL mouse_change(TbInputKey key, BOOL bdown, SINT x, SINT y, void*)
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
        return TRUE;

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
#endif
    return TRUE;
}

bool is_gui_active()
{
    return in_new_gui;
}

bool should_i_switch = false;
void invert_new_gui()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    should_i_switch = !should_i_switch;
#endif
}

void switch_mode(bool mode)
{
#if !D3D_VERSION && ENABLE_NEW_GUI

    if (!mode)
    {
        in_new_gui = false;
    }
    else
    {
        if (first_init)
        {
            // Setup ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            ImGui_ImplWin32_Init(nullptr);
            ImGui_ImplDX9_Init((*poptb_d3d_device));
            ImGui::StyleColorsDark();
            first_init = false;
        }

        in_new_gui = true;
    }
#endif
}

void gui_res_changed()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
    {
        // Reinit DX9
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplDX9_Init((*poptb_d3d_device));
    }
#endif
}

void gui_shutdown()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
#endif
}

// GUI Driver code
void draw_gui()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (!first_init)
    {
        // States
        static bool terminate = false;
        static bool is_keyboard_active = false;

        // Update mouse
        auto & io = ImGui::GetIO();
        const auto & mpos = Pop3Input::getMouseXY();
        io.MousePos.x = mpos->X;
        io.MousePos.y = mpos->Y;

        // Draw pointer
        if (is_mouse_over_gui())
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

        // Adjust mouse due to resolution window
        auto diff_x = static_cast<float>(io.DisplaySize.x) / gnsi.ScreenW;
        auto diff_y = static_cast<float>(io.DisplaySize.y) / gnsi.ScreenH;
        io.MousePos.x *= diff_x;
        io.MousePos.y *= diff_y;

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // This enables and disabled textmode depending if the mouse is 
        // over a textbox or not.
        if (is_keyboard_input_required() && !is_keyboard_active)
        {
            Pop3Input::EnterTextMode();
            is_keyboard_active = true;
            gnsi.ControlMode = GCM_INPUT;
        }
        else if (!is_keyboard_input_required() && is_keyboard_active)
        {
            Pop3Input::ExitTextMode();
            is_keyboard_active = false;
            gnsi.ControlMode = GCM_NORMAL;
        }

        // Draw all guis
        draw_gui_windows(terminate);

        // Rendering
        ImGui::EndFrame();

        // Render UI
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        // We should wait until we're done rendering
        // to decide to exit the gui.
        if (terminate)
        {
            switch_mode(false);
            terminate = false;
            return;
        }
    }
#endif
}

void draw_gui_windows(bool & terminate)
{
    // Windows
    static bool window_debug_output = false;
    static bool window_script_engine = false;
    static bool window_script_windows = false;

#if CM_USE_SCRIPT4
    // Call all LUA OnImGuiFrame windows
    if (window_script_windows)
        gsi.Script3.call_back(ecallback::OnImGuiFrame, {});
#endif

    if (in_new_gui)
    {
        // Game Menu
        ImGui::Begin("Menu");
        if (ImGui::Button("Output"))
            window_debug_output = !window_debug_output;

#if CM_USE_SCRIPT4
        if (!(gnsi.Flags & GNS_NETWORK) && ImGui::Button("Script Engine"))
            window_script_engine = !window_script_engine;
        if (ImGui::Button("Script Windows"))
            window_script_windows = !window_script_windows;
#endif

        if (ImGui::Button("Exit to game"))
        {
            // Finish render then terminate.
            terminate = true;
        }
        ImGui::End();
    }

    if (window_debug_output)
    {
        ImGui::Begin("Debug Output");

        auto & debugoutput = Pop3Debug::getOutputVect();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiInputTextFlags_ReadOnly;
        ImGui::BeginChild("", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 35.0f), false, window_flags);
        auto s = debugoutput.size();
        for (int i = 0; i < s; i++)
        {
            ImGui::TextWrapped("%02d: %s", i, debugoutput[i].c_str());
            ImGui::SetScrollHereY();
        }
        ImGui::EndChild();
        ImGui::End();
    }

#if CM_USE_SCRIPT4 
    // Draw all LUA OnImGuiFrame windows
    if (window_script_engine)
        gsi.Script3.draw();
#endif
}

void check_switch()
{
#if !D3D_VERSION && ENABLE_NEW_GUI
    if (should_i_switch)
    {
        if (!is_gui_active())
            switch_mode(true);
        should_i_switch = false;
    }
#endif
}