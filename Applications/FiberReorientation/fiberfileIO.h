#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <itkMacro.h> //For itkEception

vtkSmartPointer<vtkPolyData> readVTKFile (std::string filename);

void writeVTKFile (std::string filename, vtkSmartPointer<vtkPolyData> output);