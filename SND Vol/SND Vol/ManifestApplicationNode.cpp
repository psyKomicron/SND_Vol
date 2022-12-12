#include "pch.h"
#include "ManifestApplicationNode.h"

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
            logo = path + wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"SmallLogo", &value);
        if (value)
        {
            smallLogo = path + wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square150x150Logo", &value);
        if (value)
        {
            square150x150Logo = path + wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square70x70Logo", &value);
        if (value)
        {
            square70x70Logo = path + wstring(value);
            CoTaskMemFree(value);
        }

        application->GetStringValue(L"Square30x30Logo", &value);
        if (value)
        {
            square30x30Logo = path + wstring(value);
            CoTaskMemFree(value);
        }
	}
}