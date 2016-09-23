#include "FiberParametrizer.h"
#include "FiberParametrizer.hxx"
#include "fiberprocessing.h"
#include "vtkSmartPointer.h"
#include "FiberParametrizerCLP.h"
int main (int argc, char* argv[])
{
    PARSE_ARGS;

    if(argc < 3)
    {
        std::cerr << "Required: [Executable] -- inputfilename [inputfilename path] --outputfilename [outputfilename path] -N [Number of Samples]" << std::endl;
        return EXIT_FAILURE;
    }


    GroupType::Pointer test;
    test = readFiberFile(inputFiber);
    vtkSmartPointer<FiberParametrizer> Filter = vtkSmartPointer<FiberParametrizer>::New();
    Filter->SetInput(test);
    Filter->SetnbSamples(nbSample);
    
    writeFiberFile(outputFiber,Filter->GetOutput());

    // Read all the data from the file
     vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
     reader->SetFileName(outputFiber.c_str());
     reader->Update();

    vtkSmartPointer<vtkPolyData> linesPolyData = reader->GetOutput();

    std::cout << "There is " << linesPolyData->GetNumberOfLines() << " fibers." << std::endl;

    linesPolyData->GetLines()->InitTraversal();
    vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
   /* while(linesPolyData->GetLines()->GetNextCell(idList))
      {
      std::cout << "Line #"<<i<< " has " << idList->GetNumberOfIds() << " points." << std::endl;
      std::cout << std::endl;
      i++;
      }*/
    return EXIT_SUCCESS;

    return 0;
}
