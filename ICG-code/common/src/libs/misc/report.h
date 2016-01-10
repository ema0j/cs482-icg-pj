#ifndef _REPORT_H_
#define _REPORT_H_

#include <stdio.h>
#include <string>
#include <misc/timer.h>
#include <stdint.h>
#include <vector>
#include <assert.h>
#include <fstream>
#include <tbbutils/tbbtimer.h>
using namespace std;

class ReportHandler {
public:
    virtual void beginActivity(const string& activity) = 0;
    virtual void progress(float percent, int percentToPrint = 5) = 0;
    virtual void endActivity() = 0;
    virtual void message(const string& msg) = 0;
};

class FileReportHandler : public ReportHandler
{
public:
    FileReportHandler(const string &filename) : fout(filename.c_str()) {}
    virtual void beginActivity(const string& activity) 
    {
        currentActivity = activity;
        currentTimer.Reset();
        currentTimer.Start();
    }

    virtual void progress(float percent, int percentToPrint = 5) 
    {
    }

    virtual void endActivity() {
        currentTimer.Stop();
        fout << currentActivity << ": done            in " << currentTimer.GetElapsedTime() << " s" << endl;
        fout.flush();
    }

    virtual void message(const string& msg) {
        fout << msg << endl;
        fout.flush();
    }

protected:
    ofstream fout;
    string currentActivity;
    TbbTimer currentTimer;
};

class PrintReportHandler : public ReportHandler {
public:
    virtual void beginActivity(const string& activity) 
    {
        currentActivity = activity;
        printf("\r%s: in progress", currentActivity.c_str());
        currentTimer.Reset();
        currentTimer.Start();
        lastPrintedPercentRounded = -1;
    }

    virtual void progress(float percent, int percentToPrint = 5) 
    {
        currentPercentRounded = (int)(percent * 100 + 0.5);
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
    int currentPercentRounded;
    int lastPrintedPercentRounded;
    TbbTimer currentTimer;
};


#endif
