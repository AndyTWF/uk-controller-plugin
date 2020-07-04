#pragma once

namespace UKControllerPlugin {
    namespace Euroscope {
        class UserSetting;
    }  // namespace Euroscope
}  // namespace UKControllerPlugin

namespace UKControllerPlugin {
    namespace Windows {
        /*
            Interface between the plugin and the Windows API.
        */
        class WinApiInterface
        {
        public:
            explicit WinApiInterface(HINSTANCE dllInstance)
                : dllInstance(dllInstance)
            {
            };
            virtual ~WinApiInterface(void) {}  // namespace Windows
            virtual bool CreateFolder(std::wstring folder) = 0;
            virtual bool CreateFolderRecursive(std::wstring folder) = 0;
            virtual bool CreateLocalFolderRecursive(std::wstring folder) = 0;
            virtual bool DeleteGivenFile(std::wstring filename) = 0;
            virtual bool FileExists(std::wstring filename) = 0;
            virtual std::wstring FileOpenDialog(
                std::wstring title,
                UINT numFileTypes,
                const COMDLG_FILTERSPEC * fileTypes
            ) const = 0;
            HINSTANCE GetDllInstance(void) const {
                return this->dllInstance;
            }
            virtual std::wstring GetFullPathToLocalFile(std::wstring relativePath) const = 0;
            virtual int OpenMessageBox(LPCWSTR message, LPCWSTR title, int options) = 0;
            virtual void PlayWave(LPCTSTR sound) = 0;
            virtual std::string ReadFromFile(std::wstring filename, bool relativePath = true) = 0;
            virtual bool SetPermissions(std::wstring fileOrFolder, std::filesystem::perms permissions) = 0;
            virtual void WriteToFile(std::wstring filename, std::string data, bool truncate) = 0;

        private:
            // DLL Instance for the plugin
            const HINSTANCE dllInstance;
        };
    }  // namespace Windows
}  // namespace UKControllerPlugin
