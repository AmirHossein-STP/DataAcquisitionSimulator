#pragma once

WaitForLastSubmittedFrame();

// Cleanup
ImGui_ImplDX12_Shutdown();
ImGui_ImplWin32_Shutdown();
ImPlot::DestroyContext();
ImGui::DestroyContext();

CleanupDeviceD3D();
::DestroyWindow(hwnd);
::UnregisterClassW(wc.lpszClassName, wc.hInstance);