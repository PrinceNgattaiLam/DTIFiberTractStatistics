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

    if(nbLandmarks<0)
    {
        std::cerr << "wrong value for the number of Landmarks, must be an integer greater or equal to 0" << std::endl;
        return EXIT_FAILURE;
    }

	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->SetFileName(inputFiber.c_str());
	reader->Update();

	vtkSmartPointer<FiberFeaturesCreator> test = vtkSmartPointer<FiberFeaturesCreator>::New();

	test->SetInput(reader->GetOutput());
	test->SetNbLandmarks(nbLandmarks);
	test->Update();
	test->WriteOutputFile(outputFiber.c_str());

	return EXIT_SUCCESS;
}