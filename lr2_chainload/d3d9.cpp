#include <string>
#include <fstream>
#include <windows.h>
#include <filesystem>

static std::filesystem::path d3d9path;

struct d3d9_dll {
	HMODULE dll;
	FARPROC OrignalD3DPERF_BeginEvent;
	FARPROC OrignalD3DPERF_EndEvent;
	FARPROC OrignalD3DPERF_GetStatus;
	FARPROC OrignalD3DPERF_QueryRepeatFrame;
	FARPROC OrignalD3DPERF_SetMarker;
	FARPROC OrignalD3DPERF_SetOptions;
	FARPROC OrignalD3DPERF_SetRegion;
	FARPROC OrignalDebugSetLevel;
	FARPROC OrignalDebugSetMute;
	FARPROC OrignalDirect3D9EnableMaximizedWindowedModeShim;
	FARPROC OrignalDirect3DCreate9;
	FARPROC OrignalDirect3DCreate9Ex;
	FARPROC OrignalDirect3DCreate9On12;
	FARPROC OrignalDirect3DCreate9On12Ex;
	FARPROC OrignalDirect3DShaderValidatorCreate9;
	FARPROC OrignalPSGPError;
	FARPROC OrignalPSGPSampleTexture;
} d3d9;

__declspec(naked) void FakeD3DPERF_BeginEvent() { _asm { jmp[d3d9.OrignalD3DPERF_BeginEvent] } }
__declspec(naked) void FakeD3DPERF_EndEvent() { _asm { jmp[d3d9.OrignalD3DPERF_EndEvent] } }
__declspec(naked) void FakeD3DPERF_GetStatus() { _asm { jmp[d3d9.OrignalD3DPERF_GetStatus] } }
__declspec(naked) void FakeD3DPERF_QueryRepeatFrame() { _asm { jmp[d3d9.OrignalD3DPERF_QueryRepeatFrame] } }
__declspec(naked) void FakeD3DPERF_SetMarker() { _asm { jmp[d3d9.OrignalD3DPERF_SetMarker] } }
__declspec(naked) void FakeD3DPERF_SetOptions() { _asm { jmp[d3d9.OrignalD3DPERF_SetOptions] } }
__declspec(naked) void FakeD3DPERF_SetRegion() { _asm { jmp[d3d9.OrignalD3DPERF_SetRegion] } }
__declspec(naked) void FakeDebugSetLevel() { _asm { jmp[d3d9.OrignalDebugSetLevel] } }
__declspec(naked) void FakeDebugSetMute() { _asm { jmp[d3d9.OrignalDebugSetMute] } }
__declspec(naked) void FakeDirect3D9EnableMaximizedWindowedModeShim() { _asm { jmp[d3d9.OrignalDirect3D9EnableMaximizedWindowedModeShim] } }
__declspec(naked) void FakeDirect3DCreate9() { _asm { jmp[d3d9.OrignalDirect3DCreate9] } }
__declspec(naked) void FakeDirect3DCreate9Ex() { _asm { jmp[d3d9.OrignalDirect3DCreate9Ex] } }
__declspec(naked) void FakeDirect3DCreate9On12() { _asm { jmp[d3d9.OrignalDirect3DCreate9On12] } }
__declspec(naked) void FakeDirect3DCreate9On12Ex() { _asm { jmp[d3d9.OrignalDirect3DCreate9On12Ex] } }
__declspec(naked) void FakeDirect3DShaderValidatorCreate9() { _asm { jmp[d3d9.OrignalDirect3DShaderValidatorCreate9] } }
__declspec(naked) void FakePSGPError() { _asm { jmp[d3d9.OrignalPSGPError] } }
__declspec(naked) void FakePSGPSampleTexture() { _asm { jmp[d3d9.OrignalPSGPSampleTexture] } }

void LoadExternalD3D9(HMODULE hModule) {
	TCHAR module_path[MAX_PATH] = {};
	GetModuleFileName(hModule, module_path, MAX_PATH);
	auto library_list = std::wifstream(
		// Strip the filename part from the module path, then add 'chainload.txt'.
		std::filesystem::path(module_path).remove_filename().append("chainload.txt")
	);

	if (library_list.is_open())
	{
		for (std::wstring line; std::getline(library_list, line);)
		{
			// Find the external d3d9 directory
			if (line.starts_with(L"d3d9_overwrite=")) {
				d3d9path = line.substr(std::wstring_view(L"d3d9_overwrite=").length());
				break;
			}
		}
	}
}

void LoadDlls(HMODULE hModule) {
	TCHAR module_path[MAX_PATH] = {};
	GetModuleFileName(hModule, module_path, MAX_PATH);
	auto library_list = std::wifstream(
		// Strip the filename part from the module path, then add 'chainload.txt'.
		std::filesystem::path(module_path).remove_filename().append("chainload.txt")
	);

	if (library_list.is_open())
	{
		// Read filenames from each line and call LoadLibrary.
		for (std::wstring line; std::getline(library_list, line);)
		{
			// Treat lines starting with '#' as comments.
			if (line.empty() || line.starts_with(L"#") || line.starts_with(L"d3d9_overwrite="))
				continue;

			LoadLibrary(line.c_str());
		}
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	TCHAR system_dir[MAX_PATH];
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		LoadExternalD3D9(hModule);
		GetSystemDirectory(system_dir, MAX_PATH);

		if (d3d9path.empty()) {
			d3d9.dll = LoadLibrary(std::filesystem::path(system_dir).append("d3d9.dll").c_str());
		}
		else {
			d3d9.dll = LoadLibrary(d3d9path.append("d3d9.dll").c_str());
		}

		if (d3d9.dll == 0)
		{
			MessageBox(0, L"Cannot load original d3d9.dll library", L"Proxy", MB_ICONERROR);
			ExitProcess(0);
		}
		d3d9.OrignalD3DPERF_BeginEvent = GetProcAddress(d3d9.dll, "D3DPERF_BeginEvent");
		d3d9.OrignalD3DPERF_EndEvent = GetProcAddress(d3d9.dll, "D3DPERF_EndEvent");
		d3d9.OrignalD3DPERF_GetStatus = GetProcAddress(d3d9.dll, "D3DPERF_GetStatus");
		d3d9.OrignalD3DPERF_QueryRepeatFrame = GetProcAddress(d3d9.dll, "D3DPERF_QueryRepeatFrame");
		d3d9.OrignalD3DPERF_SetMarker = GetProcAddress(d3d9.dll, "D3DPERF_SetMarker");
		d3d9.OrignalD3DPERF_SetOptions = GetProcAddress(d3d9.dll, "D3DPERF_SetOptions");
		d3d9.OrignalD3DPERF_SetRegion = GetProcAddress(d3d9.dll, "D3DPERF_SetRegion");
		d3d9.OrignalDebugSetLevel = GetProcAddress(d3d9.dll, "DebugSetLevel");
		d3d9.OrignalDebugSetMute = GetProcAddress(d3d9.dll, "DebugSetMute");
		d3d9.OrignalDirect3D9EnableMaximizedWindowedModeShim = GetProcAddress(d3d9.dll, "Direct3D9EnableMaximizedWindowedModeShim");
		d3d9.OrignalDirect3DCreate9 = GetProcAddress(d3d9.dll, "Direct3DCreate9");
		d3d9.OrignalDirect3DCreate9Ex = GetProcAddress(d3d9.dll, "Direct3DCreate9Ex");
		d3d9.OrignalDirect3DCreate9On12 = GetProcAddress(d3d9.dll, "Direct3DCreate9On12");
		d3d9.OrignalDirect3DCreate9On12Ex = GetProcAddress(d3d9.dll, "Direct3DCreate9On12Ex");
		d3d9.OrignalDirect3DShaderValidatorCreate9 = GetProcAddress(d3d9.dll, "Direct3DShaderValidatorCreate9");
		d3d9.OrignalPSGPError = GetProcAddress(d3d9.dll, "PSGPError");
		d3d9.OrignalPSGPSampleTexture = GetProcAddress(d3d9.dll, "PSGPSampleTexture");

		LoadDlls(hModule);

		break;
	}
	case DLL_PROCESS_DETACH:
	{
		FreeLibrary(d3d9.dll);
	}
	break;
	}
	return TRUE;
}
