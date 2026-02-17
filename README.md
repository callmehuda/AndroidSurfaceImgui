This is a professional English translation of the documentation, tailored for the GitHub environment. I have polished the technical terms to ensure they align with standard Android development and ImGui terminology.
AndroidSurfaceImgui
Introduction
An Android implementation that creates a system-level Surface to perform Dear ImGui rendering.
Key Features
 * RPC Support: Supports multi-process and remote rendering via RPC.
 * Input Support: Full support for mouse wheel and physical keyboard input.
 * Auto-Rotation: Seamlessly handles switching between Portrait and Landscape modes.
 * Screen Record Protection: Supports flags to prevent UI capture during screen recording.
> [!CAUTION]
> Process Permissions: If the process owning the Surface runs with permissions lower than shell, the system will not add your Surface to the root view (it won't be visible). For technical details, see: SurfaceFlinger.cpp
> 
Supported Android Versions
Major versions are listed below; sub-versions (point releases) are supported unless stated otherwise.
| Legacy | Transition | Intermediate | Modern |
|---|---|---|---|
| ✅ Android 5 | ✅ Android 8 | ✅ Android 9 | ✅ Android 12 |
| ✅ Android 6 |  | ✅ Android 10 | ✅ Android 13 |
| ✅ Android 7 |  | ✅ Android 11 | ✅ Android 14 |
|  |  |  | ✅ Android 15 |
|  |  |  | ✅ Android 16 |
Note: All current versions can be screen-recorded normally. If you find an incompatible version, please submit an issue.
Compilation / Build Instructions
 * Open your terminal in the project root directory.
 * Initialize submodules: git submodule init
 * Update submodules: git submodule update
 * Generate build files:
   cmake -DCMAKE_BUILD_TYPE=Release \
-DCMAKE_TOOLCHAIN_FILE=${NDK_PATH}/build/cmake/android.toolchain.cmake \
-DANDROID_PLATFORM=android-23 \
-DANDROID_ABI=${PLATFORM} \
-S . -B build -G Ninja

   * Replace ${NDK_PATH} with your local Android NDK path.
   * Replace ${PLATFORM} with your target ABI (e.g., arm64-v8a).
 * Execute build: cmake --build build --config Release
Batch Build (Windows)
You can use build.bat for batch compilation. By default, it builds for armeabi-v7a, arm64-v8a, x86, and x86_64.
 * Arguments: The script accepts three optional parameters: NDK Path, Minimum SDK Version, and CMake Path.
 * If not provided, the script will auto-detect NDK_PATH and CMake via environment variables.
Usage Examples
 * Main UI Example: src/test-ui/main.cc
 * RPC Server: src/test-ui/canvas.cc
 * RPC Client: src/test-ui/render.cc
Roadmap / TODO
 * [ ] Refactor AImGui and ATouchEvent: Fully decouple event handling logic and standardize imports/include_directories.
 * [ ] Organize the Remote Rendering Demo for Android on PC (Windows).
Acknowledgments & Sponsors
Special thanks to the following sponsors for supporting this project:
 * Dasongzi1366
 * barry-ran
Thank you for your support! ❤️
Would you like me to help you analyze the main.cc file to see how it handles the root shell execution alongside this ImGui interface?
