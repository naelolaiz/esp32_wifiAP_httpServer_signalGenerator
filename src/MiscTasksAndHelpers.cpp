#include "MiscTasksAndHelpers.h"

std::atomic<std::optional<bool>> Misc::OnBoardLedMonitor::mRequestedValue =
    std::optional<bool>();