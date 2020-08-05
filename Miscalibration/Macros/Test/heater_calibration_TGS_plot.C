{
  TString debug_file_heater = "/global/homes/v/vsharma2/Test_Diana/calib/ds3519/wOF/debug/cal_debug_ds3519_tower001.root";
  TString debug_file_cal = "/global/homes/v/vsharma2/Test_Diana/calib/ds3519/wOF_WoH/debug/cal_debug_ds3519_tower001.root";
  Cuore::QGlobalDataManager dm;

  TCanvas* can = new TCanvas("can","can",800,800);
  can->Divide(2,1);
  

  QTObjectHandle<TGraphAsymmErrors> pHistHandleH(Form("p%d_hist",2615));
  pHistHandleH.SetChannel(5);
  dm.Get("CalCoefficients_heaterTGS",&pHistHandleH,debug_file_heater.Data());
  if(pHistHandleH.IsValid()){
    Cuore::QTObject<TGraphAsymmErrors> gH;
    gH = pHistHandleH.Get();
    TGraphAsymmErrors* gr1 = (TGraphAsymmErrors*)(gH.fObject.Clone(Form("clone_hist%d",2615)));
    can->cd(1);
    gr1->Draw("AP");
  }
  QTObjectHandle<TGraphAsymmErrors> pHistHandleC(Form("p%d_hist",2615));
  pHistHandleC.SetChannel(5);
  dm.Get("CalCoefficients_calibrationTGS",&pHistHandleC,debug_file_cal.Data());
  if(pHistHandleC.IsValid()){
      Cuore::QTObject<TGraphAsymmErrors> gC;
      gC = pHistHandleC.Get();
      TGraphAsymmErrors* gr2 = (TGraphAsymmErrors*)(gC.fObject.Clone(Form("clone_hist%d",2615)));
      can->cd(2);
      //gr2->SetLineColor(kRed);
      gr2->Draw("AP");
    }
  else(cout<<"Something went wrong with calibration_TGS plot"<<endl);
}
    
      

    
