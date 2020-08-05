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
  double fit_par_H=1.;
  double fit_par_C=1.;
  const int num_peaks=9;

  std::vector<int> channel;
  std::vector<int>::iterator it;
  TString dir = "/global/homes/v/vsharma2/Test_Diana/calib";
  int peak_list [num_peaks] = {239,338,583,511,911,969,1173,1333,2615};

  TGraphErrors* Res_gr_H= NULL;
  TGraphErrors* Res_gr_C= NULL;
  TF1* line = NULL;
  TF1* fit_H = NULL;
  TF1* fit_C = NULL;
  TGraphErrors* calgraph_H = NULL;
  TGraphErrors* calgraph_C = NULL;
  TGraph* gr_H = NULL;
  TGraph* gr_C = NULL;
  TGraphAsymmErrors* gr2_H = NULL;
  TGraphAsymmErrors* gr2_C = NULL;
  TH1D* hSpectrum_H = NULL;
  TH1D* hSpectrum_C = NULL;
  double* Energy_arr_H = NULL;
  double* Res_arr_H = NULL;
  double* Res_error_arr_H = NULL;
  double* Energy_arr_C = NULL;
  double* Res_arr_C = NULL;
  double* Res_error_arr_C = NULL;

  TCanvas* myCan_H = NULL;
  TCanvas* myCan_C = NULL;
  
  ifstream listfile(argv[1]);
  if(!listfile){
    cout<<"!!!!!!!! Unable to open channel list file !!!!!!!!!"<<endl;
  }

  /* ---------- Obtain list of miscalibrated channels ----------*/
  string file_line;
  while(getline(listfile,file_line)){
    stringstream ss(file_line);
    ss>>x;
    cout<<"-------- channel being added is --------"<<x<<endl;
    channel.push_back(x);
  }

  int dataset = channel[0]; //first element of channel[] is dataset

  /*---------- Looping over miscalibrated channels ----------*/
  
  for(it=channel.begin()+1;it!=channel.end();it++){
    cout<<"~~~~~~~~~~ begin for channel "<<*it<<"~~~~~~~~~~ "<<endl;
    myCan_H = new TCanvas("can_H","can_H",2400,2400);
    myCan_H->Divide(2,2,0.00001,0.01);

    myCan_C = new TCanvas("can_C","can_C",2400,2400);
    myCan_C->Divide(2,2,0.00001,0.01);
    
    if(*it%52==0){tower=(*it)/52;}  
    else{tower=((*it)/52+1);}
    cout<<"Tower being worked on is "<<tower<<endl;

    /* Setting up file names for heater_TGS */
    
    TString debug_file_name_heater = Form("%s/ds%d/wOF/debug/cal_debug_ds%d_tower%.3d.root",dir.Data(),dataset,dataset,tower);
    TString cal_file_name_heater = Form("%s/ds%d/wOF/cal_coeffs_ds%d_tower%.3d.txt",dir.Data(),dataset,dataset,tower);

     /* Setting up file names for calibration_TGS */
    
    TString debug_file_name_cal = Form("%s/ds%d/wOF_WoH/debug/cal_debug_ds%d_tower%.3d.root",dir.Data(),dataset,dataset,tower);
    TString cal_file_name_cal = Form("%s/ds%d/wOF_WoH/cal_coeffs_ds%d_tower%.3d.txt",dir.Data(),dataset,dataset,tower);

    cout<<debug_file_name_heater<<endl;
    cout<<cal_file_name_heater<<endl;

    unsigned int arr_size_H=0;
    std::vector<double> Energy_H;
    std::vector<double> Res_H;
    std::vector<double> Res_error_H;

    unsigned int arr_size_C=0;
    std::vector<double> Energy_C;
    std::vector<double> Res_C;
    std::vector<double> Res_error_C;

    /*---------- Plotting Residuals ----------*/

    /* ---- For heaterTGS ---- */
    
    QCalibrationParametersHandle paramHH((*it),dataset,"CalibrationParameters");
    dm.Get("CalCoefficients_heaterTGS",&paramHH,cal_file_name_heater.Data());
    myCan_H->cd(3);
    if(paramHH.IsValid()){
      const QCalibrationParameters& paramsH=paramHH.Get();
      
      /* ---------- Looping over peaks ----------*/
      for(int iR=0;iR<paramsH.fResiduals.size();iR++){
	const QCalibrationResiduals& aResH=paramsH.GetCalibrationResidual(iR);
	Energy_H.push_back(aResH.PeakEnergy);
	Res_H.push_back(aResH.Residual);
	Res_error_H.push_back(aResH.ErrorResidual);
	cout<<"PeakEnergy   "<<aResH.PeakEnergy<<"   Residual   "<<aResH.Residual<<"   Residual Error   "<<aResH.ErrorResidual<<endl;
	cout<<"Success"<<endl;
      }
      
      arr_size_H = Energy_H.size();
      //cout<<arr_size<<endl;
      Energy_arr_H = new double [arr_size_H];
      Res_arr_H = new double [arr_size_H];
      Res_error_arr_H = new double [arr_size_H];
      
      for(int i=0;i<arr_size_H;i++){
	Energy_arr_H[i]=Energy_H[i];
	Res_arr_H[i]=Res_H[i];
	Res_error_arr_H[i]=Res_error_H[i];
	//cout<<Energy_arr[i]<<"   "<<Res_arr[i]<<"   "<<Res_error_arr[i]<<endl;
      }
      
      Res_gr_H = new TGraphErrors(arr_size_H,Energy_arr_H,Res_arr_H,0,Res_error_arr_H);
      line = new TF1("line","[0]*x",0,3000);
      
      line->SetParameter(0,0);
      line->SetLineColor(kRed);
      Res_gr_H->SetMarkerStyle(3);
      Res_gr_H->SetMarkerSize(3.0);
      Res_gr_H->GetXaxis()->SetTitle("Energy of peaks[keV]");
      Res_gr_H->GetYaxis()->SetTitle("Fixed Energy - True Energy");
      Res_gr_H->SetTitle(Form("Residuals for ch%d",*it));
      Res_gr_H->Draw("AP");
      line->Draw("SAME");

      /*delete[] Energy_arr;
      delete[] Res_arr;
      delete[] Res_error_arr;*/
    }
    else{cout<<"Something went wrong with parameter handle for heater_TGS"<<endl;}

     /* ---- For calibrationTGS ---- */
    
    QCalibrationParametersHandle paramHC((*it),dataset,"CalibrationParameters");
    dm.Get("CalCoefficients_calibrationTGS",&paramHC,cal_file_name_cal.Data());
    myCan_C->cd(3);
    if(paramHC.IsValid()){
      const QCalibrationParameters& paramsC=paramHC.Get();
      
      /* ---------- Looping over peaks ----------*/
      for(int iR=0;iR<paramsC.fResiduals.size();iR++){
	const QCalibrationResiduals& aResC=paramsC.GetCalibrationResidual(iR);
	Energy_C.push_back(aResC.PeakEnergy);
	Res_C.push_back(aResC.Residual);
	Res_error_C.push_back(aResC.ErrorResidual);
	cout<<"PeakEnergy   "<<aResC.PeakEnergy<<"   Residual   "<<aResC.Residual<<"   Residual Error   "<<aResC.ErrorResidual<<endl;
	cout<<"Success"<<endl;
      }
      
      arr_size_C = Energy_C.size();
      //cout<<arr_size<<endl;
      Energy_arr_C = new double [arr_size_C];
      Res_arr_C = new double [arr_size_C];
      Res_error_arr_C = new double [arr_size_C];
      
      for(int i=0;i<arr_size_C;i++){
	Energy_arr_C[i]=Energy_C[i];
	Res_arr_C[i]=Res_C[i];
	Res_error_arr_C[i]=Res_error_C[i];
	//cout<<Energy_arr[i]<<"   "<<Res_arr[i]<<"   "<<Res_error_arr[i]<<endl;
      }
      
      Res_gr_C = new TGraphErrors(arr_size_C,Energy_arr_C,Res_arr_C,0,Res_error_arr_C);
      line = new TF1("line","[0]*x",0,3000);
      
      line->SetParameter(0,0);
      line->SetLineColor(kRed);
      Res_gr_C->SetMarkerStyle(3);
      Res_gr_C->SetMarkerSize(3.0);
      Res_gr_C->GetXaxis()->SetTitle("Energy of peaks[keV]");
      Res_gr_C->GetYaxis()->SetTitle("Fixed Energy - True Energy");
      Res_gr_C->SetTitle(Form("Residuals for ch%d",*it));
      Res_gr_C->Draw("AP");
      line->Draw("SAME");

      /*delete[] Energy_arr;
      delete[] Res_arr;
      delete[] Res_error_arr;*/
    }
    else{cout<<"Something went wrong with parameter handle for calibration_TGS"<<endl;}


    /*---------- Plotting calibration function ----------*/
    
    /* ---- For heater_TGS ---- */
    
    myCan_H->cd(4);
    QTObjectHandle<TF1> fitFunHandleH("CalFunction");
    fitFunHandleH.SetChannel(*it);
    dm.Get("CalCoefficients_heaterTGS",&fitFunHandleH,debug_file_name_heater.Data());
    if(fitFunHandleH.IsValid()){
      Cuore::QTObject<TF1> qfitH = fitFunHandleH.Get();
      fit_H = (TF1*)(qfitH.fObject.Clone(Form("clone_fit%d",*it)));
      fit_par = fit_H->GetParameter(0);
      fit_H->SetTitle(Form("Calibration fit function for ch %d;Stabilized Amplitude (heater_TGS);Energy[keV]",*it));
      fit_H->GetYaxis()->SetTitleOffset(1.45);
      fit_H->Draw();
    }
    else{cout<<"Something went wrong with fitFunHandle for heater_TGS"<<endl;}
    
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
      /*---------- Plotting peak amplitude spectrum ----------*/

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

    
