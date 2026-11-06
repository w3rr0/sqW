#include "gui/Utils.hpp"
#include <regex>
#include "imgui.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

static Utils::Theme s_currentTheme = Utils::Theme::ModernDark;
static Utils::ThemeColors s_currentThemeColors;

Utils::Theme Utils::getCurrentTheme() {
    return s_currentTheme;
}

const Utils::ThemeColors& Utils::getCurrentThemeColors() {
    return s_currentThemeColors;
}

void Utils::applyModernTheme() {
    applyTheme(Theme::ModernDark);
}

void Utils::applyTheme(Theme theme) {
    s_currentTheme = theme;
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Smooth rounded corners and modern borders
    style.WindowRounding    = 8.0f;
    style.ChildRounding     = 6.0f;
    style.FrameRounding     = 5.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding      = 5.0f;
    style.TabRounding       = 6.0f;

    style.WindowBorderSize  = 1.0f;
    style.ChildBorderSize   = 1.0f;
    style.PopupBorderSize   = 1.0f;
    style.FrameBorderSize   = 1.0f;
    style.TabBorderSize     = 0.0f;

    style.WindowPadding     = ImVec2(12.0f, 12.0f);
    style.FramePadding      = ImVec2(10.0f, 6.0f);
    style.ItemSpacing       = ImVec2(10.0f, 8.0f);
    style.ItemInnerSpacing  = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize     = 14.0f;

    switch (theme) {
        case Theme::ModernDark: {
            // Theme 0: Modern Dark (Tokyo Night / Catppuccin inspired)
            colors[ImGuiCol_Text]                  = ImVec4(0.93f, 0.95f, 0.98f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.52f, 0.56f, 0.65f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.10f, 0.14f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(0.12f, 0.14f, 0.19f, 1.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.14f, 0.19f, 0.98f);
            colors[ImGuiCol_Border]                = ImVec4(0.20f, 0.23f, 0.32f, 0.80f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.15f, 0.17f, 0.24f, 1.00f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.22f, 0.30f, 1.00f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.23f, 0.27f, 0.38f, 1.00f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.09f, 0.10f, 0.14f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.09f, 0.10f, 0.14f, 0.75f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.12f, 0.14f, 0.19f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.09f, 0.10f, 0.14f, 0.50f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.24f, 0.27f, 0.38f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.32f, 0.36f, 0.48f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.38f, 0.42f, 0.56f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);
            colors[ImGuiCol_Button]                = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.24f, 0.28f, 0.38f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.15f, 0.17f, 0.24f, 1.00f);
            colors[ImGuiCol_Header]                = ImVec4(0.15f, 0.17f, 0.24f, 0.85f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(0.20f, 0.24f, 0.34f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.25f, 0.30f, 0.42f, 1.00f);
            colors[ImGuiCol_Separator]             = ImVec4(0.20f, 0.23f, 0.32f, 0.80f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.32f, 0.36f, 0.50f, 1.00f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(0.40f, 0.46f, 0.62f, 1.00f);
            colors[ImGuiCol_Tab]                   = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(0.24f, 0.28f, 0.40f, 1.00f);
            colors[ImGuiCol_TabActive]             = ImVec4(0.19f, 0.22f, 0.32f, 1.00f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(0.12f, 0.14f, 0.19f, 1.00f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.15f, 0.17f, 0.24f, 1.00f);
            colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.15f, 0.17f, 0.24f, 1.00f);
            colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.22f, 0.25f, 0.35f, 1.00f);
            colors[ImGuiCol_TableBorderLight]      = ImVec4(0.17f, 0.19f, 0.27f, 1.00f);
            colors[ImGuiCol_TableRowBg]            = ImVec4(0.12f, 0.14f, 0.19f, 1.00f);
            colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);

            s_currentThemeColors = {
                ImVec4(0.35f, 0.60f, 1.00f, 1.00f), // brand
                ImVec4(0.24f, 0.44f, 0.88f, 1.00f), // accent
                ImVec4(0.32f, 0.52f, 0.98f, 1.00f), // accentHovered
                ImVec4(0.18f, 0.36f, 0.78f, 1.00f), // accentActive
                ImVec4(1.00f, 1.00f, 1.00f, 1.00f), // executeBtnText
                ImVec4(0.52f, 0.56f, 0.65f, 1.00f), // subtext
                ImVec4(0.30f, 0.85f, 0.52f, 1.00f), // statusOk
                ImVec4(0.95f, 0.38f, 0.38f, 1.00f), // statusErr
                ImVec4(0.40f, 0.72f, 1.00f, 1.00f), // kwColor
                ImVec4(0.92f, 0.50f, 0.80f, 1.00f), // typeColor
                ImVec4(1.00f, 0.82f, 0.35f, 1.00f), // numColor
                ImVec4(0.42f, 0.88f, 0.55f, 1.00f)  // strColor
            };
            break;
        }

        case Theme::ClassicLight: {
            // Theme 1: Minimalist Editorial Light (Inspired by Teapot Cashier Tablet reference)
            colors[ImGuiCol_Text]                  = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.42f, 0.45f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.96f, 0.96f, 0.97f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
            colors[ImGuiCol_Border]                = ImVec4(0.85f, 0.86f, 0.89f, 1.00f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.96f, 0.96f, 0.97f, 1.00f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.92f, 0.93f, 0.95f, 1.00f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.88f, 0.89f, 0.92f, 1.00f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.94f, 0.94f, 0.95f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.90f, 0.91f, 0.93f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.94f, 0.94f, 0.95f, 0.75f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.96f, 0.96f, 0.97f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.96f, 0.96f, 0.97f, 0.60f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.78f, 0.80f, 0.84f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.65f, 0.68f, 0.74f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.52f, 0.55f, 0.62f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.24f, 0.28f, 0.34f, 1.00f);

            // Sleek grayish buttons for secondary actions
            colors[ImGuiCol_Button]                = ImVec4(0.92f, 0.93f, 0.95f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.84f, 0.86f, 0.89f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.76f, 0.78f, 0.82f, 1.00f);

            // Neutral soft gray for list headers and tree nodes (no bright blue!)
            colors[ImGuiCol_Header]                = ImVec4(0.94f, 0.95f, 0.97f, 0.90f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.80f, 0.83f, 0.88f, 1.00f);

            colors[ImGuiCol_Separator]             = ImVec4(0.85f, 0.86f, 0.89f, 1.00f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.72f, 0.75f, 0.80f, 1.00f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(0.58f, 0.62f, 0.70f, 1.00f);
            colors[ImGuiCol_Tab]                   = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(0.88f, 0.90f, 0.93f, 1.00f);
            colors[ImGuiCol_TabActive]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.97f, 0.97f, 0.98f, 1.00f);
            colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.93f, 0.94f, 0.96f, 1.00f);
            colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.82f, 0.84f, 0.88f, 1.00f);
            colors[ImGuiCol_TableBorderLight]      = ImVec4(0.90f, 0.91f, 0.94f, 1.00f);
            colors[ImGuiCol_TableRowBg]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.97f, 0.97f, 0.98f, 1.00f);

            s_currentThemeColors = {
                ImVec4(0.10f, 0.12f, 0.16f, 1.00f), // brand
                ImVec4(0.12f, 0.14f, 0.18f, 1.00f), // accent (deep matte charcoal primary button)
                ImVec4(0.22f, 0.25f, 0.30f, 1.00f), // accentHovered
                ImVec4(0.06f, 0.08f, 0.10f, 1.00f), // accentActive
                ImVec4(1.00f, 1.00f, 1.00f, 1.00f), // executeBtnText (crisp white text on dark button)
                ImVec4(0.40f, 0.43f, 0.48f, 1.00f), // subtext
                ImVec4(0.08f, 0.55f, 0.24f, 1.00f), // statusOk
                ImVec4(0.82f, 0.15f, 0.15f, 1.00f), // statusErr
                ImVec4(0.12f, 0.38f, 0.85f, 1.00f), // kwColor
                ImVec4(0.55f, 0.15f, 0.65f, 1.00f), // typeColor
                ImVec4(0.75f, 0.35f, 0.05f, 1.00f), // numColor
                ImVec4(0.08f, 0.52f, 0.22f, 1.00f)  // strColor
            };
            break;
        }

        case Theme::WarmAmber: {
            // Theme 2: Fleet Health / Obsidian Warm Amber (Inspired by Insoftex reference)
            colors[ImGuiCol_Text]                  = ImVec4(0.94f, 0.94f, 0.96f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.54f, 0.53f, 0.58f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(0.12f, 0.12f, 0.15f, 0.98f);
            colors[ImGuiCol_Border]                = ImVec4(0.22f, 0.22f, 0.26f, 0.80f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.19f, 0.19f, 0.24f, 1.00f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.24f, 0.24f, 0.30f, 1.00f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.08f, 0.08f, 0.10f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.08f, 0.08f, 0.10f, 0.75f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.08f, 0.08f, 0.10f, 0.50f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.24f, 0.23f, 0.27f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.34f, 0.32f, 0.38f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.44f, 0.40f, 0.48f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(0.85f, 0.40f, 0.10f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = ImVec4(0.85f, 0.40f, 0.10f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.95f, 0.50f, 0.15f, 1.00f);

            // Sleek dark charcoal buttons (no neon orange!)
            colors[ImGuiCol_Button]                = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.24f, 0.23f, 0.28f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.12f, 0.12f, 0.16f, 1.00f);

            // Subtle dark card header (no orange block!)
            colors[ImGuiCol_Header]                = ImVec4(0.15f, 0.15f, 0.19f, 0.85f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(0.22f, 0.21f, 0.26f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.28f, 0.26f, 0.32f, 1.00f);

            colors[ImGuiCol_Separator]             = ImVec4(0.22f, 0.22f, 0.26f, 0.80f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.35f, 0.34f, 0.40f, 1.00f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(0.48f, 0.46f, 0.54f, 1.00f);
            colors[ImGuiCol_Tab]                   = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(0.22f, 0.20f, 0.25f, 1.00f);
            colors[ImGuiCol_TabActive]             = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
            colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.16f, 0.16f, 0.20f, 1.00f);
            colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.25f, 0.24f, 0.28f, 1.00f);
            colors[ImGuiCol_TableBorderLight]      = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
            colors[ImGuiCol_TableRowBg]            = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
            colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);

            s_currentThemeColors = {
                ImVec4(0.92f, 0.45f, 0.12f, 1.00f), // brand
                ImVec4(0.78f, 0.34f, 0.08f, 1.00f), // accent (grounded rich burnt amber, NOT neon!)
                ImVec4(0.88f, 0.40f, 0.10f, 1.00f), // accentHovered
                ImVec4(0.65f, 0.28f, 0.06f, 1.00f), // accentActive
                ImVec4(1.00f, 1.00f, 1.00f, 1.00f), // executeBtnText
                ImVec4(0.55f, 0.54f, 0.58f, 1.00f), // subtext
                ImVec4(0.38f, 0.82f, 0.35f, 1.00f), // statusOk
                ImVec4(0.95f, 0.35f, 0.30f, 1.00f), // statusErr
                ImVec4(0.95f, 0.52f, 0.18f, 1.00f), // kwColor
                ImVec4(0.92f, 0.72f, 0.28f, 1.00f), // typeColor
                ImVec4(0.92f, 0.40f, 0.42f, 1.00f), // numColor
                ImVec4(0.40f, 0.80f, 0.55f, 1.00f)  // strColor
            };
            break;
        }

        case Theme::MidnightPurple: {
            // Theme 3: Cosmic Space Navy & Ethereal Indigo (Inspired by Huly reference)
            colors[ImGuiCol_Text]                  = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.48f, 0.52f, 0.65f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.04f, 0.05f, 0.08f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(0.07f, 0.08f, 0.14f, 1.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.10f, 0.16f, 0.98f);
            colors[ImGuiCol_Border]                = ImVec4(0.18f, 0.20f, 0.32f, 0.80f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.09f, 0.11f, 0.18f, 1.00f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.14f, 0.17f, 0.26f, 1.00f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.18f, 0.22f, 0.34f, 1.00f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.04f, 0.05f, 0.08f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.09f, 0.11f, 0.18f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.04f, 0.05f, 0.08f, 0.75f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.07f, 0.08f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.04f, 0.05f, 0.08f, 0.50f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.18f, 0.22f, 0.34f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.26f, 0.30f, 0.46f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.34f, 0.38f, 0.58f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(0.48f, 0.42f, 0.95f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = ImVec4(0.48f, 0.42f, 0.95f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.58f, 0.52f, 1.00f, 1.00f);

            // Sleek cosmic slate buttons (no neon purple!)
            colors[ImGuiCol_Button]                = ImVec4(0.12f, 0.14f, 0.22f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.18f, 0.22f, 0.34f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.09f, 0.11f, 0.18f, 1.00f);

            // Subtle cosmic header (no purple block!)
            colors[ImGuiCol_Header]                = ImVec4(0.10f, 0.12f, 0.20f, 0.85f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(0.16f, 0.19f, 0.30f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.20f, 0.24f, 0.38f, 1.00f);

            colors[ImGuiCol_Separator]             = ImVec4(0.18f, 0.20f, 0.32f, 0.80f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.30f, 0.34f, 0.50f, 1.00f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(0.42f, 0.46f, 0.65f, 1.00f);
            colors[ImGuiCol_Tab]                   = ImVec4(0.09f, 0.11f, 0.18f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(0.16f, 0.19f, 0.30f, 1.00f);
            colors[ImGuiCol_TabActive]             = ImVec4(0.14f, 0.17f, 0.26f, 1.00f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(0.07f, 0.08f, 0.14f, 1.00f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.11f, 0.13f, 0.20f, 1.00f);
            colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.10f, 0.12f, 0.20f, 1.00f);
            colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.20f, 0.23f, 0.35f, 1.00f);
            colors[ImGuiCol_TableBorderLight]      = ImVec4(0.14f, 0.16f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg]            = ImVec4(0.07f, 0.08f, 0.14f, 1.00f);
            colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.09f, 0.10f, 0.17f, 1.00f);

            s_currentThemeColors = {
                ImVec4(0.50f, 0.44f, 0.95f, 1.00f), // brand (Ethereal Indigo logo)
                ImVec4(0.32f, 0.28f, 0.82f, 1.00f), // accent (Ethereal Electric Indigo - exact Huly button!)
                ImVec4(0.40f, 0.36f, 0.92f, 1.00f), // accentHovered
                ImVec4(0.25f, 0.22f, 0.70f, 1.00f), // accentActive
                ImVec4(1.00f, 1.00f, 1.00f, 1.00f), // executeBtnText
                ImVec4(0.48f, 0.52f, 0.65f, 1.00f), // subtext
                ImVec4(0.22f, 0.85f, 0.75f, 1.00f), // statusOk (Neon Cyan / Mint)
                ImVec4(0.95f, 0.32f, 0.50f, 1.00f), // statusErr
                ImVec4(0.68f, 0.55f, 1.00f, 1.00f), // kwColor (Lavender Indigo)
                ImVec4(0.78f, 0.52f, 1.00f, 1.00f), // typeColor (Violet)
                ImVec4(0.98f, 0.85f, 0.38f, 1.00f), // numColor (Soft Gold)
                ImVec4(0.25f, 0.88f, 0.82f, 1.00f)  // strColor (Aqua Cyan)
            };
            break;
        }
    }
}

// method that renders SQL text with syntax highlighting using Regex
void Utils::renderColoredSQL(const std::string& text, bool isGui) {
    // sql keywords
    static std::regex regKeywords("\\b(SELECT|FROM|WHERE|CREATE|TABLE|INSERT|INTO|VALUES|UPDATE|DELETE|SET|DROP|"
                                  "COLUMN|ALTER|ADD|LIMIT|OFFSET|DISTINCT|IS|NOT|AND|OR|ORDER|BY|ASC|DESC|BETWEEN|"
                                  "LIKE|IN|COUNT|MIN|MAX|SUM|SHOW|TABLES)\\b", std::regex_constants::icase);
    // sql data types
    static std::regex regTypes("\\b(INT|INTEGER|STRING|TEXT|VARCHAR|BOOL|BOOLEAN|DOUBLE|FLOAT|NULL)\\b",
                                    std::regex_constants::icase);
    // numbers
    static std::regex regNumbers("\\b\\d+(\\.\\d+)?\\b");
    // strings
    static std::regex regStrings("('\\s*[^']*\\s*'|\\\"\\s*[^\\\"]*\\s*\\\")");
    // connecting words into a sentence
    static std::regex wordRegex("([\\w*]+|'[^']*'|\\\"[^\\\"]*\\\"|[,;\\(\\)=<>!\\.]+|[ ]+)");

    auto words_begin = std::sregex_iterator(text.begin(), text.end(), wordRegex);
    auto words_end = std::sregex_iterator();

    const ThemeColors& tc = getCurrentThemeColors();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::string part = i->str();
        ImVec4 color = isGui ? ImGui::GetStyle().Colors[ImGuiCol_Text] : GUI_WHITE;
        std::string cliColor = CLI_WHITE;

        // applying correct colors based on the token type
        if (std::regex_match(part, regKeywords)) { color = tc.kwColor; cliColor = CLI_RED; }
        else if (std::regex_match(part, regTypes)) { color = tc.typeColor; cliColor = CLI_PINK; }
        else if (std::regex_match(part, regNumbers)) { color = tc.numColor; cliColor = CLI_YELLOW; }
        else if (std::regex_match(part, regStrings)) { color = tc.strColor; cliColor = CLI_GREEN; }

        if (isGui) {
            if (part[0] == ' ') {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcTextSize(part.c_str()).x);
            } else {
                ImGui::TextColored(color, "%s", part.c_str());
                ImGui::SameLine(0, 0); // keeps symbols like '(' or '*' with no space
            }
        } else {
            std::cout << cliColor << part << CLI_RESET;
        }
    }
    if (isGui) ImGui::NewLine(); else std::cout << std::flush;
}

// method for formatting and printing table results in console
void Utils::printTableCLI() {
    if (gui_headers.empty()) return;
    std::vector<size_t> colWidths(gui_headers.size());
    for (size_t i = 0; i < gui_headers.size(); ++i) {
        colWidths[i] = gui_headers[i].length();
        for (const auto& row : gui_results) {
            if (i < row.size()) {
                colWidths[i] = std::max(colWidths[i], row[i].length());
            }
        }
    }
    auto sep = [&]() {
        std::cout << "+";
        for (size_t w : colWidths) std::cout << std::string(w + 2, '-') << "+";
        std::cout << std::endl;
    };
    sep();
    std::cout << "|";
    for (size_t i = 0; i < gui_headers.size(); ++i) {
        std::cout << " " << std::left << std::setw(colWidths[i]) << gui_headers[i] << " |";
    }
    std::cout << std::endl;
    sep();
    for (const auto& row : gui_results) {
        std::cout << "|";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << " " << std::left << std::setw(colWidths[i]) << row[i] << " |";
        }
        std::cout << std::endl;
    }
    sep();
}

void Utils::runCLI() {
    std::cout << "SQL CLI Mode (type 'exit' to quit)\n";
    std::cout << "Available tables: (type 'show tables' to see)\n";

    std::string line;
    std::string queryBuffer;

    while (true) {
        std::cout << "sqW> ";
        std::cout.flush();

        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) continue;

        if (line == "exit" || line == "quit") {
            break;
        }

        if (line == "show tables" || line == "SHOW TABLES") {
            auto tables = db.getTableNames();
            std::cout << "\nTables in database:\n";
            for (const auto& t : tables) {
                std::cout << "  - " << t << "\n";
            }
            std::cout << std::endl;
            continue;
        }

        queryBuffer += line + " ";

        if (line.find(';') != std::string::npos) {
            executeSQL(queryBuffer.c_str());
            queryBuffer.clear();

            if (!gui_error.empty()) {
                std::cout << CLI_RED << "Error: " << gui_error << CLI_RESET << std::endl;
            } else if (!gui_headers.empty()) {
                printTableCLI();
            }
        }
    }
}
