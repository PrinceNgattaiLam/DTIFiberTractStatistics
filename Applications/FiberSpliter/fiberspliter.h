#ifndef FIBERSPLITER_H
#define FIBERSPLITER_H

#include <vtkSmartPointer.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
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
class FiberSpliter : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberSpliter,vtkPolyDataAlgorithm);
    static FiberSpliter *New();
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
     * Update the filter and process the input to get the output
     */
    void Update();

    /**
     * Set the outputfilename
     * @param output - output Fibers FIlename
     */
    void SetOutput(std::string output);




private:

    /** Variables */
    vtkSmartPointer<vtkPolyData> inputFibers;
    std::string filename;
    std::string ext;
    std::string outputDir;

    /** Internal Functions*/

    /**
     * Initialize ouputFibers
     */
    void init_output();

    /**
     * Update the filter and process the input to get the output
     */
    void extract_fiber(int index);

protected:
    /** Constructor & Destructor */
    FiberSpliter();
    ~FiberSpliter();

};

#endif // FIBERSPLITER_H