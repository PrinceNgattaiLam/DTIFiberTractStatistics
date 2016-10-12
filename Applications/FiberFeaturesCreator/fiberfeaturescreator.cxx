#include "fiberfeaturescreator.hxx"
#include "fiberfileIO.hxx"
#include "fiberfeaturescreatorCLP.h"

int main (int argc, char *argv[])
{
	PARSE_ARGS;
	if(argc < 5)
    {
        std::cerr << "Required: [Executable] --input [inputfilename] --output [outputfilename] --fcsv [FCSV File] --[Feature to compute]" << std::endl;
        return EXIT_FAILURE;
    }
    if(inputFiber.rfind(".vtp")==std::string::npos && inputFiber.rfind(".vtk")==std::string::npos || inputFiber.empty())
    {
        std::cerr << "Wrong Input File Format, must be a .vtk or .vtp file" << std::endl;
        return EXIT_FAILURE;
    }

    if(outputFiber.rfind(".vtp")==std::string::npos && outputFiber.rfind(".vtk")==std::string::npos || outputFiber.empty())
    {
        std::cerr << "Wrong Output File Format, must be a .vtk or .vtp file" << std::endl;
        return EXIT_FAILURE;
    }

    if(!modelFiber.empty() && modelFiber.rfind(".vtp")==std::string::npos && modelFiber.rfind(".vtk")==std::string::npos)
    {
        std::cerr << "Wrong Model File Format, must be a .vtk or .vtp file" << std::endl;
        return EXIT_FAILURE;
    }

    if(!fcsvFile.empty() && fcsvFile.rfind(".fcsv")==std::string::npos)
    {
        std::cerr << "Wrong File Format, must be a .fcsv file" << std::endl;
        return EXIT_FAILURE;
    }


    if(nbLandmarks<0)
    {
        std::cerr << "wrong value for the number of Landmarks, must be an integer greater or equal to 0" << std::endl;
        return EXIT_FAILURE;
    }

	vtkSmartPointer<FiberFeaturesCreator> test = vtkSmartPointer<FiberFeaturesCreator>::New();
	

    if(!landmarkOn && !curvatureOn && !torsionOn)
    {
        std::cerr << "Need a Feature to compute" << std::endl;
        return EXIT_FAILURE;
    }
    if(landmarkOn)
    {
        test->SetLandmarksOn();
        if(fcsvFile.empty()&&modelFiber.empty())
        {
            std::cerr << "Specify a Model Fiber File or a FCSV file to compute the landmarks" << std::endl;
            return EXIT_FAILURE;
        }
    }
    if(curvatureOn)
        test->SetCurvatureOn();
    if(torsionOn)
        test->SetTorsionOn();
    if(landmarksOption=="average")
        test->SetAverageOn();

    test->SetInput(readVTKFile(inputFiber.c_str()),readVTKFile(modelFiber.c_str()),fcsvFile.c_str());
    test->SetNbLandmarks(nbLandmarks);

	test->Update();
    writeVTKFile(outputFiber.c_str(),test->GetOutput());

	return EXIT_SUCCESS;
}