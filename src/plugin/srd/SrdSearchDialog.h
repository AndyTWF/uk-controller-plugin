#pragma once
#include "api/ApiInterface.h"

namespace UKControllerPlugin {
    namespace Srd {

        /*
            A class for performing SRD searches
        */
        class SrdSearchDialog
        {
            public:
            explicit SrdSearchDialog(const UKControllerPlugin::Api::ApiInterface& api);
            static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
            bool SearchResultsValid(const nlohmann::json& results) const;
            std::string FormatNotes(const nlohmann::json& json, size_t selectedIndex) const;

            private:
            LRESULT _WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
            void InitDialog(HWND hwnd, LPARAM lParam);
            void StartSearch(HWND hwnd);
            void CopyRouteStringToClipboard(HWND hwnd);
            void SelectSearchResult(HWND hwnd, NMLISTVIEW* details);
            void PrepopulateSearch(HWND hwnd, LPARAM lParam);

            // The API for making searches
            const UKControllerPlugin::Api::ApiInterface& api;

            // No results found JSON
            const nlohmann::json noResultsFound = nlohmann::json::array();

            // The JSON result of the previous search
            nlohmann::json previousSearchResults = noResultsFound;

            // The selected search result
            size_t selectedResult = 0;
        };
    } // namespace Srd
} // namespace UKControllerPlugin
