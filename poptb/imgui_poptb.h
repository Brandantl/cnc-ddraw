#pragma once
#include "imgui.h"
#include <type_traits>

template<typename T>
using remove_cvref = std::remove_reference_t<std::remove_cv_t<T>>;

namespace PopTB_GUI
{
    template<typename T>
    int constexpr getImguiType()
    {
        /////////////// Signed ///////////////
        if constexpr (std::is_same_v<remove_cvref<T>, signed char>)
        {
            return ImGuiDataType_S8;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, signed short>)
        {
            return ImGuiDataType_S16;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, signed int>)
        {
            return ImGuiDataType_S32;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, signed long>)
        {
            return ImGuiDataType_S32;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, signed __int64>)
        {
            return ImGuiDataType_S64;
        }

        /////////////// Unsigned ///////////////
        if constexpr (std::is_same_v<remove_cvref<T>, unsigned char>)
        {
            return ImGuiDataType_U8;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, unsigned short>)
        {
            return ImGuiDataType_U16;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, unsigned int>)
        {
            return ImGuiDataType_U32;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, unsigned long>)
        {
            return ImGuiDataType_U32;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, unsigned __int64>)
        {
            return ImGuiDataType_U64;
        }

        // Illrational data types
        if constexpr (std::is_same_v<remove_cvref<T>, float>)
        {
            return ImGuiDataType_Float;
        }

        if constexpr (std::is_same_v<remove_cvref<T>, double>)
        {
            return ImGuiDataType_Double;
        }

        return -1;
    }

    template<typename T>
    bool constexpr _Impl_InputScalerBox(const char * name, T & param, const int & step, const int & step_fast, const int & flags)
    {
        const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
        constexpr int param_data_type = getImguiType<T>();
        static_assert(param_data_type != -1, "Unsupported Data Type");
        return ImGui::InputScalar(name, param_data_type, static_cast<void*>(&param), (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), format, flags);
    }

    // Lambdas
    auto InputScalerBox = [](const char * name, auto & param, const auto step, const auto step_fast, const int flags) constexpr
    {
        _Impl_InputScalerBox<decltype(param)>(name, param, step, step_fast, flags);
    };
}