#ifndef _REPORT_H_
#define _REPORT_H_

#include <stdio.h>
#include <string>
#include <misc/timer.h>
using namespace std;

class ReportHandler {
public:
    virtual void beginActivity(const string& activity) = 0;
    virtual void progress(float percent, int percentToPrint = 5) = 0;
    virtual void endActivity() = 0;
    virtual void message(const string& msg) = 0;
};

class PrintReportHandler : public ReportHandler {
public:
    virtual void beginActivity(const string& activity) {
        currentActivity = activity;
        printf("\r%s: in progress", currentActivity.c_str());
        currentTimer.Reset();
        currentTimer.Start();
        lastPrintedPercentRounded = -1;
        percents = 0;
    }

    virtual void progress(float percent, int percentToPrint = 5) {
        percents = percent;

        currentPercentRounded = (int)(percents * 100 + 0.5);
        if(currentPercentRounded % percentToPrint) return;
        if(currentPercentRounded == lastPrintedPercentRounded) return;

        printf("\r%s: %3d               ", currentActivity.c_str(), currentPercentRounded);
        fflush(stdout);
        
        lastPrintedPercentRounded = currentPercentRounded;
    }

    virtual void endActivity() {
        currentTimer.Stop();
        printf("\r%s: done            in %lf s\n", currentActivity.c_str(), currentTimer.GetElapsedTime());
        fflush(stdout);
    }

    virtual void message(const string& msg) {
        printf("%s\n", msg.c_str());
        fflush(stdout);
    }

protected:
    string currentActivity;
    float percents;
    int currentPercentRounded;
    int lastPrintedPercentRounded;
    Timer currentTimer;
};

#endif
