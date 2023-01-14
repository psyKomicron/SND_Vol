#include "pch.h"
#include "ProcessInfo.h"

#include <appmodel.h>
#include <ShObjIdl.h>
#include <objbase.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include "ManifestApplicationNode.h"

using namespace std;
using namespace winrt;


namespace System
{
    ProcessInfo::ProcessInfo(const PID& pid)
    {
        GetProcessInfo(pid);
    }


    void ProcessInfo::GetProcessInfo(const PID& pid)
    {
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
        check_pointer(processHandle);

        if (!GetProcessInfoUWP(processHandle))
        {
            OutputDebugHString(L"Failed to get process info from package (UWP-like) for PID " + to_hstring((uint64_t)pid));

            if (!GetProcessInfoWin32(processHandle))
            {
                OutputDebugHString(L"Failed to get process info from process handle for PID " + to_hstring((uint64_t)pid));

                DEBUG_BREAK(); // Failed to retreive infos for this process by searching through the app's manifest or using classic Win32 methods.
            }
        }

        CloseHandle(processHandle);
    }

    bool ProcessInfo::GetProcessInfoWin32(const HANDLE& processHandle)
    {
        return false;
    }

    bool ProcessInfo::GetProcessInfoUWP(const HANDLE& processHandle)
    {
        bool success = false;
        uint32_t packageFullNameLength = 0;
        long result = GetPackageFullName(processHandle, &packageFullNameLength, nullptr);
        if (result != ERROR_INSUFFICIENT_BUFFER)
        {
            return false;
        }

        PWSTR packageFullNameWstr = new WCHAR[packageFullNameLength](0);
        if (SUCCEEDED(GetPackageFullName(processHandle, &packageFullNameLength, packageFullNameWstr)))
        {
            PACKAGE_INFO_REFERENCE packageInfoReference{};
            if (SUCCEEDED(OpenPackageInfoByFullName(packageFullNameWstr, 0, &packageInfoReference)))
            {
                uint32_t bufferLength = 0;
                uint32_t count = 0;
                if (GetPackageInfo(packageInfoReference, PACKAGE_FILTER_HEAD, &bufferLength, nullptr, &count) == ERROR_INSUFFICIENT_BUFFER)
                {
                    BYTE* bytes = (BYTE*)malloc(bufferLength * sizeof(BYTE));
                    if (SUCCEEDED(GetPackageInfo(packageInfoReference, PACKAGE_FILTER_HEAD, &bufferLength, bytes, &count)))
                    {
                        try
                        {
                            PACKAGE_INFO* packageInfos = reinterpret_cast<PACKAGE_INFO*>(bytes);
                            if (packageInfos)
                            {
                                for (uint32_t i = 0; i < count; i++)
                                {
                                    PACKAGE_INFO packageInfo = packageInfos[i];

                                    // Create AppX factory.
                                    IAppxFactoryPtr factory = nullptr;
                                    check_hresult(CoCreateInstance(__uuidof(AppxFactory), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)));

                                    // Get the AppXManifest.xml path for the package.
                                    wchar_t appxManifest[2048](0); // 32 768 bits -- Supported by UTF-8 APIs
                                    PathCombine(appxManifest, packageInfo.path, L"AppXManifest.xml");
                                    if (lstrlen(appxManifest) > 0) // Check if PathCombine succeeded.
                                    {
                                        // Create stream and package reader to read package data.
                                        IStreamPtr streamPtr = nullptr;
                                        check_hresult(SHCreateStreamOnFileEx(appxManifest, STGM_SHARE_DENY_NONE, 0, false, nullptr, &streamPtr));

                                        IAppxManifestReaderPtr manifestReaderPtr = nullptr;
                                        check_hresult(factory->CreateManifestReader(streamPtr, &manifestReaderPtr));

                                        // Get the manifest data for the package.
                                        //  Get package display name through manifest properties.
                                        IAppxManifestPropertiesPtr properties = nullptr;
                                        manifestReaderPtr->GetProperties(&properties);

                                        /*LPWSTR packageDisplayNameProperty = nullptr;
                                        if (SUCCEEDED(properties->GetStringValue(L"DisplayName", &packageDisplayNameProperty)))
                                        {
                                            name = to_hstring(packageDisplayNameProperty);
                                            CoTaskMemFree(packageDisplayNameProperty);
                                        }*/

                                        //  Get manifest Applications node to get package data.
                                        IAppxManifestApplicationsEnumeratorPtr manifestResEnumeratorPtr = nullptr;
                                        manifestReaderPtr->GetApplications(&manifestResEnumeratorPtr);
                                        BOOL getHasCurrent = false;
                                        while (SUCCEEDED(manifestResEnumeratorPtr->GetHasCurrent(&getHasCurrent)) && getHasCurrent)
                                        {
                                            IAppxManifestApplicationPtr application = nullptr;
                                            if (FAILED(manifestResEnumeratorPtr->GetCurrent(&application)))
                                            {
                                                continue;
                                            }

                                            System::AppX::ManifestApplicationNode applicationNode{ application, packageInfo.path };
                                            if (name.empty())
                                            {
                                                name = applicationNode.DisplayName();
                                            }

                                            wstring icon = applicationNode.Logo();
                                            if (!icon.empty())
                                            {
                                                // TODO: Icon handling.
                                            }

                                            manifestResEnumeratorPtr->MoveNext(&getHasCurrent);
                                        }

                                        success = true;

                                        // I don't need to close ComPtrs thanks to the smart ComPtrs definitions.

                                        CloseHandle(processHandle); // Close the handle here to free up work.
                                    }
                                }
                            }
                        }
                        catch (const hresult_error& err)
                        {
                            OutputDebugHString(err.message());
                        }
                        catch (const std::exception& ex)
                        {
                            OutputDebugHString(to_hstring(ex.what()));
                        }
                    }

                    free(bytes);
                }
                else
                {
                    OutputDebugString(L"Failed to get package info.\n");
                }
            }
            else
            {
                OutputDebugString(L"Failed to open package.\n");
            }

            ClosePackageInfo(packageInfoReference);
            delete[] packageFullNameWstr;
        }
        else
        {
            OutputDebugString(L"Failed to get package full name.\n");
        }

        return success;
    }
}
