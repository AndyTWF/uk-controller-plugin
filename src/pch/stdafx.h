#pragma once
#define _WIN32_WINNT 0x0603
#define _SILENCE_CXX17_UNCAUGHT_EXCEPTION_DEPRECATION_WARNING
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
#define NOMINMAX 1
#define ISOLATION_AWARE_ENABLED 1

// Custom headers
#pragma warning( push )
#pragma warning( disable : 26495 26451)
#include "date/include/date/date.h"
#include "../../resource/resource.h"
#include "json/json.hpp"
#include "spdlog/include/spdlog/logger.h"
#include "spdlog/include/spdlog/sinks/file_sinks.h"
#include "spdlog/include/spdlog/sinks/null_sink.h"
#include "log/LoggerFunctions.h"
#include "euroscope/EuroScopePlugIn.h"
#pragma warning( pop )

// Standard headers
#include <algorithm>
#include <CommCtrl.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")  // NOLINT
#include <CommDlg.h>
#include <shtypes.h>
#include <filesystem>
#include <cctype>
#include <ctime>
#include <string>
#include <tchar.h>
#include <map>
#include <mutex>
#include <vector>
#include <KnownFolders.h>
#include <iterator>
#include <sstream>
#include <queue>
#include <set>
#include <fstream>
#include <mmsystem.h>
#include <minmax.h>
#include <gdiplus.h>
#include <gdiplusgraphics.h>
#include <gdiplustypes.h>
#include <gdiplusenums.h>
#include <thread>
#include <regex>
#include <type_traits>
#include <gdipluspixelformats.h>
#include <unordered_set>
#include <codecvt>
#include <locale>
#include <Shobjidl.h>
#include <Shlobj.h>
#include <shellapi.h>
#include <Windows.h>
