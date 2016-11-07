#ifndef TRANSFORMPOINTSFILTER_H
#define TRANSFORMPOINTSFILTER_H

#include <vtkSmartPointer.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
//#include <itkSystemTools.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataAlgorithm.h>
#include <iostream>
#include "itkFileTools.h"
#include "itkTransformFactoryBase.h"
#include "itkOptimizerParameters.h"
#include "itkTransformFileReader.h"
#include "../FiberFeaturesCreator/fiberfileIO.hxx"
#include <fstream>

/**
 * Filter that takes a fiber as input and reorientate it in order to have all the fibers in the same
 * sense. It cans be necessary to apply this filter twice to get a good result.
 */
//template<typename TParametersValueType>
class TransformPointsFilter: public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(TransformPointsFilter,vtkPolyDataAlgorithm);
    static TransformPointsFilter *New();
    /**
     * Set the inputFibers
     * @param input - Path to the input Fibers to read
     */
    void SetInputFilename(std::string input);

    void SetTransform(std::string t_filename);

    /**
     * Update the filter and process the input to get the output
     */
    void Update();

    /**
     * Set the outputfilename
     * @param output - output Fibers FIlename
     */
    void SetOutputFilename(std::string output);




private:

    /** Variables */
    vnl_matrix<float> m_X; // Input Points
    vnl_matrix<float> m_Y; // Output Points
    vnl_matrix<float> M; // Transform Matrix
    vnl_matrix<float> t; // Translation Matrix

    std::string inputfilename;
    std::string outputfilename;

    /** Internal Functions*/

    /**
     * Initialize ouputFibers
     */

    void read_fcsv_points();

    void write_fcsv_points();

    void read_vtk_points();

    void write_vtk_points();


protected:
    /** Constructor & Destructor */
    TransformPointsFilter();
    ~TransformPointsFilter();

};

#endif // TRANSFORMPOINTSFILTER_H