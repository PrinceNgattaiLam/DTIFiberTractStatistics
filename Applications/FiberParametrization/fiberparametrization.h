#ifndef FIBERTRACTSAMPLING_H
#define FIBERTRACTSAMPLING_H
#include <vtkPolyDataReader.h>
#include <itkGroupSpatialObject.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>
#include <itkDTITubeSpatialObject.h>

typedef itk::GroupSpatialObject<3> GroupType;
typedef GroupType::ChildrenListType ChildrenListType;
typedef itk::DTITubeSpatialObjectPoint<3> DTIPointType;

class FiberParametrization : public vtkPolyDataAlgorithm 
{
public:
    /** Conventions for a VTK Class*/
     vtkTypeMacro(FiberParametrization,vtkPolyDataAlgorithm);
     static FiberParametrization *New();

     /** Set & Get*/
     vtkSetMacro(nbSamples, int);


    /** Main Functions */
    GroupType::Pointer GetOutput();
    void SetInput(GroupType::Pointer input);
    //void SetNbSamples(int nbSamples);

private:
    /** Internal Functions*/
    void sampling_unit(ChildrenListType::const_iterator it); // Create a new fiber with the good number of samples

    /** Variables */
    GroupType::Pointer inputFibers;
    GroupType::Pointer outputFibers;
    int nbSamples;

protected:

    /** Constructor & Destructor */
    FiberParametrization();
    ~FiberParametrization();

};

#endif // FIBERTRACTSAMPLING_H
