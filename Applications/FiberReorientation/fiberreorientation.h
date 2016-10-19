#ifndef FIBERREORIENTATION_H
#define FIBERREORIENTATION_H

#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataAlgorithm.h>
#include <iostream>
#include <fstream>
#include "fiberfileIO.hxx"

/**
 * Filter that takes a fiber as input and reorientate it in order to have all the fibers in the same
 * sense. It cans be necessary to apply this filter twice to get a good result.
 */
class FiberReorientation : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberReorientation,vtkPolyDataAlgorithm);
    static FiberReorientation *New();
    /**
     * Set the inputFibers
     * @param input - Path to the input Fibers to read
     */
    void SetInput(std::string input);

    /**
     * Set the inputFibers
     * @param input - input Fibers Data
     */
    void SetInputData(vtkSmartPointer<vtkPolyData> input);

    /**
     * Update the filter and process the output to get the output
     */
    void Update();
    /**
     * Return the output of the Filter
     * @return       output of the Filter
     */
    vtkSmartPointer<vtkPolyData> GetOutput();

    /**
     * Set at state the boolean hasReversed
     * @param state - state to set the boolean hasReversed
     */
    void SetHasReversed(bool state);

    /**
     * Return the state of hasReversed
     * @return - Current state of hasReversed
     */
    bool GetHasReversed();



private:

    /** Variables */
    vtkSmartPointer<vtkPolyData> inputFibers;
    vtkSmartPointer<vtkPolyData> outputFibers;
    vtkSmartPointer<vtkPoints> outputPts;
    vtkSmartPointer<vtkPoints> startPoints;
    vtkSmartPointer<vtkPoints> endPoints;
    bool hasReversed; /** True - When one fiber has been reversed
                          False - No Fibers has been reversed*/




    /** Internal Functions*/

    /**
     * Initialize ouputFibers
     */
    void init_output();

    /**
     * Initialize startPoints list of points
     */
    void init_startPoints();

    /**
     * Initialize endPoints list of points
     */
    void init_endPoints();

    /**
     * Compute the euclidian distance between the point pt and the next start point at index+1
     * @param  pt    - current point
     * @param  index - index of pt
     * @return       - distance between pt and start point at index+1
     */
    double compute_dist_next_startpoint(double* pt, int index);

    /**
     * Compute the euclidian distance between the point pt and the next end point at index+1
     * @param  pt    - current point
     * @param  index - index of pt
     * @return       - distance between pt and end point at index+1
     */
    double compute_dist_next_endpoint(double* pt, int index);



protected:
    /** Constructor & Destructor */
    FiberReorientation();
    ~FiberReorientation();

};

#endif // FIBERREORIENTATION_H