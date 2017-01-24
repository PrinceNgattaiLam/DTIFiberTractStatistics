#ifndef FIBERFEATURESCREATOR_H
#define FIBERFEATURESCREATOR_H

#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkSimplePointsWriter.h>
#include <vtkCurvatures.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>
//#include <vtkTetra.h>
#include <string> 
#include <fstream>
#include <vtkFloatArray.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include "fiberfileIO.h"

/**
 * Filter that compute some features on a Fiber Bundle set in paramaters. The differents possible features are:
 * 1 - Landmarks:   We compute on each fiber of the input fiber bundle, the distance of each point to a number of
 *                  of landmarks. We can send the position of the landmarks in a .fcsv file or a .vtk/vtp file, 
 *                  or in giving a model fiber and a number of landmarks to automatically compute. In this case 
 *                  the filter produce in addition of the output fiber, a .fcsv file containing the landmarks computed.
 */
class FiberFeaturesCreator : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberFeaturesCreator,vtkPolyDataAlgorithm);
    static FiberFeaturesCreator *New();

    /**
     * Set the inputs data of the filter
     * @param input         - location of the input fiber file
     * @param model         - location of the model fiber file (optionnal - depends on the method)
     * @param landmarksFile - location of the file containing landmarks (optionnal - depends on the method)
     */
    void SetInput(std::string input, std::string model, std::string landmarksFile, std::string output);

    /**
     * Set Number of Landmarks to compute
     * @param nbLandmarks Number of landmarks to compute
     */
    void SetNbLandmarks(int nbLandmarks);

    /**
     * Set torsionsOn at true
     */
    void SetTorsionOn();

    /**
     * Set curvatureOn at true
     */
    void SetCurvatureOn();

    /**
     * Set landmarksOn at true
     */
    void SetLandmarksOn();

    /**
     * Update the filter and process the output to get the output
     */
    void Update();

    /**
     * Return the output of the Filter
     * @return       output of the Filter
     */
    vtkSmartPointer<vtkPolyData> GetOutput();


private:

    /** Variables */
    int nbLandmarks;
    
    vtkSmartPointer<vtkPolyData> inputFibers;
    vtkSmartPointer<vtkPolyData> outputFibers;
    vtkSmartPointer<vtkPolyData> modelFibers;   //modelFibers that requires a scalar whose the name is "SamplingDistance2Origin"

    vtkSmartPointer<vtkPoints> avgLandmarks;
    std::vector< vtkSmartPointer<vtkPoints> > landmarks;
    std::vector< vnl_vector_fixed<double,3> > tangents;     //Tangents on each point of the fiber
    std::vector< vnl_vector_fixed<double,3> > binormals;    //binormals on each point of the fiber
    std::vector< vnl_vector_fixed<double,3> > normals;      //normals on each point of the fiber
    std::vector< double > ds; // Unity distance between each point of the fiber
    //std::vector< double* > tangents;
    //std::vector< vtkSmartPointer<vtkPoints> > curvatures;
    //std::vector< vtkSmartPointer<vtkPoints> > torsions;
    std::string landmarksFilename;

    /** Methods states*/
    bool landmarkOn;
    bool torsionsOn;
    bool curvaturesOn;
    bool fcsvPointsOn;   // True  - The Landmarks are defined by a .fcsv file
    			         // False - The Landmarks are directely computed on the model Fiber in founction of the number of fibers
    bool vtPointsOn;



    /** Internal Functions*/

    /**
     * Compute Landmarks from model
     */
    void compute_landmarks_from_model();

    /**
     * Compute Landmarks from fcsv file
     */
    void compute_landmarks_from_fcsv();

    /**
     * Compute Landmarks from .vtp/vtk file
     */
    void compute_landmarks_from_vtk_vtp();

    /**
     * Compute the average Landmarks
     */
    void compute_landmarks_average();

    /**
     * Compute the landmark feature on the fiber
     */
    void compute_landmark_feature();

    /**
     * Compute the tangents on each point of the fiber, necessary for the Torsions and Curvatures method
     */
    void compute_tangents_binormals_normals();


    void compute_torsions_feature();	// Method Torsions;
    void compute_curvatures_feature();	// Method Curvature;
    
    /**
     * Initialize outputFibers
     */
    void init_output();

    /**
     * Write the .fcsv file containing the landmarks
     */
    void write_landmarks_file();


protected:
    /** Constructor & Destructor */
    FiberFeaturesCreator();
    ~FiberFeaturesCreator();

};
std::vector<int> find_landmarks_index(int nbSamples , int nbLandmarks);

#endif // FIBERFEATURESCREATOR_H