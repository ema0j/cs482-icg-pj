#include "background.h"

void Background::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Backgrounds");
    stat->Increment();        
}

