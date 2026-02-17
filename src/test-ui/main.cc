#include "Global.h"
#include "AImGui.h"

// Include tambahan untuk fungsi Terminal & Environment
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <cstdio>
#include <stdlib.h>
#include <iostream>

// --- KELAS TERMINAL TERMUX ---
class ImGuiTerminal {
public:
    std::vector<std::string> Logs;
    char InputBuf[256];
    std::mutex LogMutex;
    bool ScrollToBottom = false;

    ImGuiTerminal() {
        memset(InputBuf, 0, sizeof(InputBuf));
        Logs.push_back("--- Termux ImGui Terminal Ready ---");
    }

    void LoadTermuxEnv() {
        const char* prefix = "/data/data/com.termux/files/usr";
        const char* home   = "/data/data/com.termux/files/home";

        // Setup environment agar menunjuk ke binary Termux
        setenv("PATH", (std::string(prefix) + "/bin:" + getenv("PATH")).c_str(), 1);
        setenv("LD_PRELOAD", (std::string(prefix) + "/lib/libtermux-exec-ld-preload.so").c_str(), 1);
        setenv("HOME", home, 1);
        setenv("PREFIX", prefix, 1);
        setenv("TMPDIR", (std::string(prefix) + "/tmp").c_str(), 1);
        setenv("TERM", "xterm-256color", 1);
        setenv("COLORTERM", "truecolor", 1);
    }

    void Execute(std::string command) {
        // Jalankan di thread terpisah agar UI tidak freeze
        std::thread([this, command]() {
            LoadTermuxEnv();
            
            std::string bash_path = "/data/data/com.termux/files/usr/bin/bash";
            // Gabungkan stderr ke stdout agar pesan error terlihat
            std::string final_cmd = bash_path + " -c \"" + command + "\" 2>&1";

            FILE* pipe = popen(final_cmd.c_str(), "r");
            if (!pipe) {
                std::lock_guard<std::mutex> lock(LogMutex);
                Logs.push_back("[!] Gagal membuka pipe.");
                return;
            }

            char buffer[256];
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                std::lock_guard<std::mutex> lock(LogMutex);
                Logs.push_back(std::string(buffer));
                ScrollToBottom = true;
            }
            pclose(pipe);
        }).detach();
    }

    void Draw(const char* title, bool* p_open) {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open)) {
            ImGui::End();
            return;
        }

        // Tampilan Gelap khas Terminal
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.05f, 0.05f, 0.05f, 1.0f));
        const float footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        
        ImGui::BeginChild("LogRegion", ImVec2(0, -footer_height), true);
        {
            std::lock_guard<std::mutex> lock(LogMutex);
            for (const auto& line : Logs) {
                if (line.find("> ") == 0)
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", line.c_str()); // Warna Hijau untuk Prompt
                else
                    ImGui::TextUnformatted(line.c_str()); // Putih untuk Output
            }
            if (ScrollToBottom) {
                ImGui::SetScrollHereY(1.0f);
                ScrollToBottom = false;
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        ImGui::Separator();

        // Input Text
        bool reclaim_focus = false;
        if (ImGui::InputText("Command", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
            std::string cmd(InputBuf);
            if (!cmd.empty()) {
                Logs.push_back("> " + cmd);
                Execute(cmd);
            }
            memset(InputBuf, 0, sizeof(InputBuf));
            reclaim_focus = true;
        }
        
        ImGui::SetItemDefaultFocus();
        if (reclaim_focus) ImGui::SetKeyboardFocusHere(-1);

        ImGui::End();
    }
};

// --- MAIN FUNCTION ---
int main()
{
    android::AImGui imgui(android::AImGui::Options{.renderType = android::AImGui::RenderType::RenderNative, .autoUpdateOrientation = true});
    bool state = true;
    bool showTermux = true; // State untuk jendela Terminal
    
    if (!imgui)
    {
        LogInfo("[-] ImGui initialization failed");
        return 0;
    }

    // Inisialisasi terminal
    static ImGuiTerminal terminal;

    std::thread processInputEventThread(
        [&]
        {
            while (state)
            {
                imgui.ProcessInputEvent();
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });

    while (state)
    {
        imgui.BeginFrame();

        // Panggil jendela Terminal Termux
        if (showTermux) {
            terminal.Draw("Termux ImGui Overlay", &showTermux);
        }

        // Contoh window bawaan repo tetap dipertahankan
        {
            ImGui::Begin("Settings", &state);
            ImGui::Checkbox("Tampilkan Terminal", &showTermux);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            if (ImGui::Button("Exit")) state = false;
            ImGui::End();
        }

        imgui.EndFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (processInputEventThread.joinable())
        processInputEventThread.join();

    return 0;
}
