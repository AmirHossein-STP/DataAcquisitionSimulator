#pragma once

// Poll and handle messages (inputs, window resize, etc.)
// See the WndProc() function below for our to dispatch events to the Win32 backend.
MSG msg;
while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
{
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
    if (msg.message == WM_QUIT)
        done = true;
}
if (done)
break;

// Handle window screen locked
if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
{
    ::Sleep(10);
    continue;
}
g_SwapChainOccluded = false;

// Start the Dear ImGui frame
ImGui_ImplDX12_NewFrame();
ImGui_ImplWin32_NewFrame();
ImGui::NewFrame();

//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//if (show_demo_window)
//    ImGui::ShowDemoWindow(&show_demo_window);