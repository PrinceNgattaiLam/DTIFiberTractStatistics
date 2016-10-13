#ifndef FIBERFEATURESCREATOR_H
#define FIBERFEATURESCREATOR_H

#include <vtkSmartPointer.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkFloatArray.h>
//#include <vtkDataSet.h>
//#include <vtkDataSetAttributes.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <fstream>
#include "fiberfileIO.h"

class FiberFeaturesCreator : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberFeaturesCreator,vtkPolyDataAlgorithm);
    static FiberFeaturesCreator *New();

    void SetInput(std::string input, std::string model, std::string landmarksFile);
    void SetNbLandmarks(int nbLandmarks);
    void SetTorsionOn();
    void SetCurvatureOn();
    void SetLandmarksOn();
    void Update();
    vtkSmartPointer<vtkPolyData> GetOutput();


private:

    /** Variables */
    vtkSmartPointer<vtkPolyData> inputFibers;
    vtkSmartPointer<vtkPolyData> outputFibers;
    vtkSmartPointer<vtkPolyData> modelFibers;
    std::vector< vtkSmartPointer<vtkPoints> > landmarks;
    vtkSmartPointer<vtkPoints> avgLandmarks;
    int nbLandmarks;
    std::vector< vtkSmartPointer<vtkPoints> > curvatures;
    std::vector< vtkSmartPointer<vtkPoints> > torsions;
    std::string landmarksFilename;

    /** Methods states*/
    bool landmarkOn;
    bool torsionsOn;
    bool curvaturesOn;
    bool fcsvPointsOn;  // True - The Landmarks are defined by a .fcsv file
    			         // False - The Landmarks are directely computed on the model Fiber in founction of the number of fibers
    bool vtPointsOn;



    /** Internal Functions*/
    void compute_landmarks_from_model();	// Compute Landmarks Points;
    void compute_landmarks_from_fcsv();
    void compute_landmarks_from_vtk_vtp();
    void compute_landmarks_average();
    void compute_landmark_feature();	// Method landmark
    void compute_torsions_feature();	// Method Torsions;
    void compute_curvatures_feature();	// Method Curvature;
    void init_output();
    //void read_fcsv_file();
    void write_landmarks_file();


protected:
    /** Constructor & Destructor */
    FiberFeaturesCreator();
    ~FiberFeaturesCreator();

};
std::vector<int> find_landmarks_index(int nbSamples , int nbLandmarks);

#endif // FIBERFEATURESCREATOR_H