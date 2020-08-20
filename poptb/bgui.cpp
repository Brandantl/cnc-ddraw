#include    "Game.h"
#include    "bgui.h"
#include    "engine3.h"
#include    "fenewmlt.h"
#include    "Objedit.h"
#include    "spredit.h"
#include    "io.h"
#include	"Things.h"
#include    "Crash.h"
#include    "Version.h"
#include "MapEdit.h"
#include <thread>

#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/UnicodeConverter.h"

#if !D3D_VERSION
#include    "CNCDDraw.h"
#include    "imgui.h"
#include    "imgui_impl_win32.h"
#include    "imgui_impl_dx9.h"
#include	"Chat.h"
#endif

enum class windows_available
{
    Main_Menu,
    Debug_Ouput,
    Game_Mode,
    Report_Bug,
    END
};

struct bgui_window windows[] =
{
    { "Main Menu",      false, nullptr,  bgui::draw_menu, nullptr,           { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, ImGuiCond_Appearing,     true, true, true, true, true, false, true },
    { "Debug Ouput",    false, nullptr,  bgui::draw_debug_output, nullptr,   { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, ImGuiCond_FirstUseEver,  true, true, true, true, true, true,  true },
    { "Game Mode",    false, nullptr,    bgui::draw_game_mode, nullptr,   { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, ImGuiCond_FirstUseEver,  true, true, true, true, false, true,  true },
    { "Report Bug",    false, nullptr,    bgui::draw_report_window, nullptr, { gnsi.ScreenW/2.0f, gnsi.ScreenH/2.0f }, { 400.0f, 300.0f }, 0, ImGuiCond_Once,  true, true, true, true, true, true,  true },
    { "Key Debug",    false, nullptr,    bgui::draw_input_test_window, nullptr, { 0, 0 }, { 400.0f, 300.0f }, 0, ImGuiCond_Once,  true, true, true, true, true, true,  true },
    { "File Transfer",    false, nullptr,    bgui::draw_filetransfer_test_window, nullptr, { 0, 0 }, { 400.0f, 300.0f }, 0, ImGuiCond_Once,  true, true, true, true, true, true,  true },

    { nullptr, 0, 0, 0, 0, { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0, 0, 0, 0, 0, 0, 0, 0 } // END
};

bool bgui::is_initd;
bgui_state bgui::state;
eastl::vector<struct bgui_window*> bgui::all_windows;
static bool first_frame_ready = false;

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
#if CM_USE_SCRIPT4
    add_window_array_to_vect(gsi.Script3.getWindows());
#endif
    add_window_array_to_vect(getObjedit_windows());
    add_window_array_to_vect(getSpredit_windows());
    add_window_array_to_vect(getMapEdit_windows());
    add_window_array_to_vect(gci.Mappack.getWindows());
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
        all_windows.clear();
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
            first_frame_ready = false;
        }
    }
#endif
}

void bgui::open_main_menu()
{
    auto & window = windows[(int)windows_available::Main_Menu];
    window.visible = true;
}

void bgui::open_report_window()
{
    auto & window = windows[(int)windows_available::Report_Bug];
    window.visible = true;
}

bool bgui::is_report_window_open()
{
    auto & window = windows[(int)windows_available::Report_Bug];
    return window.visible;
}

char * bgui::get_report_window_text_ptr()
{
    return &state.main.report_msg[0];
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
Poco::FastMutex _render_mu;

void bgui::render_gui()
{
#if !D3D_VERSION
    if (is_initd)
    {
        if (!_render_mu.tryLock(84))
            return;

        sprite_texture_manager::update();

        // Update mouse
        auto & io = ImGui::GetIO();
        const auto & mpos = Pop3Input::getMouseXY();
        io.MousePos.x = mpos->X;
        io.MousePos.y = mpos->Y;

        // Draw pointer
        if (bgui::is_mouse_over_gui())
            io.MouseDrawCursor = true;
        else io.MouseDrawCursor = false;

        // Boarderless mode
        if ((*poptb_ddraw_ptr)->windowed && (*poptb_ddraw_ptr)->border)
        {
            io.DisplaySize.x = (*poptb_ddraw_ptr)->render.width;
            io.DisplaySize.y = (*poptb_ddraw_ptr)->render.height;
        }
        // Window mode
        else if ((*poptb_ddraw_ptr)->windowed)
        {
            io.DisplaySize.x = poptb_window_rect->right;
            io.DisplaySize.y = poptb_window_rect->bottom;
        } 
        // Fulscreen
        else 
        {
             io.DisplaySize.x = gnsi.ScreenW;
             io.DisplaySize.y = gnsi.ScreenH;
        }

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

        g_Chat.m_IsHovered = false;
        if ((g_Chat.m_bWindowOpen) && !(gnsi.Flags3 & GNS3_INGAME_OPTIONS) && gnsi.GameMode.Mode == GM_MAIN_GAME)
        {
            g_Chat.Draw("Chat", &g_Chat.m_bWindowOpen);
        }

        ImGui::EndFrame();

        // Render UI
        ImGui::Render();

        first_frame_ready = true;
        _render_mu.unlock();
    }
#endif
}

void bgui::directx_render()
{
#if !D3D_VERSION
    if (is_initd && first_frame_ready)
    {
        if (identify_poptb_renderer() == Renderers::DIRECTX9)
        {
            auto data = ImGui::GetDrawData();
            if (data)
                ImGui_ImplDX9_RenderDrawData(data);
        }
    }
#endif
}

void bgui::draw_all_guis()
{
#if !D3D_VERSION
    if (is_initd)
    {
        render_gui_windows();
        gnsi.TDebug.draw_all();
#if CM_USE_SCRIPT4
        gsi.Script3.draw(state);
#endif
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

enum class GameModesAvail
{
    Invalid = 0,
    MainMenu = GM_MAIN_MENU,
    MainGame = GM_MAIN_GAME,
    MapEditor = GM_EDITOR,
    ObjectEditor = GM_OBJECT_EDITOR,
    GaumutGen = GM_GAMUT_GENERATION,
    NetworkI2 = GM_NETWORK_INTERFACE_2,
    FrontEnd = GM_FRONTEND,
    LanguageTest = GM_LANGUAGE_TEST,
    FeNet = GM_FE_NET,
    PlanetarySelect = GM_PLANETARY_LEVEL_SELECT,
    KeyDeifne = GM_KEY_DEFINE_TEST,
    Credits = GM_CREDITS,
    SprtieEditor = GM_SPRITE_EDITOR
};

const char * gameModes[] =
{
    "Invalid",
    "Main Menu",
    "Main Game",
    "Map Editor",
    "Object Editor",
    "Gamut Generation",
    "Network Interface 2",
    "Front End",
    "Language Test",
    "FE Net",
    "Planetary Level Select",
    "Key Define Test",
    "Credits",
    "Sprite Editor"
};

void bgui::draw_game_mode(struct bgui_window & window, struct bgui_state & state)
{
#if !D3D_VERSION
    static GameModesAvail item_current;
    item_current = static_cast<GameModesAvail>(gnsi.GameMode.Mode);
    if (ImGui::Combo("Game Mode", reinterpret_cast<int*>(&item_current), gameModes, IM_ARRAYSIZE(gameModes)))
        change_game_mode(static_cast<int>(item_current));
#endif
}

enum class send_status
{
    IDLE,
    SENDING,
    COMPLETE,
    FAILED
};

void bgui::draw_report_window(bgui_window & window, bgui_state & state)
{
    static send_status status = send_status::IDLE;
    static auto send_report = [&]()
    {
        status = send_status::SENDING;
        try
        {
            auto crash_dump_zip = getAbsoultePath(CRASH_FILENAME, "wb");
            std::ofstream out = std::ofstream(crash_dump_zip, std::ios::binary | std::ios::trunc);
            if (out.is_open())
            {
                Poco::Zip::Compress c(out, true);
                auto game_gsi_path = getAbsoultePath("Save//gsi-dump.dat", "wb");
                Poco::Path crash_gsi_file(game_gsi_path);
                c.addFile(crash_gsi_file, crash_gsi_file.getFileName());
                c.close();
                out.close();

                Poco::URI uri(REPORTING_SERVER);
                Poco::Net::HTTPClientSession client_session(uri.getHost(), uri.getPort());
                client_session.setKeepAlive(true);

                // Prepare and send request
                std::string path(uri.getPathAndQuery());
                //Poco::Net::HTTPBasicCredentials cred(SERVER_USERNAME, SERVER_PASSWORD);
                Poco::Net::HTTPRequest req(Poco::Net::HTTPRequest::HTTP_POST, path, Poco::Net::HTTPMessage::HTTP_1_1);
                Poco::Net::HTTPResponse response;
                req.setKeepAlive(true); // notice setKeepAlive is also called on session (above)

                Poco::Net::HTMLForm form;
                form.setEncoding(Poco::Net::HTMLForm::ENCODING_MULTIPART);
                form.set("subject", "Bug Report! - " + Poco::NumberFormatter::format(BUILD_NUMBER) + " - " + Poco::NumberFormatter::format(gsi.Seed));
                form.set("body", state.main.report_msg);
                form.set("build-number", Poco::NumberFormatter::format(BUILD_NUMBER));
                form.set("seed", Poco::NumberFormatter::format(gsi.Seed));
                form.addPart("zip", new Poco::Net::FilePartSource(crash_dump_zip));
                form.set("bug-report", "true");
                form.prepareSubmit(req);
                std::ostream& send = client_session.sendRequest(req);
                form.write(send);

                // get response  
                client_session.receiveResponse(response);

                auto ret = response.getStatus();
                Poco::File(crash_gsi_file).remove();

                if (ret != Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK)
                {
                    status = send_status::FAILED;
                } else status = send_status::COMPLETE;
            }
        }
        catch (...)
        {
            status = send_status::FAILED;
        }
    };

    switch (status)
    {
    case send_status::IDLE:
        if (status == send_status::IDLE)
        {
            ImGui::Text("Explain the issue in detail (%d characters max)", MAX_NUM_REPORT_CHARS);
            ImGui::InputTextMultiline("", &state.main.report_msg[0], MAX_NUM_REPORT_CHARS, { -1, ImGui::GetWindowHeight() - 80.0f }, ImGuiInputTextFlags_None);
            if (ImGui::Button("Send Report"))
            {
                gci.SS.saveToDisk();
                std::thread send_thread(send_report);
                send_thread.detach();
            }
            ImGui::SameLine();
            ImGui::TextUnformatted("This may cause the game to briefly freeze.");
        }
        break;
    case send_status::SENDING:
        ImGui::TextUnformatted("Sending report...");
        break;
    case send_status::COMPLETE:
        window.visible = false;
        memset(&state.main.report_msg, 0, MAX_NUM_REPORT_CHARS);
        status = send_status::IDLE;
        break;
    case send_status::FAILED:
        status = send_status::IDLE;
        break;
    }
}

void bgui::draw_input_test_window(bgui_window & window, bgui_state & state)
{
    const auto & key_state = Pop3Input::getState();

    for (int key = 0; key < MAX_KEYS; key++)
    {
        if (key_state[key])
        {
            ImGui::Text("Key %d is down", key);
        }
    }
}

const char * status_strs[] =
{
        "Ready",
        "Host_Waiting_On_Client_Ack",
        "Host_Waiting_On_Client_To_Finish_Resend_Requests",
        "Host_Transfering_Data",
        "Host_Waiting_On_Client_Resend_Complete",
        "Host_Waiting_On_Client_To_Finish_Transfer",
        "Client_Sending_Resend_Requests",
        "Client_Waiting_On_Host_To_Start_Sending_File_Parts",
        "Client_Waiting_On_Host_Parts_To_Finish",
        "Transfer_Complete"
};

void bgui::draw_filetransfer_test_window(bgui_window & window, bgui_state & state)
{
    if (prnet)
    {
        unsigned int idx = static_cast<unsigned int>(prnet->getFileTransferStatus());
        ImGui::Text("Status: %s", status_strs[idx]);
        if (prnet->am_i_host())
        {
            if (ImGui::Button("Transfer"))
            {
                // Load a real file.
                Poco::Path path = Poco::Path::home();
                path.pushDirectory(GAME_DIRECTORY_MAIN);
                path.pushDirectory(GAME_DIRECTORY_MAPPACKS);

                auto file_path = path.toString() + "\\SampleMod.zip";

                // Load file
                std::ifstream f(file_path, std::ios::binary | std::ios::ate);
                auto fsize = f.tellg();
                f.seekg(std::ios::beg);

                char* data = new char[fsize];
                f.read(data, fsize);
                f.close();

                // Lets do a file transfer.
                prnet->transfer_file(NET_ALLPLAYERS, "SampleMod.zip", data, fsize);

                delete[] data;
            }
        }
        else
        {
            auto total_size = prnet->getFileTransferTotalBytes();
            auto recieved_bytes = prnet->getFileTransferRecievedBytes();
            auto percent = prnet->getFileTransferPercent();
            auto filename = prnet->getFileTransferName();

            ImGui::Text("File Name: %s", filename.c_str());
            ImGui::Text("Transfer: %d / %d bytes (%d%%)", recieved_bytes, total_size, percent);
        }
        ImGui::Text("Wait Time %d", prnet->getFileTransferSleepTimer());
    }
    else ImGui::Text("No Network Subsystem Detected!");
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

            if (!window.visible && window.close_function)
                window.close_function(window, state);
        }
    }
}
