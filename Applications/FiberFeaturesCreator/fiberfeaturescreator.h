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

class FiberFeaturesCreator : public vtkPolyDataAlgorithm
{
public:
    /** Conventions for a VTK Class*/
    vtkTypeMacro(FiberFeaturesCreator,vtkPolyDataAlgorithm);
    static FiberFeaturesCreator *New();

    void SetInput(vtkSmartPointer<vtkPolyData> input);
    void SetNbLandmarks(int nbLandmarks);
    void Update();
    void WriteOutputFile(std::string filename);	// Method Curvature;

    vtkSmartPointer<vtkPolyData> GetOutput();


private:

    /** Variables */
    vtkSmartPointer<vtkPolyData> inputFibers;
    vtkSmartPointer<vtkPolyData> outputFibers;
    std::vector<std::vector<double*> > landmarks;
    int nbLandmarks;
    std::vector<std::vector<double*> > curvatures;
    std::vector<std::vector<double*> > torsions;

    /** Methods states*/
    bool arclengthOn;
    bool torsionsOn;
    bool curvaturesOn;



    /** Internal Functions*/
    void compute_landmarks();	// Compute Landmarks Points;
    void compute_arclength_feature();	// Method Arclength;
    void compute_torsions_feature();	// Method Torsions;
    void compute_curvatures_feature();	// Method Curvature;
    void update_methods_state();
    void init_output();

protected:
    /** Constructor & Destructor */
    FiberFeaturesCreator();
    ~FiberFeaturesCreator();

};

std::vector<int> find_landmarks_index(int nbSamples , int nbLandmarks);

#endif // FIBERFEATURESCREATOR_H