#include "lens.h"

void Lens::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Lenses");
    stat->Increment();
}
