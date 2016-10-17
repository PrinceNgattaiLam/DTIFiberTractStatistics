
#define NB 10
#include "fiberfileIO.h"
#include "deviationfunction.hxx"
#include "deviationfunction.h"
#include "fiberreorientation.hxx"
#include "fiberreorientationCLP.h"
using namespace std;

int main(int argc, char* argv[])
{
  PARSE_ARGS;

  vtkSmartPointer<FiberReorientation> needle = vtkSmartPointer<FiberReorientation>::New();
  needle->SetInput(inputFiber.c_str());
  needle->Update();
  needle->SetInputData(needle->GetOutput());
  needle->Update();
  writeVTKFile(outputFiber.c_str(),needle->GetOutput());

  return EXIT_SUCCESS;
}