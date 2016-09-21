
#include "fiberprocessing.h"
#define E(X) ((int)X)

SamplingFilter::SamplingFilter(int Nb, GroupType::Pointer input){
    this->SetInput(input);
    this->SetNbSamples(Nb);
    this->outputFibers=GroupType::New();
}


SamplingFilter::~SamplingFilter(){};

GroupType::Pointer SamplingFilter::GetOutput(){
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
        throw itk::ExceptionObject("Input Fibers void (null pointer)");
    }

}
void SamplingFilter::SetInput(GroupType::Pointer input){
    //TO DO: CHECK PATH
    this->inputFibers = input;

}
void SamplingFilter::SetNbSamples(int NbSamples){

    //TO DO: CHECK VALUE

    this->nbSamples = NbSamples;

}


void SamplingFilter::sampling_unit(ChildrenListType::const_iterator it){

    itk::SpatialObject<3>* tmp = (*it).GetPointer();
    itk::DTITubeSpatialObject<3>* tube = dynamic_cast<itk::DTITubeSpatialObject<3>* >(tmp);
    unsigned int nbPtOnFiber = tube->GetNumberOfPoints();
    //vtkIdType currentId = ids->GetNumberOfIds();
    itk::Point<double,3> p0, p1;
    float fa0, fa1, md0, md1, l10, l11, l20, l21, l30, l31, rd0, rd1;
    std::vector<DTIPointType> pts = tube->GetPoints();
    itk::Vector<double, 3> spacing(tube->GetSpacing());
    itk::Vector<double, 3> origin(tube->GetObjectToWorldTransform()->GetOffset());

    itk::DTITubeSpatialObject<3>::Pointer dtiTube = itk::DTITubeSpatialObject<3>::New();
    std::vector<DTIPointType> pointsToAdd;

    double step = (float) (nbPtOnFiber-1)/(this->nbSamples-1);
    for (unsigned int k = 0; k < this->nbSamples; k++)
    {
        DTIPointType newDtiPt;
        double new_fa, new_md, new_l1, new_l2, new_l3, new_rd;
        itk::Point<double,3> new_p;
        double t = k*step - E((k*step));

        if(E((k*step))<(nbPtOnFiber-1))
        {
            DTIPointType dtiPt0 = pts[E((k*step))];          DTIPointType dtiPt1 = pts[E((k*step))+1];
            p0   =   dtiPt0.GetPosition();                   p1  =   dtiPt1.GetPosition();
            fa0  =   dtiPt0.GetField("FA");                  fa1 =   dtiPt1.GetField("FA");
            md0  =   dtiPt0.GetField("MD");                  md1 =   dtiPt1.GetField("MD");
            l10  =   dtiPt0.GetField("l1");                  l11 =   dtiPt1.GetField("l1");
            l20  =   dtiPt0.GetField("l2");                  l21 =   dtiPt1.GetField("l2");
            l30  =   dtiPt0.GetField("l3");                  l31 =   dtiPt1.GetField("l3");
            rd0  =   dtiPt0.GetField("RD");                  rd1 =   dtiPt1.GetField("RD");

            new_p[0]    =   (1-t)*p0[0]  + t*p1[0];
            new_p[1]    =   (1-t)*p0[1]  + t*p1[1];
            new_p[2]    =   (1-t)*p0[2]  + t*p1[2];
            new_fa      =   (1-t)*fa0 + t*fa1;
            new_md      =   (1-t)*md0 + t*md1;
            new_l1      =   (1-t)*l10 + t*l11;
            new_l2      =   (1-t)*l20 + t*l21;
            new_l3      =   (1-t)*l30 + t*l31;
            new_rd      =   (1-t)*rd0 + t*rd1;

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
        pointsToAdd.push_back(newDtiPt);
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
    dtiTube->SetPoints(pointsToAdd);
    this->outputFibers->AddSpatialObject(dtiTube);
}

