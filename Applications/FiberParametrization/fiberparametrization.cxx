#include "fiberparametrization.h"
#include "fiberparametrization.hxx"
#include "fiberprocessing.h"
#include "vtkSmartPointer.h"
#include "fiberparametrizationCLP.h"
int main (int argc, char* argv[])
{
    PARSE_ARGS;

    if(argc < 3)
    {
        std::cerr << "Required: [Executable] -- input [inputfilename] --output [outputfilename] -N [Number of Samples - (30 by default)]" << std::endl;
        return EXIT_FAILURE;
    }

    GroupType::Pointer test;
    test = readFiberFile(inputFiber);
    vtkSmartPointer<FiberParametrization> Filter = vtkSmartPointer<FiberParametrization>::New();
    Filter->SetInput(test);
    Filter->SetNbSamples(nbSample);
    Filter->Update();
    writeFiberFile(outputFiber,Filter->GetOutput());

    return EXIT_SUCCESS;
}
