#pragma once



// Create application window
//ImGui_ImplWin32_EnableDpiAwareness();
WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
::RegisterClassExW(&wc);
HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Data Generator", WS_OVERLAPPEDWINDOW, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);

// Initialize Direct3D
if (!CreateDeviceD3D(hwnd))
{
    CleanupDeviceD3D();
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 1;
}

// Show the window
::ShowWindow(hwnd, SW_SHOWDEFAULT);
::UpdateWindow(hwnd);

// Setup Dear ImGui context
IMGUI_CHECKVERSION();
ImGui::CreateContext();
ImPlot::CreateContext();
ImGuiIO& io = ImGui::GetIO(); (void)io;
io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

// Setup Dear ImGui style
ImGui::StyleColorsDark();
//ImGui::StyleColorsLight();

// Setup Platform/Renderer backends
ImGui_ImplWin32_Init(hwnd);

ImGui_ImplDX12_InitInfo init_info = {};
init_info.Device = g_pd3dDevice;
init_info.CommandQueue = g_pd3dCommandQueue;
init_info.NumFramesInFlight = APP_NUM_FRAMES_IN_FLIGHT;
init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
init_info.SrvDescriptorHeap = g_pd3dSrvDescHeap;
init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return g_pd3dSrvDescHeapAlloc.Alloc(out_cpu_handle, out_gpu_handle); };
init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return g_pd3dSrvDescHeapAlloc.Free(cpu_handle, gpu_handle); };
ImGui_ImplDX12_Init(&init_info);

// Load Fonts
// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
// - Read 'docs/FONTS.md' for more instructions and details.
// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
//io.Fonts->AddFontDefault();
io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
//IM_ASSERT(font != nullptr);

// Our state
bool show_demo_window = false;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);