#ifndef FIBERPROCESSING_H
#define FIBERPROCESSING_H

// VTK INCLUDES
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDataArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>

// ITK INCLUDES
#include <itkSymmetricSecondRankTensor.h>
#include <itkGroupSpatialObject.h>
#include <itkExceptionObject.h>
#include <itkSpatialObjectReader.h>
#include <itkDTITubeSpatialObject.h>


// TYPEDEFS
typedef itk::GroupSpatialObject<3> GroupType;
typedef GroupType::ChildrenListType ChildrenListType;
typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;

/**
 *  Function readFiberFile:
 *  **********************
 *  Function which permits to read a fiber bundle from its filename and store in an itk::GroupSpatialObject<3>
 *  some informations as the coordinates, The FA, the RD, the MD etc... of each point for each fibers. So the output
 *  is a group of spatial objects, and each spatial object represents one fiber with its different features
 */
GroupType::Pointer readFiberFile(std::string filename); // Read the bunch of fibers at the location in parameter and return an array where each cell is a different fiber

/**
 *  Function writeFiberFile:
 *  ***********************
 *  Function which permits to write a file (.vtp or .vtk) for a itk::GroupSpatialObject<3> corresponding to a fiber
 *  bundle, with the filename thrown as input. This function is the opposite of the function readFiberFile()
 */
void writeFiberFile(const std::string &filename, GroupType::Pointer fibergroup); // write the bunch of fibers at the location in parameter return an array where each cell is a different fiber

#endif // FIBERPROCESSING_H
