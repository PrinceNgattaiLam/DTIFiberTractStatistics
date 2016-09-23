
#include "fiberprocessing.h"
#include "fiberparametrization.h"
#include "vtkObjectFactory.h"
#define E(X) ((int)X)  //We define the integer part of a variable X

vtkStandardNewMacro(FiberParametrization);


FiberParametrization::FiberParametrization(){
    this->SetnbSamples(0);
    this->outputFibers=GroupType::New();
    this->inputFibers=GroupType::New();
}


FiberParametrization::~FiberParametrization(){}

GroupType::Pointer FiberParametrization::GetOutput(){
    if(this->outputFibers.IsNotNull()) // CHECK LATER ! ! !
    {
        std::auto_ptr<ChildrenListType> children(this->inputFibers->GetChildren(0));
        typedef ChildrenListType::const_iterator IteratorType;
        for (IteratorType it = children->begin(); it != children->end(); it++)
        {
            this->sampling_unit(it);
        }
        return this->outputFibers;
    }
    else
    {
        throw itk::ExceptionObject("Output Fibers void (null pointer)");
    }

}
void FiberParametrization::SetInput(GroupType::Pointer input){
    //TO DO: CHECK INPUT

    if(this->inputFibers.IsNotNull())
    {
        this->inputFibers = input;
    }
    else
    {
        throw itk::ExceptionObject("Input Fibers void (null pointer)");
    }

}


void FiberParametrization::sampling_unit(ChildrenListType::const_iterator it){

    // We get the
    itk::SpatialObject<3>* tmp = (*it).GetPointer();
    itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);

    unsigned int nbPtOnFiber = tube->GetNumberOfPoints();

    itk::Point<double,3> p0, p1; // We create two points which will contain the coordinates of two consecutives points
    float fa0, fa1, md0, md1, l10, l11, l20, l21, l30, l31, rd0, rd1; // We create variables to store each diffusivity values for these two points
    std::vector<DTIPointType> pts = tube->GetPoints();
    itk::Vector<double, 3> spacing(tube->GetSpacing());
    itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());
    itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
    std::vector<DTIPointType> pointsToAdd;

    double step; // step will represent the distance between two points in our fiber sampled
    if(this->nbSamples > 1)
        step = (float) (nbPtOnFiber-1)/(this->nbSamples-1);
    else
        step = 0;
    for (unsigned int k = 0; k < this->nbSamples; k++)
    {
        DTIPointType newDtiPt;
        double new_fa, new_md, new_l1, new_l2, new_l3, new_rd;
        itk::Point<double,3> new_p;
        double t = k*step - E((k*step)); // t € [0,1] and is the step of interpolation

        if(E((k*step))<(nbPtOnFiber-1)) // If we are not at the end
        {
            /** We find the two points arround our new point using the integer part.
             * There is representation for what we do in this algorithm:
             *
             *                   E(k*step)   New Point   E(k*step)+1
             * --|-------------------|----------x---------|-------------------|--
             *                       <----------><-------->
             *                             t        1-t
             */
            DTIPointType dtiPt0 = pts[E((k*step))];          DTIPointType dtiPt1 = pts[E((k*step))+1];

            // We store the values for each variables created previously
            p0   =   dtiPt0.GetPosition();                   p1  =   dtiPt1.GetPosition();
            fa0  =   dtiPt0.GetField("FA");                  fa1 =   dtiPt1.GetField("FA");
            md0  =   dtiPt0.GetField("MD");                  md1 =   dtiPt1.GetField("MD");
            l10  =   dtiPt0.GetField("l1");                  l11 =   dtiPt1.GetField("l1");
            l20  =   dtiPt0.GetField("l2");                  l21 =   dtiPt1.GetField("l2");
            l30  =   dtiPt0.GetField("l3");                  l31 =   dtiPt1.GetField("l3");
            rd0  =   dtiPt0.GetField("RD");                  rd1 =   dtiPt1.GetField("RD");

            // We do a linear interpolation between the two consecutives points that arround our new point
            new_p[0]    =   (1-t)*p0[0]  + t*p1[0];
            new_p[1]    =   (1-t)*p0[1]  + t*p1[1];
            new_p[2]    =   (1-t)*p0[2]  + t*p1[2];
            new_fa      =   (1-t)*fa0 + t*fa1;
            new_md      =   (1-t)*md0 + t*md1;
            new_l1      =   (1-t)*l10 + t*l11;
            new_l2      =   (1-t)*l20 + t*l21;
            new_l3      =   (1-t)*l30 + t*l31;
            new_rd      =   (1-t)*rd0 + t*rd1;

            // We add the new computed values
            newDtiPt.SetPosition(new_p);
            newDtiPt.AddField("FA",new_fa);
            newDtiPt.AddField("MD",new_md);
            newDtiPt.AddField("l1",new_l1);
            newDtiPt.AddField("l2",new_l2);
            newDtiPt.AddField("l3",new_l3);
            newDtiPt.AddField("RD",new_rd);
        }
        else
        {
            newDtiPt = pts[E((k*step))];
        }
        pointsToAdd.push_back(newDtiPt); // We add the point to our vector
    }

// Code de TEST
//    float distance;
//    std::cout<<"Distance pour une fibre"<<std::endl;
//    for(int i=1; i<pointsToAdd.size(); i++)
//    {
//        DTIPointType dtiPt0_test = pointsToAdd[i-1];          DTIPointType dtiPt1_test = pointsToAdd[i];
//        itk::Point<double,3> p0_test = dtiPt0_test.GetPosition();
//        itk::Point<double,3> p1_test = dtiPt1_test.GetPosition();
//        float distance = sqrt((p0_test[0]-p1_test[0])*(p0_test[0]-p1_test[0])+(p0_test[1]-p1_test[1])*(p0_test[1]-p1_test[1])+(p0_test[2]-p1_test[2])*(p0_test[2]-p1_test[2]));
//        std::cout<<"distance = "<<distance<<std::endl;
//    }
    dtiTube->SetPoints(pointsToAdd); // We store all the points corresponding to one fiber sampled, in a spatial object
    this->outputFibers->AddSpatialObject(dtiTube);// And we add this spatial object to the Output Data
}

