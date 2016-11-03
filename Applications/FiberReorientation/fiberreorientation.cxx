#include "fiberfileIO.h"
#include "fiberreorientation.hxx"
#include "fiberreorientationCLP.h"
using namespace std;

int main(int argc, char* argv[])
{
 	PARSE_ARGS;
 	if(argc < 3)
    {
        std::cerr << "Required: [Executable] -- input [inputfilename] --output [outputfilename] " << std::endl;
        return EXIT_FAILURE;
    }
 	vtkSmartPointer<FiberReorientation> needle = vtkSmartPointer<FiberReorientation>::New();
	needle->SetInput(inputFiber.c_str());
	needle->Update();
	int i=0;
	while(needle->GetHasReversed() && i<20)
	{
		needle->SetHasReversed(false);
		needle->SetInputData(needle->GetOutput());
		needle->Update();
		i++;
	}
	writeVTKFile(outputFiber.c_str(),needle->GetOutput());

 	return EXIT_SUCCESS;
}