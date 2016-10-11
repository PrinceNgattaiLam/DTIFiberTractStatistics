#include "fiberfeaturescreator.h"
#include "fiberfeaturescreator.hxx"
#include "fiberfeaturescreatorCLP.h"

int main (int argc, char *argv[])
{
	PARSE_ARGS;
	if(argc < 3)
    {
        std::cerr << "Required: [Executable] --inputfilename [inputfilename path] --outputfilename [outputfilename path] -N [Number of Landmarks]" << std::endl;
        return EXIT_FAILURE;
    }
    if(inputFiber.rfind(".vtp")==std::string::npos && inputFiber.rfind(".vtk")==std::string::npos)
    {
        std::cerr << "Wrong File Format, must be a .vtk or .vtp file" << std::endl;
        return EXIT_FAILURE;
    }

    if(modelFiber.rfind(".vtp")==std::string::npos && modelFiber.rfind(".vtk")==std::string::npos)
    {
        std::cerr << "Wrong File Format, must be a .vtk or .vtp file" << std::endl;
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


	vtkSmartPointer<vtkPolyDataReader> inputreader = vtkSmartPointer<vtkPolyDataReader>::New();
	inputreader->SetFileName(inputFiber.c_str());
	inputreader->Update();

	vtkSmartPointer<vtkPolyDataReader> modelreader = vtkSmartPointer<vtkPolyDataReader>::New();
	modelreader->SetFileName(modelFiber.c_str());
	modelreader->Update();

	vtkSmartPointer<FiberFeaturesCreator> test = vtkSmartPointer<FiberFeaturesCreator>::New();
	


    if(landmarkOn)
        test->SetLandmarksOn();
    if(curvatureOn)
        test->SetCurvatureOn();
    if(torsionOn)
        test->SetTorsionOn();
    if(landmarksOption=="average")
        test->SetAverageOn();

    test->SetInput(inputreader->GetOutput(),modelreader->GetOutput(),fcsvFile.c_str());
    test->SetNbLandmarks(nbLandmarks);


	test->Update();
	test->WriteOutputFile(outputFiber.c_str());

	return EXIT_SUCCESS;
}