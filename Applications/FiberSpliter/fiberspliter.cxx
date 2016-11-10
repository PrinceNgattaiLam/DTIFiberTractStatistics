#include "../FiberFeaturesCreator/fiberfileIO.hxx"
#include "fiberspliter.hxx"
#include "fiberspliterCLP.h"
using namespace std;

int main(int argc, char* argv[])
{
 	PARSE_ARGS;
 	if(argc < 1)
    {
        std::cerr << "Required: [Executable] -- input [inputfilename] --output [outputfilename] " << std::endl;
        return EXIT_FAILURE;
    }
 	vtkSmartPointer<FiberSpliter> needle = vtkSmartPointer<FiberSpliter>::New();
	needle->SetInput(inputFiber.c_str());
	needle->SetOutput(outputFiber.c_str());
	needle->Update();

 	return EXIT_SUCCESS;
}