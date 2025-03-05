#pragma once

//// 3. Show another simple window.
//if (show_another_window)
//{
//    ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
//    ImGui::Text("Hello from another window!");
//    if (ImGui::Button("Close Me"))
//        show_another_window = false;
//    ImGui::End();
//}

// Rendering

ImGui::Render();

FrameContext* frameCtx = WaitForNextFrameResources();
UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
frameCtx->CommandAllocator->Reset();

D3D12_RESOURCE_BARRIER barrier = {};
barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
g_pd3dCommandList->ResourceBarrier(1, &barrier);

// Render Dear ImGui graphics
const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
g_pd3dCommandList->ResourceBarrier(1, &barrier);
g_pd3dCommandList->Close();

g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

// Present
HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);

UINT64 fenceValue = g_fenceLastSignaledValue + 1;
g_pd3dCommandQueue->Signal(g_fence, fenceValue);
g_fenceLastSignaledValue = fenceValue;
frameCtx->FenceValue = fenceValue;