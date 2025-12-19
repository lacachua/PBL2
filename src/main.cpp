#include "core/App.h"
#include "services/ShowtimeCleanupService.h"
#include <string>
#include <filesystem>

int main(int argc, char** argv) {
    // Ensure relative paths like "../data/..." work no matter where the process is launched from.
    // Project layout assumes the executable lives under "build/" next to the repo root.
    {
        std::error_code ec;
        const auto exePath = std::filesystem::absolute(argv[0], ec);
        if (!ec) {
            const auto exeDir = exePath.parent_path();
            std::filesystem::current_path(exeDir, ec);
        }
    }

    if (argc > 1) {
        const std::string arg = argv[1];
        if (arg == "--maintain-showtimes") {
            ShowtimeCleanupService::maintainShowtimes("../data/showtimes.txt", 5);
            return 0;
        }
        if (arg == "--regenerate-showtimes") {
            ShowtimeCleanupService::forceRegenerate("../data/showtimes.txt", 5);
            return 0;
        }
    }

    App app;
    app.run();
    return 0;
}