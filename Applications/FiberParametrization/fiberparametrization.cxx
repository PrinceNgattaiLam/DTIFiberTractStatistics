#include "fiberparametrization.h"
#include "fiberparametrization.hxx"
#include "fiberprocessing.h"
#include "fiberparametrizationCLP.h"
int main (int argc, char* argv[])
{
    PARSE_ARGS;

    if(argc < 4)
    {
        std::cerr << "Required: [Executable] [inputfilename path] [outputfilename path]" << std::endl;
        return EXIT_FAILURE;
    }


    GroupType::Pointer test;
    test = readFiberFile(inputFiber);
    SamplingFilter Filter(nbSample,test);
    writeFiberFile(outputFiber,Filter.GetOutput());

    // Read all the data from the file
     vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
     reader->SetFileName(outputFiber.c_str());
     reader->Update();

    vtkSmartPointer<vtkPolyData> linesPolyData = reader->GetOutput();

    std::cout << "There is " << linesPolyData->GetNumberOfLines() << " lines." << std::endl;

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
