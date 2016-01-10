#include <tclap/CmdLine.h>
using namespace TCLAP;
#include <scene/scene.h>
#include <sampler/pathSampler.h>
#include <misc/report.h>
#include <image/image.h>
#include <imageio/imageio.h>
#include <scene/scenearchive.h>
#include <ray/rayEngine.h>
#include <lightgen/LightGenerator.h>
#include <lightgen/LightDiffuseGenerator.h>
#include <lightgen/LightSerializeGenerator.h>
#include <nmatrix/KnnMatrix.h>
#include <tbbutils/tbbutils.h>


uint64_t AverageSampleNum( const Image<uint32_t>* sampleImage ) 
{
    double totalCut = 0;
    uint64_t pixels = 0;
    for (uint32_t i = 0; i < sampleImage->Size(); i++)
    {
        uint64_t c = sampleImage->ElementAt(i);
        if (c > 0)
            pixels++;
    }

    for (uint32_t i = 0; i < sampleImage->Size(); i++)
    {
        uint64_t c = sampleImage->ElementAt(i);
        if (c > 0)
            totalCut += c / (double)pixels;
    }
    return static_cast<uint64_t>(totalCut);
}

int main(int argc, char** argv) {
    Timer timer;
    string filenameScene = "./boxArea.xml";
    string filenameImage = "./image.exr";
	string filenameLight;
    uint32_t width = 512; 
    uint32_t height = 512;
    uint32_t indirect = 8192;
    uint32_t samples = 1;
    uint32_t iterations = 4;
    uint32_t seedNum = 300;

	bool outputSample = false;
    uint32_t budget = 600;
    bool log = false;
    CmdLine cmd("comat: ", ' ', "none", false);
    try {
        SwitchArg helpArg("?", "help", "help message", cmd, false);
        ValueArg<int> widthArg("w", "width", "image width", false, width, "int", cmd);
        ValueArg<int> heightArg("h", "height", "image height", false, height, "int", cmd);

        UnlabeledValueArg<string> filenameSceneArg("scene", "scene filename", true, filenameScene, "string", cmd);
        UnlabeledValueArg<string> filenameImageArg("image", "image filename", true, filenameImage, "string", cmd);

		SwitchArg sampleImgArg("c", "sampleimage", "sample image", cmd, false);
		ValueArg<string> filenameLightArg("f", "lights", "virtual light filename", false, filenameLight, "string", cmd);
        ValueArg<int> indirectArg("i", "indirect", "indirect virtual light number", false, indirect, "int", cmd);
        ValueArg<int> samplesArg("s", "samples", "pixel row samples", false, samples, "int", cmd);
        ValueArg<int> seedArg("r", "seeds", "seedings", false, seedNum, "int", cmd);
        ValueArg<int> clusterArg("k", "clusters", "cluster number", false, budget, "int", cmd);
        SwitchArg logArg("l", "log", "write lot to file", cmd, log);

        //cmd.parse(argc, argv);

        //if(helpArg.getValue()) StdOutput().usage(cmd);

        log = logArg.getValue();
		outputSample = sampleImgArg.getValue();
        filenameScene = filenameSceneArg.getValue();
        filenameImage = filenameImageArg.getValue();
		filenameLight = filenameLightArg.getValue();
        seedNum = seedArg.getValue();
        budget = clusterArg.getValue();
        width = widthArg.getValue();
        height = heightArg.getValue();
        indirect = indirectArg.getValue();
        samples = samplesArg.getValue();
    } catch(ArgException &e) {
        StdOutput().usage(cmd);
        cerr << "error: " << e.error() << endl << " for arg " << e.argId() << endl;
        return 1;
    }    

    //task_scheduler_init init(task_scheduler_init::deferred);
    //init.initialize(1);

    // reporting
    shared_ptr<ReportHandler> reportHandler;
    if (log)
        reportHandler = shared_ptr<ReportHandler>(new FileReportHandler(filenameImage + ".log.txt"));
    else
        reportHandler = shared_ptr<ReportHandler>(new PrintReportHandler());

    // load scene
    if(reportHandler) reportHandler->beginActivity("loading scene - " + filenameScene);
    shared_ptr<Scene> scene = SceneArchive::load(filenameScene);
    if(reportHandler) reportHandler->endActivity();

    // build engine
    if(reportHandler) reportHandler->beginActivity("build ray engine");
    shared_ptr<RayEngine> engine = RayEngine::BuildDefault(scene->Surfaces(), scene->Instances(), 0.0f, 0);
    if(reportHandler) reportHandler->endActivity();

	shared_ptr<VirtualLightGenerator> generator;
	if (filenameLight.empty())
		generator = shared_ptr<VirtualLightGenerator>(new VirtualPointLightDiffuseGenerator(scene.get(), engine.get()));
	else
		generator = shared_ptr<VirtualLightGenerator>(new LightSerializeGenerator(filenameLight));

	shared_ptr<PathSampler> sampler = shared_ptr<PathSampler>(new StratifiedPathSamplerStd());

	Image<Vec3f> image(width, height);
	shared_ptr<Image<uint32_t> > sampleImage;
	if (outputSample)
		sampleImage = shared_ptr<Image<uint32_t> >(new Image<uint32_t>(width, height));

    KnnMatrix knnMat(scene.get(), engine.get(), generator.get(), reportHandler.get());

	timer.Reset();
	timer.Start();
    knnMat.Render(&image, sampleImage.get(), samples, indirect, seedNum, budget);
	timer.Stop();

	Image<Vec3f> gpImage(width, height);
	knnMat.RenderGatherGroup(&gpImage);
	ImageIO::Save(filenameImage + ".gpg.exr", gpImage);
	cout << "************************************************************" << endl;
    if (outputSample)
    {
        ImageIO::Save(filenameImage + "sample.exr", sampleImage);
        uint32_t avgSampleNum = (uint32_t)AverageSampleNum(sampleImage.get());
        stringstream sout;
        sout << "." << avgSampleNum;
        ImageIO::Save(filenameImage + sout.str() + ".exr", image);
    }
    else
    {
        ImageIO::Save(filenameImage + ".exr", image);
    }

    stringstream sout;
    sout << "total Rendering time: " << timer.GetElapsedTime() << endl;
    if(reportHandler) reportHandler->message(sout.str());

    // stats printing
    StatsManager stats;
    scene->CollectStats(stats);
    stats.Print(cout);


    // done
    return 0;
}

