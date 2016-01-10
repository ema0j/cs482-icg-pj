#ifndef _STATS_H_
#define _STATS_H_

#include <misc/timer.h>
#include <float.h>
#include <string>
#include <assert.h>
#include <map>
#include <iostream>
#include <iomanip>
#include <stdint.h>
using std::map;
using std::string;
using std::ostream;
using std::setprecision;
using std::fixed;
using std::scientific;
using std::pair;
using std::endl;
using std::min;
using std::max;

class StatsBaseVariable {
protected:
    string category;
    string name;

    StatsBaseVariable(const string& c, const string& n) {
        category = c;
        name = n;
    }

    virtual void Print(ostream& os) = 0;

    static void PrintPaddedName(ostream& os, const string& name) {
        os << "    " << name.c_str();
	    // Pad out to results column
	    int resultsColumn = 56;
	    int paddingSpaces = resultsColumn - (int) name.size();
        while (paddingSpaces-- > 0) { os << " "; }
    }

    static void Print(ostream& os, double v) {
	    if (v > 1.0e9) os << fixed << setprecision(1) << v / 1.0e9f << "G";
	    else if (v >= 1.0e6) os << fixed << setprecision(1) << v / 1.0e6f << "M";
	    else if (v >= 1.0e3) os << fixed << setprecision(1) << v / 1.0e3f << "K";
	    else if (v >= 1.0e0) os << fixed << setprecision(1) << v / 1.0e0f << " ";
	    else if (v >= 1.0e-3) os << fixed << setprecision(1) << v / 1.0e-3f << "m";
	    else if (v >= 1.0e-6) os << fixed << setprecision(1) << v / 1.0e-6f << "mi";
	    else if (v >= 1.0e-9) os << fixed << setprecision(1) << v / 1.0e-9f << "n";
        else os << scientific << setprecision(1) << v / 1.0e-9f;
    }

    friend class StatsManager;
};

class StatsCounterVariable : public StatsBaseVariable {
public:
    void Increment() { val ++; }
    void Increment(double v) { val += v; }
	void Increment(uint32_t v) { val += v; }
    void Increment(uint64_t v) { val += v; }
    void Max(double v) { val = max(val, v); }
	void Max(uint32_t v) { val = max(val, (double)v); }
    void Max(uint64_t v) { val = max(val, (double)v); }
    void Decrement() { val --; }

protected:
    double val;

    StatsCounterVariable(const string& c, const string& n) : StatsBaseVariable(c, n) {
        val = 0;
    }

    void Print(ostream& os) {
        StatsBaseVariable::PrintPaddedName(os,name);
        StatsBaseVariable::Print(os,val);
    }

    friend class StatsManager;
};

class StatsRatioVariable : public StatsBaseVariable {
public:
    void Increment(double v1, double v2) { val1 += v1; val2 += v2; }

protected:
    double val1, val2;

    StatsRatioVariable(const string& c, const string& n) : StatsBaseVariable(c, n) {
        val1 = val2 = 0;
    }

    void Print(ostream& os) {
        StatsBaseVariable::PrintPaddedName(os,name);
	    double m = min(val1, val2);
	    if (m > 1e9) os << fixed << setprecision(3) << val1 / 1e9f << "G" << ":" << val2 / 1e9f << "G";
	    else if (m > 1e6) os << fixed << setprecision(3) << val1 / 1e6f << "M" << ":" << val2 / 1e6f << "M";
	    else if (m > 1e4) os << fixed << setprecision(3) << val1 / 1e3f << "K" << ":" << val2 / 1e3f << "K";
	    else os << fixed << setprecision(1) << val1 << " " << ":" << val2 << " ";
    }

    friend class StatsManager;
};

class StatsDistributionVariable : public StatsBaseVariable {
public:
    template<typename T>
    void Value(T vv) { double v = static_cast<double>(vv); sumv += v; count ++; minv = min(minv,v); maxv = max(maxv,v); }
protected:
    double count;
    double sumv;
    double minv;
    double maxv;

    StatsDistributionVariable(const string& c, const string& n) : StatsBaseVariable(c, n) {
        count = 0;
        sumv = 0;
        maxv = -DBL_MAX;
        minv = DBL_MAX;
    }

    virtual void Print(ostream& os) {
        StatsBaseVariable::PrintPaddedName(os,name);
        StatsBaseVariable::Print(os,minv); os << " | ";
        StatsBaseVariable::Print(os,sumv / count); os << " | ";
        StatsBaseVariable::Print(os,maxv);
    }

    friend class StatsManager;
};

class StatsTimerVariable : StatsBaseVariable {
public:
    void Start() { timer.Start(); }
    void Stop() { timer.Stop(); }

protected:
    Timer timer;

    StatsTimerVariable(const string& c, const string& n) : StatsBaseVariable(c, n) {
        timer.Reset();
    }

    virtual void Print(ostream& os) {
        StatsBaseVariable::PrintPaddedName(os,name);
        StatsBaseVariable::Print(os,timer.GetElapsedTime()/timer.GetCount());
    }

    friend class StatsManager;
};

class StatsManager {
public:
    template<class T>
    T* GetVariable(const string& category, const string& name) {
        pair<string, string> key = make_pair(category, name);
        if(varMap.find(key) == varMap.end()) {
            varMap[key] = new T(category, name);
        }
        return (T*)varMap[key];
    }

    // from PBRT
    void Print(ostream& os) {
        os << "Statistics" << endl;
        map< pair<string, string>, StatsBaseVariable* >::iterator iter = varMap.begin();
	    string lastCategory;
	    while (iter != varMap.end()) {
		    // Print statistic
            StatsBaseVariable* var = iter->second;
		    if (var->category != lastCategory) {
                os << var->category << endl;
			    lastCategory = var->category;
		    }
            var->Print(os);
            os << endl;
		    ++iter;
        }
    }

    void Cleanup() {
	    map< pair<string, string>, StatsBaseVariable* >::iterator iter = varMap.begin();
	    while (iter != varMap.end()) {
		    delete iter->second;
		    iter++;
	    }
    }

protected:
    map< pair<string, string>, StatsBaseVariable* > varMap;
};

#endif
