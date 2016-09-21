// #include "fiberparametrization.h"
// #include "fiberparametrization.hxx"
// #include "fiberprocessing.h"

// int main (int argc, char* argv[])
// {
//     PARSE_ARGS;
//     GroupType::Pointer test;
//     test = readFiberFile(argv[1]);
//     SamplingFilter Filter(50,test);
//     writeFiberFile("../Outputs/ExampleOutput1.vtp",Filter.GetOutput());

//     // Read all the data from the file
//      vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
//      reader->SetFileName("../Outputs/ExampleOutput1.vtp");
//      reader->Update();

//     vtkSmartPointer<vtkPolyData> linesPolyData = reader->GetOutput();

//     std::cout << "There are " << linesPolyData->GetNumberOfLines() << " lines." << std::endl;

//     linesPolyData->GetLines()->InitTraversal();
//     vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
//     int i=0;
//    /* while(linesPolyData->GetLines()->GetNextCell(idList))
//       {
//       std::cout << "Line #"<<i<< " has " << idList->GetNumberOfIds() << " points." << std::endl;
//       std::cout << std::endl;
//       i++;
//       }*/
//     return EXIT_SUCCESS;

//     return 0;
// }
