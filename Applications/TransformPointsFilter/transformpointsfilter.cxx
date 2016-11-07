#include "transformpointsfilter.hxx"
#include "transformpointsfilterCLP.h"
using namespace std;

int main(int argc, char* argv[])
{
 	PARSE_ARGS;
 	if(argc < 1)
    {
        std::cerr << "Required: [Executable] -- input [inputfilename] --output [outputfilename] " << std::endl;
        return EXIT_FAILURE;
    }
 	vtkSmartPointer<TransformPointsFilter> needle = vtkSmartPointer<TransformPointsFilter>::New();
 	needle->SetInputFilename(inputPoints.c_str());
 	needle->SetTransform(tranformFilename.c_str());
 	needle->SetOutputFilename(outputPoints.c_str());
 	needle->Update();
	// needle->SetInput(inputFiber.c_str());
	// needle->SetOutput(outputFiber.c_str());
	// needle->Update();

 	return EXIT_SUCCESS;
}