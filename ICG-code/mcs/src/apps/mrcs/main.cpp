
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
#include "lightgen/LightSerializeGenerator.h"

#include "MrcsCascade.h"
#include "MrcsLightgroup.h"

string g_DebugFileName;
#ifdef _DEBUG
#define SUBSAMPLE_COL 400
#else
#define SUBSAMPLE_COL 900
#endif // _DEBUG

//#define MRCS_CASCADE_CLUSTERING true
#define MRCS_LIGHTGROUP_CLUSTERING true

void WriteColumn(const vector<ScaledLight> &scaledLight, const string &filenameColumn );
int main(int argc, char** argv) {

	string filenameImage = "ourmethod-image";
	char buf[1000];

	Timer timer;
	string filenameScene = "boxArea.xml";
	string filenameLight;
	string filenameColumn;

	int width = 512;
	int height = 512;
	int indirect = 8192;
	int samples = 1;
	float errorRatio = 0.02f;
	int nclusters = 900;
	uint32_t nRow = 300;

	bool log = false;

	CmdLine cmd("matcut: ", ' ', "none", false);

	// reporting
	shared_ptr<ReportHandler> reportHandler;
	if (log)
		reportHandler = shared_ptr<ReportHandler>(new FileReportHandler(string(buf) + ".log.txt"));
	else
		reportHandler = shared_ptr<ReportHandler>(new PrintReportHandler());

	cout << "loading scene" << endl;
	// load scene
	if (reportHandler) reportHandler->beginActivity("loading scene - " + filenameScene);
	shared_ptr<Scene> scene = SceneArchive::load(filenameScene);
	if (reportHandler) reportHandler->endActivity();
	cout << "loading scene end" << endl;

	// build engine
	if (reportHandler) reportHandler->beginActivity("build ray engine");
	shared_ptr<RayEngine> engine = RayEngine::BuildDefault(scene->Surfaces(), scene->Instances(), 0.0f, 0);
	if (reportHandler) reportHandler->endActivity();

	shared_ptr<VirtualLightGenerator> generator;
	if (filenameLight.empty())
		generator = shared_ptr<VirtualLightGenerator>(new VirtualPointLightDiffuseGenerator(scene.get(), engine.get()));
	else
		generator = shared_ptr<VirtualLightGenerator>(new LightSerializeGenerator(filenameLight));

	Image<Vec3f> image(width, height);

#ifdef MRCS_CASCADE_CLUSTERING
	MrcsCascade renderer(generator.get(), scene.get(), engine.get());
#endif
#ifdef MRCS_LIGHTGROUP_CLUSTERING
	MrcsLightgroup renderer(generator.get(), scene.get(), engine.get());
#endif
	timer.Start();
	renderer.Render(indirect, &image, samples, nRow, nclusters, reportHandler.get());
	timer.Stop();

	if (!filenameColumn.empty())
		WriteColumn(renderer.ScaledLights(), filenameColumn);

	stringstream sout;
	sout << filenameImage << "." << nclusters << ".exr";
	ImageIO::Save(sout.str(), image);

	sout.clear();
	sout << "total Rendering time: " << timer.GetElapsedTime() << endl;
	if (reportHandler) reportHandler->message(sout.str());

	// stats printing
	StatsManager stats;
	scene->CollectStats(stats);
	stats.Print(cout);

	// done
    return 0;
}

void WriteColumn(const vector<ScaledLight> &scaledLights, const string &filenameColumn ) 
{
	FILE *f = fopen(filenameColumn.c_str(), "wb");
	if(f == 0) {
		fprintf(stderr, "error opening file");
		return;
	}

	fprintf(f, "%d\n", (uint32_t)scaledLights.size());
	for (uint32_t i = 0; i < scaledLights.size(); i++)
	{
		const ScaledLight &scaledLight = scaledLights[i];
		fprintf(f, "%d %lf %lf %lf\n", scaledLight.idx, scaledLight.weight.x, scaledLight.weight.y, scaledLight.weight.z);
	}
	fprintf(f, "\n");
}