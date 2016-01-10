#include "xform.h"

void Xform::CollectStats(StatsManager& stats) {
    StatsCounterVariable* stat = stats.GetVariable<StatsCounterVariable>("Scene", "Xforms");
    stat->Increment();
}

shared_ptr<Xform> Xform::IdentityXform() {
	static shared_ptr<Xform> identityXform = shared_ptr<Xform>();
    if(!identityXform) { 
        identityXform = shared_ptr<Xform>(new XformIdentity()); 
    } 
    return identityXform; 
}

string XformIdentity::serialize_typename() { return "XformIdentity"; }
	
void XformIdentity::serialize(Archive* a) { }
