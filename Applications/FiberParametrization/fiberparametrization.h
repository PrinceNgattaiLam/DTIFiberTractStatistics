#ifndef FIBERTRACTSAMPLING_H
#define FIBERTRACTSAMPLING_H

//VTK INCLUDES
#include <vtkPolyDataReader.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>

//ITK INCLUDES
#include <itkGroupSpatialObject.h>
#include <itkDTITubeSpatialObject.h>

//TYPEDEFS
typedef itk::GroupSpatialObject<3> GroupType;
typedef GroupType::ChildrenListType ChildrenListType;
typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;


/**
 *  Class FiberParametrization:
 *  **************************
 *  This class defines a type of object able to resample a fiberbundle, by different ways:
 *  1- Simple: At the end each fiber of the bundle has the same number of equidistants points, this number being an input
 *  2-
 */
class FiberParametrization : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
     vtkTypeMacro(FiberParametrization,vtkPolyDataAlgorithm);
     static FiberParametrization *New();

     /** Set & Get*/
     vtkSetMacro(nbSamples, int); // Set the number of samples for our sampling

    /** Main Functions */
    GroupType::Pointer GetOutput(); // Get the output data (data sampled)
    void SetInput(GroupType::Pointer input); // Set the input data (data to sample)

private:
    /** Internal Functions*/
    void sampling_unit(ChildrenListType::const_iterator it); // Sample one fiber with the the good number of samples and add this sampled fiber to the output

    /** Variables */
    GroupType::Pointer inputFibers; // Input - Data to sample
    GroupType::Pointer outputFibers; // Output - Data sampled
    int nbSamples; // Parameter of sampling

protected:

    /** Constructor & Destructor */
    FiberParametrization();
    ~FiberParametrization();

};

#endif // FIBERTRACTSAMPLING_H
