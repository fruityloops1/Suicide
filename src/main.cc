#include <cstdio>
#include <cstdlib>

#include <imgui.h>
#include <switch.h>
#include <switch/kernel/svc.h>
#include <switch/runtime/devices/fs_dev.h>
#include <switch/runtime/diag.h>
#include <switch/services/applet.h>
#include <switch/services/fs.h>
#include <switch/services/nv.h>

#include "LogServer.h"
#include "imgui_impl_deko3d.h"
#include "types.h"
#include "util.h"

extern "C" u32 __nx_nv_service_type = NvServiceType_Factory;
extern "C" u32 __nx_nv_transfermem_size = 0x300000;
extern "C" size_t __nx_heap_size = 0x200000 * 8;

extern "C" void __appInit(void)
{
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
    }

    rc = setInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(rc);

    R_ABORT_UNLESS(plInitialize(PlServiceType_User));

    static const SocketInitConfig socketInitConfig = {
        .tcp_tx_buf_size = 0x1000,
        .tcp_rx_buf_size = 0x1000,
        .tcp_tx_buf_max_size = 0,
        .tcp_rx_buf_max_size = 0,

        .udp_tx_buf_size = 0x2400,
        .udp_rx_buf_size = 0xA500,

        .sb_efficiency = 2,

        .num_bsd_sessions = 3,
        .bsd_service_type = BsdServiceType_User,
    };

    R_ABORT_UNLESS(socketInitialize(&socketInitConfig));

    // R_ABORT_UNLESS(appletInitialize());

    R_ABORT_UNLESS(hidInitialize());

    rc = viInitialize(ViServiceType_Manager);
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));

    R_ABORT_UNLESS(fsInitialize());
    R_ABORT_UNLESS(fsdevMountSdmc());

    rc = pmdmntInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));

    rc = capsscInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));

    rc = spsmInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
}

extern "C" void __appExit(void)
{
    plExit();
    viExit();
    hidExit();
    pmdmntExit();
    fsdevUnmountAll();
    appletExit();
    fsExit();
}

int main(int argc, char* argv[])
{

    nxdb::LogServer::instance() = new nxdb::LogServer;
    nxdb::LogServer::instance()->StartServer();

    for (int i = 0; i < 5; i++) {
        nxdb::LogServer::instance()->Poll();
        nxdb::log("Waiting %d", i);
        svcSleepThread(1e+9);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    // ImGuiIO &io = ImGui::GetIO(); // to configure imgui

    ImGui_ImplDeko3d_Init();

    while (appletMainLoop()) {
        u64 down = ImGui_ImplDeko3d_UpdatePad();
        if (down & HidNpadButton_Plus) // "+" to exit
            break;

        ImGui_ImplDeko3d_NewFrame();
        ImGui::NewFrame();

        bool open;
        ImGui::ShowDemoWindow(&open);

        ImGui::Render();
        ImGui_ImplDeko3d_RenderDrawData(ImGui::GetDrawData());
    }

    ImGui_ImplDeko3d_Shutdown();
    ImGui::DestroyContext();
    return 0;
}
