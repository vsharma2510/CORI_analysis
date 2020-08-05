#include <iostream>
#include <fstream>
#include <getopt.h>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include <TH2.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TString.h>
#include <TFile.h>
#include <TLegend.h>


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

using namespace std;
using namespace Cuore;

int main(int argc, char* argv[]){

  int tower,dataset;
  QChain ch("qtree");
  TString cut1,dir,listname,outputfile;
  std::vector<int> channel;
  std::vector<int>::iterator it;



  ifstream listfile(argv[1]);
  if(!listfile){
    cout<<"----Unable to open list file----"<<endl;
  }

  while(listfile.good()){
    listfile>>x;
    channel.push_back(x);
  }

  dataset=channel[0];

  for(int i=1;i<4;i++){
    tower=i;
    dir="/global/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/spring2019_reprocessing/output";
    listname.Form("%s/ds%d/calibration_Production_ds%d_tower%.3d.list",dir.Data(),dataset,dataset,tower);

    cout<<"#### Adding file "<<listname.Data()<<" ####"<<endl;
    int nAdded=ch.Add(listname.Data());
    if(nAdded==0){
      cout<<"----Could not add file to chain----"<<endl;
    }
  }

  for(it=channel.begin()+1;it!=channel.end();it++){
    cout<<"#### Begin for channel "<<*it<<" #### "<<endl;
    cut1.Form("IsStabPulser && Channel==%d", *it);
    unsigned int sizeArr = ch.Draw("StabAmplitude_heaterTGS:Time",cut1.Data(),"GOFF");
    double* stabAmp = ch.GetV1();
    double* StabAmp = new double [sizeArr];
    double* time = ch.GetV2();
    double* Time = new double [sizeArr];
    
    

