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

  Cuore::QGlobalDataManager dm;
  dm.EnableCache(false);
  int x,tower;
  double fit_par=1.;
  const int num_peaks=9;

  std::vector<int> channel;
  std::vector<int>::iterator it;
  TString dir = "/global/homes/v/vsharma2/Test_Diana/calib";
  int peak_list [num_peaks] = {239,338,583,511,911,969,1173,1333,2615};

  TGraphErrors* Res_gr= NULL;
  TF1* line = NULL;
  TF1* fit = NULL;
  TGraphErrors* calgraph = NULL;
  TGraph* gr = NULL;
  TGraphAsymmErrors* gr2 = NULL;
  TH1D* hSpectrum = NULL;
  double* Energy_arr = NULL;
  double* Res_arr = NULL;
  double* Res_error_arr = NULL;

  TCanvas* myCan = NULL;
  
  ifstream listfile(argv[1]);
  if(!listfile){
    cout<<"Unable to open channel list file"<<endl;
  }

  /* ---------- Obtain list of miscalibrated channels ----------*/
  string file_line;
  while(getline(listfile,file_line)){
    stringstream ss(file_line);
    ss>>x;
    cout<<"channel being added is "<<x<<endl;
    channel.push_back(x);
  }

  int dataset = channel[0]; //first element of channel[] is dataset

  /*---------- Looping over miscalibrated channels ----------*/
  
  for(it=channel.begin()+1;it!=channel.end();it++){
    cout<<"~~~~~~~~~~begin for channel "<<*it<<"~~~~~~~~~~ "<<endl;
    myCan = new TCanvas("can","can",2400,2400);
    myCan->Divide(2,2,0.00001,0.01);

    if(*it%52==0){tower=(*it)/52;}  
    else{tower=((*it)/52+1);}
    cout<<"Tower being worked on is "<<tower<<endl;

    TString debug_file_name = Form("%s/ds%d/wOF/debug/cal_debug_ds%d_tower%.3d.root",dir.Data(),dataset,dataset,tower);
    TString cal_file_name = Form("%s/ds%d/wOF/cal_coeffs_ds%d_tower%.3d.txt",dir.Data(),dataset,dataset,tower);

    cout<<debug_file_name<<endl;
    cout<<cal_file_name<<endl;

    unsigned int arr_size=0;
    std::vector<double> Energy;
    std::vector<double> Res;
    std::vector<double> Res_error;

    /*---------- Plotting Residuals ----------*/
    
    QCalibrationParametersHandle paramH((*it),dataset,"CalibrationParameters");
    dm.Get("CalCoefficients_heaterTGS",&paramH,cal_file_name.Data());
    myCan->cd(3);
    if(paramH.IsValid()){
      const QCalibrationParameters& params=paramH.Get();
      
      /* ---------- Looping over peaks ----------*/
      for(int iR=0;iR<params.fResiduals.size();iR++){
	const QCalibrationResiduals& aRes=params.GetCalibrationResidual(iR);
	Energy.push_back(aRes.PeakEnergy);
	Res.push_back(aRes.Residual);
	Res_error.push_back(aRes.ErrorResidual);
	cout<<"PeakEnergy   "<<aRes.PeakEnergy<<"   Residual   "<<aRes.Residual<<"   Residual Error   "<<aRes.ErrorResidual<<endl;
	cout<<"Success"<<endl;
      }
      
      arr_size = Energy.size();
      //cout<<arr_size<<endl;
      Energy_arr = new double [arr_size];
      Res_arr = new double [arr_size];
      Res_error_arr = new double [arr_size];
      
      for(int i=0;i<arr_size;i++){
	Energy_arr[i]=Energy[i];
	Res_arr[i]=Res[i];
	Res_error_arr[i]=Res_error[i];
	//cout<<Energy_arr[i]<<"   "<<Res_arr[i]<<"   "<<Res_error_arr[i]<<endl;
      }
      
      Res_gr = new TGraphErrors(arr_size,Energy_arr,Res_arr,0,Res_error_arr);
      line = new TF1("line","[0]*x",0,3000);
      
      line->SetParameter(0,0);
      line->SetLineColor(kRed);
      Res_gr->SetMarkerStyle(3);
      Res_gr->SetMarkerSize(3.0);
      Res_gr->GetXaxis()->SetTitle("Energy of peaks[keV]");
      Res_gr->GetYaxis()->SetTitle("Fixed Energy - True Energy");
      Res_gr->SetTitle(Form("Residuals for ch%d",*it));
      Res_gr->Draw("AP");
      line->Draw("SAME");

      /*delete[] Energy_arr;
      delete[] Res_arr;
      delete[] Res_error_arr;*/
    }
    else{cout<<"Something went wrong with parameter handle"<<endl;}

    /* ---------- Plotting full spectrum ----------*/
    /* myCan->cd(1);
    QTObjectHandle<TH1D> spectHandle("spectrum");
    spectHandle.SetChannel(*it);
    dm.Get("CalCoefficients_heaterTGS",&spectHandle,debug_file_name.Data());
    if(spectHandle.IsValid()){
      Cuore::QTObject<TH1D> h = spectHandle.Get();
      TH1D* hSpectrum = (TH1D*)(h.fObject.Clone(Form("clone_fullSpectrum%d",*it)));
      
      //myCan->cd(1);
      hSpectrum->SetTitle("Full stabilized amplitude sepctrum");
      hSpectrum->SetXTitle("Stabilized Amplitude");
      hSpectrum->SetYTitle("Counts");
      hSpectrum->Draw();
      //hSpectrum->Write();
    }
    else{cout<<"Something went wrong with spectHandle"<<endl;}*/

    /*---------- Plotting calibration function ----------*/
    myCan->cd(4);
    QTObjectHandle<TF1> fitFunHandle("CalFunction");
    fitFunHandle.SetChannel(*it);
    dm.Get("CalCoefficients_heaterTGS",&fitFunHandle,debug_file_name.Data());
    if(fitFunHandle.IsValid()){
      Cuore::QTObject<TF1> qfit = fitFunHandle.Get();
      fit = (TF1*)(qfit.fObject.Clone(Form("clone_fit%d",*it)));
      fit_par = fit->GetParameter(0);
      fit->SetTitle(Form("Calibration fit function for ch %d;Stabilized Amplitude;Energy[keV]",*it));
      fit->GetYaxis()->SetTitleOffset(1.45);
      fit->Draw();
    }
    else{cout<<"Something went wrong with fitFunHandle"<<endl;}
    
    QTObjectHandle<TGraphErrors> cgHandle("CalGraph");
    cgHandle.SetChannel((*it));
    dm.Get("CalCoefficients_heaterTGS",&cgHandle,debug_file_name.Data());
    if(cgHandle.IsValid()){
      Cuore::QTObject<TGraphErrors> qCalGraph;
      qCalGraph = cgHandle.Get();
      calgraph = (TGraphErrors*)(qCalGraph.fObject.Clone(Form("clone_calgraph%d",(*it))));
      //gr->GetXaxis()->SetTitle("Stab_Amp");
      calgraph->SetMarkerStyle(2);
      calgraph->SetMarkerSize(4);
      calgraph->Draw("P");
    }
    else{cout<<"Something went wrong with cgHandle"<<endl;}

    //myCan->cd(2);
    int temp_energy;
    for(int i=0;i<arr_size;i++){
      myCan->cd(2);
      temp_energy=0;
      for(int j=0;j<num_peaks;j++){
	if(abs(Energy[i]-peak_list[j])<20){temp_energy=peak_list[j];}
      }
      //if(temp_energy==2615){continue;}
      /*---------- Plotting peak amplitude spectrum ----------*/

      /*QTObjectHandle<TGraph> pHandle(Form("p%d",temp_energy));
      pHandle.SetChannel((*it));
      dm.Get("CalCoefficients_heaterTGS",&pHandle,debug_file_name.Data());
      if(pHandle.IsValid()){
	Cuore::QTObject<TGraph> gPeak;
	gPeak = pHandle.Get();
	//gPeak.fObject.Draw("AL");
	gr = (TGraph*)(gPeak.fObject.Clone(Form("clone_%d",temp_energy)));
	gr->GetXaxis()->SetTitle("Stabilized Amplitude");
	gr->GetYaxis()->SetTitle("Counts");
	gr->SetTitle(Form("Amplitude spectrum for peak of energy %d with fit",temp_energy));
	gr->Draw("AL");
	//gPeak.Clear();
      }
      else{cout<<"Something went wrong with peak Handle"<<endl;}*/
      
      QTObjectHandle<TGraphAsymmErrors> pHistHandle(Form("p%d_hist",temp_energy));
      //cout<<"pHistHandle uses "<<temp_energy<<endl;
      pHistHandle.SetChannel((*it));
      cout<<"pHistHandle contains ----- "<<pHistHandle.GetName()<<" with label "<<pHistHandle.GetLabel().GetStringLabel()<<endl;
      dm.Get("CalCoefficients_heaterTGS",&pHistHandle,debug_file_name.Data());
      if(pHistHandle.IsValid()){
	cout<<"~~~~~~~~~ Begin for "<<temp_energy<<" ~~~~~~~~~~"<<endl;
	Cuore::QTObject<TGraphAsymmErrors> gAE;
	gAE = pHistHandle.Get();
	//gAE.fObject->Draw();
	gr2=(TGraphAsymmErrors*)(gAE.fObject.Clone(Form("clone_hist%d",temp_energy)));
	//TGraphAsymmErrors gr3=gAE.fObject;
	//int n = gr3.GetN();
	//cout<<"~~~~~number of points in gr3 are "<<n<<"~~~~~"<<endl;
	gr2->GetXaxis()->SetTitle("Stabilized Amplitude");
	gr2->GetYaxis()->SetTitle("Counts");
	gr2->SetTitle(Form("Fit of peak for energy %d keV",temp_energy));
	gr2->SetMarkerSize(2.5);
	gr2->Draw("AP");
	//gAE.Clear();
      }
      else{cout<<"Something went wrong with pHistHandle"<<endl;}
      
      QTObjectHandle<TGraph> pHandle(Form("p%d",temp_energy));
      pHandle.SetChannel((*it));
      dm.Get("CalCoefficients_heaterTGS",&pHandle,debug_file_name.Data());
      if(pHandle.IsValid()){
	Cuore::QTObject<TGraph> gPeak;
	gPeak = pHandle.Get();
	//gPeak.fObject.Draw("AL");
	gr = (TGraph*)(gPeak.fObject.Clone(Form("clone_%d",temp_energy)));
	gr->GetXaxis()->SetTitle("Stabilized Amplitude");
	gr->GetYaxis()->SetTitle("Counts");
	gr->SetTitle(Form("Fit of peak for energy %d keV",temp_energy));
	gr->Draw("SAME");
	gPeak.Clear();
      }
      else{cout<<"Something went wrong with peak Handle"<<endl;}
      
      /* ---------- Plotting full spectrum ----------*/
      myCan->cd(1);
      QTObjectHandle<TH1D> spectHandle("spectrum");
      spectHandle.SetChannel(*it);
      dm.Get("CalCoefficients_heaterTGS",&spectHandle,debug_file_name.Data());
      if(spectHandle.IsValid()){
	Cuore::QTObject<TH1D> h = spectHandle.Get();
	hSpectrum = (TH1D*)(h.fObject.Clone(Form("clone_fullSpectrum%d",*it)));
	hSpectrum->SetTitle(Form("Stabilized amplitude spectrum for ds%d ch %d",dataset,*it));
	hSpectrum->SetXTitle("Stabilized Amplitude");
	hSpectrum->SetYTitle("Counts");
	hSpectrum->Draw();
	double range_lower = (temp_energy/fit_par)-200;
	double range_upper = (temp_energy/fit_par)+200;
	hSpectrum->GetXaxis()->SetRangeUser(range_lower,range_upper);
	cout<<"spectHandle uses "<<temp_energy<<" with histogram range of "<<range_lower<<" "<<range_upper<<endl;
      }
      else{cout<<"Something went wrong with spectHandle"<<endl;}
      
      
      TString output_file_name;
      output_file_name = Form("/global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/Cal_plots_images/ds%d/Alpha/cal_plot_ds%d_ch%d_%d_keV.pdf",dataset,dataset,(*it),temp_energy);
      myCan->Update();
      TString canvas_title;
      canvas_title = Form("Calibration plots for ds%d ch%d",dataset,*it);
      myCan->SetTitle(canvas_title);
      myCan->SaveAs(output_file_name);
      //myCan->Clear();
      //if(Res_gr){delete Res_gr;Res_gr=NULL;cout<<"Res_gr deleted"<<endl;}
      //if(line){delete line;line=NULL;cout<<"line deleted"<<endl;}
      //if(line){delete line;cout<<"line deleted 2"<<endl;}
      //if(fit){delete fit;fit=NULL;cout<<"fit deleted"<<endl;}
      //if(calgraph){delete calgraph;calgraph=NULL;cout<<"calgraph deleted"<<endl;}
      if(gr){delete gr;gr=NULL;cout<<"gr deleted"<<endl;}
      if(gr2){delete gr2;gr2=NULL;cout<<"gr2 deleted"<<endl;}
      if(hSpectrum){delete hSpectrum;hSpectrum=NULL;cout<<"hSpectrum deleted"<<endl;}
      //if(Energy_arr){delete[] Energy_arr;Energy_arr=NULL;cout<<"Energy_arr deleted"<<endl;}
      //if(Res_arr){delete[] Res_arr;Res_arr=NULL;cout<<"Res_arr deleted"<<endl;}
      //if(Res_error_arr){delete[] Res_error_arr;Res_error_arr=NULL;cout<<"Res_error_arr deleted"<<endl;}
    }
    if(Res_gr){delete Res_gr;Res_gr=NULL;cout<<"Res_gr deleted"<<endl;}
    if(line){delete line;line=NULL;cout<<"line deleted"<<endl;}
    if(line){delete line;cout<<"line deleted 2"<<endl;}
    if(fit){delete fit;fit=NULL;cout<<"fit deleted"<<endl;}
    if(calgraph){delete calgraph;calgraph=NULL;cout<<"calgraph deleted"<<endl;}
    if(Energy_arr){delete[] Energy_arr;Energy_arr=NULL;cout<<"Energy_arr deleted"<<endl;}
    if(Res_arr){delete[] Res_arr;Res_arr=NULL;cout<<"Res_arr deleted"<<endl;}
    if(Res_error_arr){delete[] Res_error_arr;Res_error_arr=NULL;cout<<"Res_error_arr deleted"<<endl;}
  }
  //f_output->Close();
  
  return 0;
}

    
