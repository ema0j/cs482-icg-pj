#include "source.h"

void Source::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Sources");
    stat->Increment();
}
