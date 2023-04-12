// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <wrl/module.h>
#include <wrl/implements.h>
#include <shobjidl_core.h>
#include <wil/resource.h>
#include <Shellapi.h>
#include <Shlwapi.h>
#include <Strsafe.h>


using namespace Microsoft::WRL;

HMODULE g_hModule = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


class HelloWorldCommand : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand, IObjectWithSite>
{
public:
    // IExplorerCommand methods
    IFACEMETHODIMP GetTitle(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* name)
    {
        *name = nullptr;
        auto title = wil::make_cotaskmem_string_nothrow(L"Edit in Notepad");
        RETURN_IF_NULL_ALLOC(title);
        *name = title.release();
        return S_OK;
    }

    IFACEMETHODIMP GetIcon(_In_opt_ IShellItemArray* items, _Outptr_result_nullonfailure_ PWSTR* iconPath)
    {
        *iconPath = nullptr;
        PWSTR itemPath = nullptr;

        if (items)
        {
            DWORD count;
            RETURN_IF_FAILED(items->GetCount(&count));

            if (count > 0)
            {
                ComPtr<IShellItem> item;
                RETURN_IF_FAILED(items->GetItemAt(0, &item));

                RETURN_IF_FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &itemPath));
                wil::unique_cotaskmem_string itemPathCleanup(itemPath);

                WCHAR modulePath[MAX_PATH];
                if (GetModuleFileNameW(g_hModule, modulePath, ARRAYSIZE(modulePath)))
                {
                    PathRemoveFileSpecW(modulePath);
                    StringCchCatW(modulePath, ARRAYSIZE(modulePath), L"\\Notepad.ico");

                    auto iconPathStr = wil::make_cotaskmem_string_nothrow(modulePath);
                    if (iconPathStr)
                    {
                        *iconPath = iconPathStr.release();
                    }
                }
            }
        }

//#ifdef _DEBUG
//        if (itemPath)
//        {
//            WCHAR logPath[MAX_PATH];
//            StringCchCopyW(logPath, ARRAYSIZE(logPath), itemPath);
//            PathRemoveFileSpecW(logPath);
//            StringCchCatW(logPath, ARRAYSIZE(logPath), L"\\icon_debug_log.txt");
//
//            FILE* logFile;
//            _wfopen_s(&logFile, logPath, L"w");
//            if (logFile)
//            {
//                if (*iconPath)
//                {
//                    fwprintf(logFile, L"Icon loaded successfully: %s\n", *iconPath);
//                }
//                else
//                {
//                    fwprintf(logFile, L"Failed to load icon.\n");
//                }
//
//                fclose(logFile);
//            }
//            else
//            {
//                fwprintf(stderr, L"Failed to create debug log file: %s\n", logPath);
//            }
//        }
//#endif

        return *iconPath ? S_OK : E_FAIL;
    }




    IFACEMETHODIMP GetToolTip(_In_opt_ IShellItemArray*, _Outptr_result_nullonfailure_ PWSTR* infoTip) { *infoTip = nullptr; return E_NOTIMPL; }
    IFACEMETHODIMP GetCanonicalName(_Out_ GUID* guidCommandName) { *guidCommandName = GUID_NULL;  return S_OK; }
    IFACEMETHODIMP GetState(_In_opt_ IShellItemArray* selection, _In_ BOOL okToBeSlow, _Out_ EXPCMDSTATE* cmdState)
    {
        *cmdState = ECS_ENABLED;
        return S_OK;
    }

    IFACEMETHODIMP Invoke(_In_opt_ IShellItemArray* selection, _In_opt_ IBindCtx*) noexcept try
    {
        if (selection)
        {
            DWORD count;
            RETURN_IF_FAILED(selection->GetCount(&count));
            if (count > 0)
            {
                ComPtr<IShellItem> item;
                RETURN_IF_FAILED(selection->GetItemAt(0, &item));

                PWSTR filePath;
                RETURN_IF_FAILED(item->GetDisplayName(SIGDN_FILESYSPATH, &filePath));
                wil::unique_cotaskmem_string filePathCleanup(filePath);

                SHELLEXECUTEINFO sei = { 0 };
                sei.cbSize = sizeof(SHELLEXECUTEINFO);
                sei.fMask = SEE_MASK_DEFAULT;
                sei.lpVerb = L"open";
                sei.lpFile = L"notepad.exe";
                sei.lpParameters = filePath;
                sei.nShow = SW_SHOWNORMAL;

                if (!ShellExecuteEx(&sei))
                {
                    return HRESULT_FROM_WIN32(GetLastError());
                }
            }
        }

        return S_OK;
    }
    CATCH_RETURN();

    IFACEMETHODIMP GetFlags(_Out_ EXPCMDFLAGS* flags) { *flags = ECF_DEFAULT; return S_OK; }
    IFACEMETHODIMP EnumSubCommands(_COM_Outptr_ IEnumExplorerCommand** enumCommands) { *enumCommands = nullptr; return E_NOTIMPL; }

    // IObjectWithSite methods
    IFACEMETHODIMP SetSite(_In_ IUnknown* site) noexcept { m_site = site; return S_OK; }
    IFACEMETHODIMP GetSite(_In_ REFIID riid, _COM_Outptr_ void** site) noexcept { return m_site.CopyTo(riid, site); }

protected:
    ComPtr<IUnknown> m_site;
};


class __declspec(uuid("7A1E471F-0D43-4122-B1C4-D1AACE76CE9B")) HelloWorldCommand1 final : public HelloWorldCommand
{
//public:
    //const wchar_t* Title() override { return L"HelloWorld Command1"; }
    //const EXPCMDSTATE State(_In_opt_ IShellItemArray* selection) override { return ECS_DISABLED; }
};

CoCreatableClass(HelloWorldCommand1)


STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}
