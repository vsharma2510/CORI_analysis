{
  Cuore::QGlobalDataManager dm;

  int energy_value = 2615;
  int dataset = 3519;
  int channel = 5;

  TString source_filename="/global/homes/v/vsharma2/Test_Diana/calib/ds3519/wOF/cal_coeffs_ds3519_tower001.txt";

  QCalibrationParametersHandle paramH(channel,dataset,"CalibrationParameters");
  dm.Get("CalCoefficients_heaterTGS",&paramH,source_filename.Data());
  if(paramH.IsValid()){
    QCalibrationParameters& params=paramH.Get();
    params.Dump();

    std::vector<double> Res;
    std::vector<double> Res_error;

    for(int iR=0;iR<params.fResiduals.size();iR++){
      const QCalibrationResiduals& aRes=params.GetCalibrationResidual(iR);
      cout<<"PeakEnergy   "<<aRes.PeakEnergy<<"   Residual   "<<aRes.Residual<<"   Residual Error   "<<aRes.ErrorResidual<<endl;
      cout<<"Success"<<endl;
    }
    
    //cout<<"PeakEnergy   "<<aRes.PeakEnergy<<"   Residual   "<<aRes.Residual<<"   Residual Error   "<<aRes.ErrorResidual<<endl;
  }
  else{cout<<"Something went wrong"<<endl;}
}

