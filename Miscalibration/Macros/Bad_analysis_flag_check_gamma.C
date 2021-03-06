#include <iostream>
#include <fstream>
#include <getopt.h>
#include <vector>
#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <cstdlib>

#include <TH2.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TLegend.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TPad.h>

#include "QCuore.hh"
#include "QError.hh"
#include "QObject.hh"
#include "QGlobalHandle.hh"
#include "QMessage.hh"
#include "QGlobalDataManager.hh"
#include "QDetChannelCollectionHandle.hh"
#include "QDetChannelCollection.hh"
#include "QHeader.hh"
#include "QPulseInfo.hh"
#include "QChain.hh"
#include "QTObject.hh"
#include "QTObjectHandle.hh"
#include "QCalibrationParametersHandle.hh"
#include "QCalibrationResiduals.hh"


using namespace std;
using namespace Cuore;

int main(int argc, char* argv[]){

  int x,tower;
  std::vector<int> channel;
  std::vector<int>::iterator it;
  ifstream listfile(argv[1]);
  int run_number=atoi(argv[2]);
  if(!listfile){
    cout<<"!!!!!! Channel list file not found !!!!!!"<<endl;
  }

  /*----- Obtain list of miscalibrated channels -----*/
  string file_line;
  while(getline(listfile,file_line)){
    stringstream ss(file_line);
    ss>>x;
    cout<<"----- channel being added is -----"<<x<<endl;
    channel.push_back(x);
  }
  
  int dataset = channel[0]; //first element of channel[] is dataset

  /*----- Setting up output text file -----*/
  ofstream output_text_file;
  TString text_file_name = Form("/global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/Bad_for_Analysis/ds%d/ds%d_bad_for_analysis_channels_gamma.txt",dataset,dataset);
  output_text_file.open(text_file_name);


   for(it=channel.begin()+1;it!=channel.end();it++){
     cout<<"~~~~~~~~~~ begin for channel "<<*it<<"~~~~~~~~~~ "<<endl;
     if(*it%52==0){tower=(*it)/52;}  
     else{tower=((*it)/52+1);}
     cout<<"Tower being worked on is "<<tower<<endl;
     TString dir = Form("/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/spring2019_reprocessing/output/ds%d/Blinded_%d_%.3d_R.list",dataset,run_number,tower);
     cout<<"File being added is "<<dir.Data()<<endl;
     QChain ch("qtree");
     int nAdded = ch.Add(dir.Data());
     if (nAdded==0){
       cout<<"Could not add file to chain"<<endl;
     }
     TString cut = Form("Channel==%d",(*it));
     unsigned int sizeArr_H = ch.Draw("BadForAnalysis_heaterTGS@Passed.fValue",cut.Data(),"GOFF");

     double* heaterTGS_flag = ch.GetV1();
     double* heaterTGS_Flag = new double [sizeArr_H];

     for(int i=0;i<sizeArr_H;i++){
       heaterTGS_Flag[i] = heaterTGS_flag[i];
     }

     unsigned int sizeArr_C = ch.Draw("BadForAnalysis_calibrationTGS@Passed.fValue",cut.Data(),"GOFF");
      
     double* calibrationTGS_flag = ch.GetV1();
     double* calibrationTGS_Flag = new double [sizeArr_C];

     for(int i=0;i<sizeArr_H;i++){
       calibrationTGS_Flag[i] = calibrationTGS_flag[i];
     }

     if(heaterTGS_Flag[0]==0.0 || calibrationTGS_Flag[0]==0.0){
       //cout<<"For channel "<<(*it)<<" "<<"heaterTGS flag is "<<heaterTGS_Flag[0]<<" "<<"calibrationTGS flag is "<<calibrationTGS_Flag[0]<<endl;
       TString flag_string = Form("For channel %d \t heaterTGS flag is %d \t calibrationTGS flag is %d\n",(*it),(int)heaterTGS_Flag[0],(int)calibrationTGS_Flag[0]);
       output_text_file<<flag_string;
     }
   }
   cout<<"Output saved to "<<text_file_name<<endl;
   output_text_file.close();
   return 0;
}
