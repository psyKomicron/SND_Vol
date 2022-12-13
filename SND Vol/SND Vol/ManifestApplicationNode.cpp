#include "pch.h"
#include "ManifestApplicationNode.h"
#include <Shlwapi.h>

using namespace std;


namespace System::AppX
{
	ManifestApplicationNode::ManifestApplicationNode(IAppxManifestApplicationPtr& application, PWSTR packagePath)
	{
        wstring path = wstring(packagePath) + L"\\";

        PWSTR value = nullptr;
        application->GetStringValue(L"Description", &value);
        if (value)
        {
            description = wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"DisplayName", &value);
        if (value)
        {
            displayName = wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"BackgroundColor", &value);
        if (value)
        {
            backgroundColor = wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"ForegroundText", &value);
        if (value)
        {
            foregroundText = wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"ShortName", &value);
        if (value)
        {
            shortName = wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Logo", &value);
        if (value)
        {
            logo = GetScale(path + wstring(value));
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"SmallLogo", &value);
        if (value)
        {
            smallLogo = GetScale(path + wstring(value));
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square150x150Logo", &value);
        if (value)
        {
            square150x150Logo = GetScale(path + wstring(value));
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square70x70Logo", &value);
        if (value)
        {
            square70x70Logo = GetScale(path + wstring(value));
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square30x30Logo", &value);
        if (value)
        {
            square30x30Logo = GetScale(path + wstring(value));
            CoTaskMemFree(value);
        }
	}

    wstring ManifestApplicationNode::GetScale(wstring path)
    {
        wstring filePathWithoutExt{};
        wstring ext{};
        for (int i = path.size() - 1; i >= 0; i--)
        {
            if (path[i] == '.')
            {
                filePathWithoutExt = path.substr(0, i);
                ext = path.substr(i);
                break;
            }
        }

        if (!filePathWithoutExt.empty() && !ext.empty())
        {
            filePathWithoutExt += L".scale-100";
            wstring filePath = filePathWithoutExt + ext;
            if (PathFileExists(filePath.c_str()))
            {
                return filePath;
            }
        }

        return wstring();
    }
}