#ifndef FIBERREORIENTATION_H
#define FIBERREORIENTATION_H

#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>
#include <iostream>
#include <fstream>
#include "fiberfileIO.hxx"

class FiberReorientation : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberReorientation,vtkPolyDataAlgorithm);
    static FiberReorientation *New();

    void SetInput(std::string input);
    void SetInputData(vtkSmartPointer<vtkPolyData> input);

    void Update();
    vtkSmartPointer<vtkPolyData> GetOutput();



private:

    /** Variables */
    vtkSmartPointer<vtkPolyData> inputFibers;
    vtkSmartPointer<vtkPolyData> outputFibers;
    vtkSmartPointer<vtkPoints> outputPts;
    vtkSmartPointer<vtkPoints> startPoints;
    vtkSmartPointer<vtkPoints> endPoints;

    /** Internal Functions*/
    void init_output();
    void init_startPoints();
    void init_endPoints();
    double compute_dist_next_startpoint(double* pt, int index);
    double compute_dist_next_endpoint(double* pt, int index);



protected:
    /** Constructor & Destructor */
    FiberReorientation();
    ~FiberReorientation();

};

#endif // FIBERREORIENTATION_H