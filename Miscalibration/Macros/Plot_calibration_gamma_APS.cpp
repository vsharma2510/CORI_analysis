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
  TF1* line_H = NULL;
  TF1* line_C = NULL;
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

  /*---------- Setting up tex input file -----------*/
  ofstream output_tex_file;
  TString tex_file_name = Form("/global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/Cal_plots_images/ds%d/Gamma/ds%d_gamma.txt",dataset,dataset);
  output_tex_file.open(tex_file_name);

  /*---------- Looping over miscalibrated channels ----------*/
  
  for(it=channel.begin()+1;it!=channel.end();it++){
    cout<<"~~~~~~~~~~ begin for channel "<<*it<<"~~~~~~~~~~ "<<endl;
    myCan_H = new TCanvas("can_H","can_H",3600,3600);
    myCan_H->Divide(2,2,0.01   ,0.00001);

    myCan_C = new TCanvas("can_C","can_C",3600,3600);
    myCan_C->Divide(2,2,0.01   ,0.00001);
    
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
      line_H = new TF1("line_H","[0]*x",0,3000);
      
      line_H->SetParameter(0,0);
      line_H->SetLineColor(kRed);
      Res_gr_H->SetMarkerStyle(3);
      Res_gr_H->SetMarkerSize(3.0);
      Res_gr_H->GetXaxis()->SetTitle("Energy of peaks[keV]");
      Res_gr_H->GetYaxis()->SetTitle("Fixed Energy - True Energy");
      Res_gr_H->SetTitle(Form("Residuals for ch%d",*it));
      Res_gr_H->GetYaxis()->SetLabelOffset(0.008);
      Res_gr_H->Draw("AP");
      line_H->Draw("SAME");

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
      line_C = new TF1("line","[0]*x",0,3000);
      line_C->SetParameter(0,0);
      line_C->SetLineColor(kRed);
      Res_gr_C->SetMarkerStyle(3);
      Res_gr_C->SetMarkerSize(3.0);
      Res_gr_C->GetXaxis()->SetTitle("Energy of peaks[keV]");
      Res_gr_C->GetYaxis()->SetTitle("Fixed Energy - True Energy");
      Res_gr_C->SetTitle(Form("Residuals for ch%d",*it));
      Res_gr_H->GetYaxis()->SetLabelOffset(0.008);
      Res_gr_C->Draw("AP");
      line_C->Draw("SAME");

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
      fit_par_H = fit_H->GetParameter(0);
      fit_H->SetTitle(Form("Calibration fit function for ch %d;Stabilized amplitude;             Energy[keV]",*it));
      fit_H->GetYaxis()->SetTitleOffset(1.55);
      fit_H->Draw();
    }
    else{cout<<"Something went wrong with fitFunHandle for heater_TGS"<<endl;}
    
    QTObjectHandle<TGraphErrors> cgHandleH("CalGraph");
    cgHandleH.SetChannel((*it));
    dm.Get("CalCoefficients_heaterTGS",&cgHandleH,debug_file_name_heater.Data());
    if(cgHandleH.IsValid()){
      Cuore::QTObject<TGraphErrors> qCalGraphH;
      qCalGraphH = cgHandleH.Get();
      calgraph_H = (TGraphErrors*)(qCalGraphH.fObject.Clone(Form("clone_calgraph%d",(*it))));
      //gr->GetXaxis()->SetTitle("Stab_Amp");
      calgraph_H->SetMarkerStyle(2);
      calgraph_H->SetMarkerSize(4);
      calgraph_H->Draw("P");
    }
    else{cout<<"Something went wrong with cgHandle for heaterTGS"<<endl;}

     /* ---- For calibration_TGS ---- */
    
    myCan_C->cd(4);
    QTObjectHandle<TF1> fitFunHandleC("CalFunction");
    fitFunHandleC.SetChannel(*it);
    dm.Get("CalCoefficients_calibrationTGS",&fitFunHandleC,debug_file_name_cal.Data());
    if(fitFunHandleC.IsValid()){
      Cuore::QTObject<TF1> qfitC = fitFunHandleC.Get();
      fit_C = (TF1*)(qfitC.fObject.Clone(Form("clone_fit%d",*it)));
      fit_par_C = fit_C->GetParameter(0);
      fit_C->SetTitle(Form("Calibration fit function for ch %d;Stabilized amplitude                  ;Energy[keV]",*it));
      fit_C->GetYaxis()->SetTitleOffset(1.55);
      fit_C->Draw();
    }
    else{cout<<"Something went wrong with fitFunHandle for calibration_TGS"<<endl;}
    
    QTObjectHandle<TGraphErrors> cgHandleC("CalGraph");
    cgHandleC.SetChannel((*it));
    dm.Get("CalCoefficients_calibrationTGS",&cgHandleC,debug_file_name_cal.Data());
    if(cgHandleC.IsValid()){
      Cuore::QTObject<TGraphErrors> qCalGraphC;
      qCalGraphC = cgHandleC.Get();
      calgraph_C = (TGraphErrors*)(qCalGraphC.fObject.Clone(Form("clone_calgraph%d",(*it))));
      //gr->GetXaxis()->SetTitle("Stab_Amp");
      calgraph_C->SetMarkerStyle(2);
      calgraph_C->SetMarkerSize(4);
      calgraph_C->Draw("P");
    }
    else{cout<<"Something went wrong with cgHandle for calibrationTGS"<<endl;}
    
    //myCan->cd(2);
    int temp_energy_H,temp_energy_C;

    /*----- Loop over peaks for heater_TGS -----*/
    
    for(int i=0;i<arr_size_H;i++){
      myCan_H->cd(2);
      temp_energy_H=0;
      for(int j=0;j<num_peaks;j++){
	if(abs(Energy_H[i]-peak_list[j])<20){temp_energy_H=peak_list[j];}
      }
      /*---------- Plotting peak amplitude spectrum for heaterTGS ----------*/
      
      QTObjectHandle<TGraphAsymmErrors> pHistHandleH(Form("p%d_hist",temp_energy_H));
      //cout<<"pHistHandle uses "<<temp_energy<<endl;
      pHistHandleH.SetChannel((*it));
      cout<<"pHistHandleH contains ----- "<<pHistHandleH.GetName()<<" with label "<<pHistHandleH.GetLabel().GetStringLabel()<<endl;
      dm.Get("CalCoefficients_heaterTGS",&pHistHandleH,debug_file_name_heater.Data());
      if(pHistHandleH.IsValid()){
	//cout<<"~~~~~~~~~ Begin for "<<temp_energy_H<<" ~~~~~~~~~~"<<endl;
	Cuore::QTObject<TGraphAsymmErrors> gAEH;
	gAEH = pHistHandleH.Get();
	//gAE.fObject->Draw();
	gr2_H=(TGraphAsymmErrors*)(gAEH.fObject.Clone(Form("clone_hist%d",temp_energy_H)));
	//TGraphAsymmErrors gr3=gAE.fObject;
	//int n = gr3.GetN();
	//cout<<"~~~~~number of points in gr3 are "<<n<<"~~~~~"<<endl;
	gr2_H->GetXaxis()->SetTitle("Stabilized amplitude");
	gr2_H->GetYaxis()->SetTitle("Counts");
	gr2_H->SetTitle(Form("Fit (%d keV)",temp_energy_H));
	gr2_H->SetMarkerSize(2.5);
	gr2_H->Draw("AP");
	//gAE.Clear();
      }
      else{cout<<"Something went wrong with pHistHandle for heaterTGS"<<endl;}
      
      QTObjectHandle<TGraph> pHandleH(Form("p%d",temp_energy_H));
      pHandleH.SetChannel((*it));
      dm.Get("CalCoefficients_heaterTGS",&pHandleH,debug_file_name_heater.Data());
      if(pHandleH.IsValid()){
	Cuore::QTObject<TGraph> gPeakH;
	gPeakH = pHandleH.Get();
	//gPeak.fObject.Draw("AL");
	gr_H = (TGraph*)(gPeakH.fObject.Clone(Form("clone_%d",temp_energy_H)));
	gr_H->GetXaxis()->SetTitle("Stabilized amplitude");
	gr_H->GetYaxis()->SetTitle("Counts");
	gr_H->SetTitle(Form("Fit (%d keV)",temp_energy_H));
	gr_H->Draw("SAME");
	gPeakH.Clear();
      }
      else{cout<<"Something went wrong with pHandle for heaterTGS"<<endl;}
      
      /* ---------- Plotting full spectrum for heaterTGS ----------*/
      myCan_H->cd(1);
      QTObjectHandle<TH1D> spectHandleH("spectrum");
      spectHandleH.SetChannel(*it);
      dm.Get("CalCoefficients_heaterTGS",&spectHandleH,debug_file_name_heater.Data());
      if(spectHandleH.IsValid()){
	Cuore::QTObject<TH1D> hH = spectHandleH.Get();
	hSpectrum_H = (TH1D*)(hH.fObject.Clone(Form("clone_fullSpectrum%d",*it)));
	hSpectrum_H->SetTitle(Form("Stabilized amplitude spectrum of ch%d",*it));
	hSpectrum_H->SetXTitle("Stabilized amplitude");
	hSpectrum_H->SetYTitle("Counts");
	hSpectrum_H->SetStats(0);
	hSpectrum_H->Draw();
	double range_lower_H = 200;
	double range_upper_H = 4000;
	hSpectrum_H->GetXaxis()->SetRangeUser(range_lower_H,range_upper_H);
	cout<<"spectHandle uses "<<temp_energy_H<<" with histogram range of "<<range_lower_H<<" "<<range_upper_H<<endl;
      }
      else{cout<<"Something went wrong with spectHandle for heaterTGS"<<endl;}
      
      /*-----Saving plots for heaterTGS-----*/
      TString output_file_name_heater;
      output_file_name_heater = Form("/global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/Cal_plots_images/ds%d/Gamma/cal_plot_ds%d_ch%d_%d_keV_heaterTGS.pdf",dataset,dataset,(*it),temp_energy_H);
      myCan_H->Update();
      TString canvas_title_H;
      canvas_title_H = Form("Calibration plots for ds%d ch%d (heaterTGS)",dataset,*it);
      myCan_H->SetTitle(canvas_title_H);
      myCan_H->SaveAs(output_file_name_heater);

      /*----- Saving tex command to tex input file for heaterTGS -----*/
      TString tex_string_H = Form("\\begin{figure}[h]\n\\centering\n\\includegraphics[/Users/vivek/Documents/Miscalibration_images/Cal_plots_images/ds%d/Gamma/cal_plot_ds%d_ch%d_%d_keV_heaterTGS.pdf]\n\\caption{Calibration plots for dataset %d, channel %d and calibration peak of %d keV for heaterTGS}\n\\end{figure}\n",dataset,dataset,(*it),temp_energy_H,dataset,(*it),temp_energy_H);
      output_tex_file<<tex_string_H;
      
      //myCan->Clear();
      //if(Res_gr){delete Res_gr;Res_gr=NULL;cout<<"Res_gr deleted"<<endl;}
      //if(line){delete line;line=NULL;cout<<"line deleted"<<endl;}
      //if(line){delete line;cout<<"line deleted 2"<<endl;}
      //if(fit){delete fit;fit=NULL;cout<<"fit deleted"<<endl;}
      //if(calgraph){delete calgraph;calgraph=NULL;cout<<"calgraph deleted"<<endl;}
      if(gr_H){delete gr_H;gr_H=NULL;cout<<"gr_H deleted"<<endl;}
      if(gr2_H){delete gr2_H;gr2_H=NULL;cout<<"gr2_H deleted"<<endl;}
      if(hSpectrum_H){delete hSpectrum_H;hSpectrum_H=NULL;cout<<"hSpectrum_H deleted"<<endl;}
      //if(Energy_arr){delete[] Energy_arr;Energy_arr=NULL;cout<<"Energy_arr deleted"<<endl;}
      //if(Res_arr){delete[] Res_arr;Res_arr=NULL;cout<<"Res_arr deleted"<<endl;}
      //if(Res_error_arr){delete[] Res_error_arr;Res_error_arr=NULL;cout<<"Res_error_arr deleted"<<endl;}
    }
    if(Res_gr_H){delete Res_gr_H;Res_gr_H=NULL;cout<<"Res_gr_H deleted"<<endl;}
    if(line_H){delete line_H;line_H=NULL;cout<<"line_H deleted"<<endl;}
    if(line_H){delete line_H;cout<<"line_H deleted 2"<<endl;}
    if(fit_H){delete fit_H;fit_H=NULL;cout<<"fit_H deleted"<<endl;}
    if(calgraph_H){delete calgraph_H;calgraph_H=NULL;cout<<"calgraph_H deleted"<<endl;}
    if(Energy_arr_H){delete[] Energy_arr_H;Energy_arr_H=NULL;cout<<"Energy_arr_H deleted"<<endl;}
    if(Res_arr_H){delete[] Res_arr_H;Res_arr_H=NULL;cout<<"Res_arr_H deleted"<<endl;}
    if(Res_error_arr_H){delete[] Res_error_arr_H;Res_error_arr_H=NULL;cout<<"Res_error_arr_H deleted"<<endl;}

    /*----- Loop over peaks for calibration_TGS -----*/
    
    for(int i=0;i<arr_size_C;i++){
      myCan_C->cd(2);
      temp_energy_C=0;
      for(int j=0;j<num_peaks;j++){
	if(abs(Energy_C[i]-peak_list[j])<20){temp_energy_C=peak_list[j];}
      }
      /*---------- Plotting peak amplitude spectrum for calibrationTGS ----------*/
      
      QTObjectHandle<TGraphAsymmErrors> pHistHandleC(Form("p%d_hist",temp_energy_C));
      //cout<<"pHistHandle uses "<<temp_energy<<endl;
      pHistHandleC.SetChannel((*it));
      cout<<"pHistHandleC contains ----- "<<pHistHandleC.GetName()<<" with label "<<pHistHandleC.GetLabel().GetStringLabel()<<endl;
      dm.Get("CalCoefficients_calibrationTGS",&pHistHandleC,debug_file_name_cal.Data());
      if(pHistHandleC.IsValid()){
	//cout<<"~~~~~~~~~ Begin for "<<temp_energy_C<<" ~~~~~~~~~~"<<endl;
	Cuore::QTObject<TGraphAsymmErrors> gAEC;
	gAEC = pHistHandleC.Get();
	//gAEC.fObject->Draw();
	gr2_C=(TGraphAsymmErrors*)(gAEC.fObject.Clone(Form("clone_hist%d",temp_energy_C)));
	//TGraphAsymmErrors gr3=gAE.fObject;
	//int n = gr3.GetN();
	//cout<<"~~~~~number of points in gr3 are "<<n<<"~~~~~"<<endl;
	gr2_C->GetXaxis()->SetTitle("Stabilized amplitude");
	gr2_C->GetYaxis()->SetTitle("Counts");
	gr2_C->SetTitle(Form("Fit (%d keV)",temp_energy_C));
	gr2_C->SetMarkerSize(2.5);
	gr2_C->Draw("AP");
	//gAEC.Clear();
      }
      else{cout<<"Something went wrong with pHistHandle for calibrationTGS"<<endl;}
      
      QTObjectHandle<TGraph> pHandleC(Form("p%d",temp_energy_C));
      pHandleC.SetChannel((*it));
      dm.Get("CalCoefficients_calibrationTGS",&pHandleC,debug_file_name_cal.Data());
      if(pHandleC.IsValid()){
	Cuore::QTObject<TGraph> gPeakC;
	gPeakC = pHandleC.Get();
	//gPeakC.fObject.Draw("AL");
	gr_C = (TGraph*)(gPeakC.fObject.Clone(Form("clone_%d",temp_energy_C)));
	gr_C->GetXaxis()->SetTitle("Stabilized amplitude");
	gr_C->GetYaxis()->SetTitle("Counts");
	gr_C->SetTitle(Form("Fit (%d keV)",temp_energy_C));
	gr_C->Draw("SAME");
	gPeakC.Clear();
      }
      else{cout<<"Something went wrong with pHandle for calibrationTGS"<<endl;}
      
      /* ---------- Plotting full spectrum for calibrationTGS ----------*/
      myCan_C->cd(1);
      QTObjectHandle<TH1D> spectHandleC("spectrum");
      spectHandleC.SetChannel(*it);
      dm.Get("CalCoefficients_calibrationTGS",&spectHandleC,debug_file_name_cal.Data());
      if(spectHandleC.IsValid()){
	Cuore::QTObject<TH1D> hC = spectHandleC.Get();
	hSpectrum_C = (TH1D*)(hC.fObject.Clone(Form("clone_fullSpectrum%d",*it)));
	hSpectrum_C->SetTitle(Form("Stabilized amplitude spectrum for ch %d",*it));
	hSpectrum_C->SetXTitle("Stabilized amplitude");
	hSpectrum_C->SetYTitle("Counts");
	hSpectrum_C->SetStats(0);
	hSpectrum_C->Draw();
	//double range_lower_C = (temp_energy_C/fit_par_C)-500;
	//double range_upper_C = (temp_energy_C/fit_par_C)+500;
	//hSpectrum_C->GetXaxis()->SetRangeUser(range_lower_C,range_upper_C);
	//cout<<"spectHandleC uses "<<temp_energy_C<<" with histogram range of "<<range_lower_C<<" "<<range_upper_C<<endl;
      }
      else{cout<<"Something went wrong with spectHandle for calibrationTGS"<<endl;}
      
      TString output_file_name_cal;
      output_file_name_cal = Form("/global/homes/v/vsharma2/Analysis_code/Miscalibration/Output/Cal_plots_images/ds%d/Gamma/cal_plot_ds%d_ch%d_%d_keV_calibrationTGS.pdf",dataset,dataset,(*it),temp_energy_C);
      myCan_C->Update();
      TString canvas_title_C;
      canvas_title_C = Form("Calibration plots for ds%d ch%d (calibrationTGS)",dataset,*it);
      myCan_C->SetTitle(canvas_title_C);
      myCan_C->SaveAs(output_file_name_cal);

      /*----- Saving tex command to tex input file for calibrationTGS -----*/
      TString tex_string_C = Form("\\begin{figure}[h]\n\\centering\n\\includegraphics[/Users/vivek/Documents/Miscalibration_images/Cal_plots_images/ds%d/Gamma/cal_plot_ds%d_ch%d_%d_keV_calibrationTGS.pdf]\n\\caption{Calibration plots for dataset %d, channel %d and calibration peak of %d keV for calibrationTGS}\n\\end{figure}\n",dataset,dataset,(*it),temp_energy_C,dataset,(*it),temp_energy_C);
      output_tex_file<<tex_string_C;
      
      //myCan->Clear();
      //if(Res_gr){delete Res_gr;Res_gr=NULL;cout<<"Res_gr deleted"<<endl;}
      //if(line){delete line;line=NULL;cout<<"line deleted"<<endl;}
      //if(line){delete line;cout<<"line deleted 2"<<endl;}
      //if(fit){delete fit;fit=NULL;cout<<"fit deleted"<<endl;}
      //if(calgraph){delete calgraph;calgraph=NULL;cout<<"calgraph deleted"<<endl;}
      if(gr_C){delete gr_C;gr_C=NULL;cout<<"gr_C deleted"<<endl;}
      if(gr2_C){delete gr2_C;gr2_C=NULL;cout<<"gr2_C deleted"<<endl;}
      if(hSpectrum_C){delete hSpectrum_C;hSpectrum_C=NULL;cout<<"hSpectrum_C deleted"<<endl;}
      //if(Energy_arr){delete[] Energy_arr;Energy_arr=NULL;cout<<"Energy_arr deleted"<<endl;}
      //if(Res_arr){delete[] Res_arr;Res_arr=NULL;cout<<"Res_arr deleted"<<endl;}
      //if(Res_error_arr){delete[] Res_error_arr;Res_error_arr=NULL;cout<<"Res_error_arr deleted"<<endl;}
    }
    if(Res_gr_C){delete Res_gr_C;Res_gr_C=NULL;cout<<"Res_gr_C deleted"<<endl;}
    if(line_C){delete line_C;line_C=NULL;cout<<"line_C deleted"<<endl;}
    if(line_C){delete line_C;cout<<"line_C deleted 2"<<endl;}
    if(fit_C){delete fit_C;fit_C=NULL;cout<<"fit_C deleted"<<endl;}
    if(calgraph_C){delete calgraph_C;calgraph_C=NULL;cout<<"calgraph_C deleted"<<endl;}
    if(Energy_arr_C){delete[] Energy_arr_C;Energy_arr_C=NULL;cout<<"Energy_arr_C deleted"<<endl;}
    if(Res_arr_C){delete[] Res_arr_C;Res_arr_C=NULL;cout<<"Res_arr_C deleted"<<endl;}
    if(Res_error_arr_C){delete[] Res_error_arr_C;Res_error_arr_C=NULL;cout<<"Res_error_arr_C deleted"<<endl;}
    
  }
  output_tex_file.close();
  //f_output->Close();
  
  return 0;
}

    
