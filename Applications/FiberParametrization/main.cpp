#include "samplingfilter.h"
#include "samplingfilter.hxx"
#include "fiberprocessing.h"

int main (int argc, char* argv[])
{
    GroupType::Pointer test;
    SamplingFilter Filter(200,"../Inputs/Arc_L_FT_bundle_clean_processed.vtp");
    write_fiber("../Outputs/ExampleOutput.vtp",Filter.GetOutput());

    // Read all the data from the file
     vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
     reader->SetFileName("../Outputs/ExampleOutput.vtp");
     reader->Update();

    vtkSmartPointer<vtkPolyData> linesPolyData = reader->GetOutput();

    std::cout << "There are " << linesPolyData->GetNumberOfLines() << " lines." << std::endl;

    linesPolyData->GetLines()->InitTraversal();
    vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
    int i=0;
    while(linesPolyData->GetLines()->GetNextCell(idList))
      {
      std::cout << "Line #"<<i<< " has " << idList->GetNumberOfIds() << " points." << std::endl;
      std::cout << std::endl;
      i++;
      }
    return EXIT_SUCCESS;

    return 0;
}
