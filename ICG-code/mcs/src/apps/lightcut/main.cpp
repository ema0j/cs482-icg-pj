//////////////////////////////////////////////////////////////////////////
//!
//!	\file    main.cpp
//!	\date    3:3:2010   15:49
//!	\author  Jiawei Ou
//!	
//!	\brief   Matrix Column Sampling Main Entry
//!
//////////////////////////////////////////////////////////////////////////
#include <tclap/CmdLine.h>
using namespace TCLAP;

#include <scene/scene.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <image/image.h>
#include <imageio/imageio.h>
#include <scene/scenearchive.h>
#include <scene/camera.h>
#include <scene/background.h>
#include <ray/rayEngine.h>
#include <lightgen/LightGenerator.h>
#include <lighttree/DivisiveLightTreeBuilder.h>
#include <lightcutter/Lightcutter.h>
#include <lightcutter/MTLightcutter.h>
#include <lightgen/LightDiffuseGenerator.h>
#include <lightgen/LightSerializeGenerator.h>

string g_outfilename;

uint32_t _AverageCutSize( shared_ptr<Image<uint32_t> > cutImage ) 
{
    double totalCut = 0;
    uint64_t pixels = 0;
    for (uint32_t i = 0; i < cutImage->Size(); i++)
    {
        uint64_t c = cutImage->ElementAt(i);
        if (c > 0)
            pixels++;
    }

    for (uint32_t i = 0; i < cutImage->Size(); i++)
    {
        uint32_t c = cutImage->ElementAt(i);
        if (c > 0)
            totalCut += c / (double)pixels;
    }
    return static_cast<uint32_t>(totalCut);
}

int main(int argc, char** argv) {
    Timer timer;
    string filenameScene = "scene.xml";
    string filenameImage = "image.exr";
	string filenameLight;
    int width = 512; 
    int height = 512;

    bool outputCutImg = false;

    int direct = 1024;
    int indirect = 8192;

    int samples = 1;
    string builder;
    string cutter;

    bool silence = false;
    int depth = 1000;
    bool log = false;
    float error = 0.02f;

    CmdLine cmd("light cut: ", ' ', "none", false);
    try {
        SwitchArg helpArg("?", "help", "help message", cmd, false);
        SwitchArg silenceArg("x", "silence", "silence less output", cmd, false);
        ValueArg<int> widthArg("w", "width", "image width", false, width, "int", cmd);
        ValueArg<int> heightArg("h", "height", "image height", false, height, "int", cmd);

        UnlabeledValueArg<string> filenameSceneArg("scene", "scene filename", true, filenameScene, "string", cmd);
        UnlabeledValueArg<string> filenameImageArg("image", "image filename", true, filenameImage, "string", cmd);

		ValueArg<string> filenameLightArg("f", "lights", "virtual light filename", false, filenameLight, "string", cmd);

        ValueArg<int> indirectArg("i", "indirect", "indirect virtual light number", false, indirect, "int", cmd);
        ValueArg<int> samplesArg("s", "samples", "sample per pixels", false, samples, "int", cmd);
        ValueArg<int> detphArg("d", "depth", "cut depth", false, depth, "int", cmd);
        ValueArg<float> errorArg("e", "error", "error rate", false, error, "float", cmd);
        SwitchArg cugImgArg("c", "cutimage", "cut size image", cmd, false);

        SwitchArg logArg("l", "log", "write lot to file", cmd, log);

        string builderTypeStr [] = {"div", "dbg"};
        vector<string> builderTypes(builderTypeStr, builderTypeStr+2);
        builder = builderTypeStr[0];
        ValuesConstraint<string> builderTypesConstraint(builderTypes);
        ValueArg<string> builderArg("b", "builder", "light tree builder option", false, builder, &builderTypesConstraint, cmd);

        string cutterTypeStr [] = {"mt", "std"};
        vector<string> cutterTypes(cutterTypeStr, cutterTypeStr+2);
        cutter = cutterTypeStr[0];
        ValuesConstraint<string> cutterTypesConstraint(cutterTypes);
        ValueArg<string> cutterArg("t", "cutter", "lightcutter option", false, cutter, &cutterTypesConstraint, cmd);

        cmd.parse(argc, argv);

        if(helpArg.getValue()) StdOutput().usage(cmd);

        error = errorArg.getValue();
        log = logArg.getValue();
        filenameScene = filenameSceneArg.getValue();
        filenameImage = filenameImageArg.getValue();
		filenameLight = filenameLightArg.getValue();
        width = widthArg.getValue();
        height = heightArg.getValue();
        indirect = indirectArg.getValue();
        samples = samplesArg.getValue();
        outputCutImg = cugImgArg.getValue();
        builder = builderArg.getValue();
        cutter = cutterArg.getValue();
        silence = silenceArg.getValue();
        depth = detphArg.getValue();
    } catch(ArgException &e) {
        StdOutput().usage(cmd);
        cerr << "error: " << e.error() << endl << " for arg " << e.argId() << endl;
        return 1;
    }    

    g_outfilename = filenameImage;
    // reporting
    shared_ptr<ReportHandler> reportHandler;
    if (log)
        reportHandler = shared_ptr<ReportHandler>(new FileReportHandler(filenameImage + ".log.txt"));
    else
        reportHandler = shared_ptr<ReportHandler>(new PrintReportHandler());

    // load scene
    shared_ptr<Scene> scene;
    if(reportHandler) reportHandler->beginActivity("loading scene - " + filenameScene);
    SceneArchive::load(filenameScene, scene);
    if(reportHandler) reportHandler->endActivity();

    // build engine
    if(reportHandler) reportHandler->beginActivity("build ray engine");
    shared_ptr<RayEngine> rayEngine = 
        RayEngine::BuildDefault(scene->Surfaces(), scene->Instances(), 0.0f, 0);
    if(reportHandler) reportHandler->endActivity();

	shared_ptr<VirtualLightGenerator> generator;
	if (filenameLight.empty())
		generator = shared_ptr<VirtualLightGenerator>(new VirtualPointLightDiffuseGenerator(scene.get(), rayEngine.get()));
	else
		generator = shared_ptr<VirtualLightGenerator>(new LightSerializeGenerator(filenameLight));

    timer.Reset();
    timer.Start();
    LightTree* lightTree = DivisiveLightTreeBuilder(generator.get()).Build(scene.get(), rayEngine.get(), indirect, reportHandler.get());

    Image<Vec3f> image(width, height);
    shared_ptr<Image<uint32_t> > cutImage;
    if (outputCutImg)
        cutImage = shared_ptr<Image<uint32_t> >(new Image<uint32_t>(width, height));

    if (cutter == "std")
        Lightcutter(lightTree, scene.get(), rayEngine.get(), error, depth).Lightcut(&image, samples, cutImage.get(), reportHandler.get());
    else if (cutter == "mt")
        MTLightcutter(lightTree, scene.get(), rayEngine.get(), error, depth).Lightcut(&image, samples, cutImage.get(), reportHandler.get());
    timer.Stop();

    if (cutImage)
    {
        ImageIO::Save(filenameImage + "cs.exr", cutImage);
        uint32_t avgCutSize = _AverageCutSize(cutImage);
        stringstream sout;
        sout << "." << avgCutSize;
        ImageIO::Save(filenameImage + sout.str() + ".exr", image);
    }
    else
        ImageIO::Save(filenameImage + ".exr", image);

    stringstream sout;
    sout << "total Rendering time: " << timer.GetElapsedTime() << endl;
    if (reportHandler) reportHandler->message(sout.str());

	delete lightTree;

    // stats printing
    StatsManager stats;
    scene->CollectStats(stats);
    stats.Print(cout);

    // done
    return 0;
}
