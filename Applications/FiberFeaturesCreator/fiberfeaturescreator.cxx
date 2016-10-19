#include "fiberfeaturescreator.hxx"
#include "fiberfileIO.hxx"
#include "fiberfeaturescreatorCLP.h"

int main (int argc, char *argv[])
{
	PARSE_ARGS;


    vtkSmartPointer<FiberFeaturesCreator> Filter = vtkSmartPointer<FiberFeaturesCreator>::New();

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
    if(nbLandmarks<0)
    {
        std::cerr << "wrong value for the number of Landmarks, must be an integer greater or equal to 0" << std::endl;
        return EXIT_FAILURE;
    }

	


    if(!landmarkOn && !curvatureOn && !torsionOn)
    {
        std::cerr << "Need a Feature to compute" << std::endl;
        return EXIT_FAILURE;
    }
    if(landmarkOn)
    {
        Filter->SetLandmarksOn();
        if(landmarkFile.empty() && modelFiber.empty())
        {
            std::cerr << "Specify a Model Fiber File or a FCSV file to compute the landmarks" << std::endl;
            return EXIT_FAILURE;
        }
        else if(!landmarkFile.empty() && landmarkFile.rfind(".vtk")==std::string::npos && landmarkFile.rfind(".vtp")==std::string::npos && landmarkFile.rfind(".fcsv")==std::string::npos)
        {
            std::cerr << "Wrong File Format, must be a .fcsv, .vtk or .vtp file" << std::endl;
            return EXIT_FAILURE;
        }
    }
    if(curvatureOn)
        Filter->SetCurvatureOn();
    if(torsionOn)
        Filter->SetTorsionOn();

    Filter->SetInput(inputFiber.c_str(),modelFiber.c_str(),landmarkFile.c_str());
    Filter->SetNbLandmarks(nbLandmarks);

	Filter->Update();
    writeVTKFile(outputFiber.c_str(),Filter->GetOutput());

	return EXIT_SUCCESS;
}