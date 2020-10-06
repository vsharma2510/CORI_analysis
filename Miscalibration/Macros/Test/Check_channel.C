void CheckEnergy(int dataset, int channel)
{

  //TCanvas* can = new TCanvas("can","can",800,800);
  //can->Divide(2,1);
  TString dir = Form("/project/projectdirs/cuore/syncData/CUORE/OfficialProcessed/spring2019_reprocessing/output/ds%d",dataset);
  TString listname;
  int tower;
  if(channel%52==0){tower=(channel)/52;}  
  else{tower=((channel)/52+1);}
  cout<<"Tower being worked on is "<<tower<<endl;
  listname.Form("%s/calibration_Production_ds%d_tower%.3d.list",dir.Data(),dataset,tower);
  cout<<"The listfile is "<<listname.Data()<<endl;
  QChain ch("qtree");
  int nAdded=ch.Add(listname.Data());
  if(nAdded==0){cout<<"nAdded not work"<<endl;exit(1);}

  TString cut;
  cut.Form("Channel==%d",channel);
  //ch.Draw("Energy_heaterTGS>>a(3000,0,3000)",cut.Data(),"GOFF");
  // TH1D* h = (TH1D*)gROOT->Get("a");
  //h->GetXaxis()->SetTitle("Energy[keV]");
  //h->GetYaxis()->SetTitle("Counts");
  //can->cd(1);
  //h->Draw();
  //can->cd(2);
  ch.Draw("Energy_calibrationTGS>>a(3000,0,3000)",cut.Data(),"GOFF");
  TH1D* h1 = (TH1D*)gROOT->Get("a");
  h1->SetLineColor(kRed);
  h1->GetXaxis()->SetTitle("Energy[keV]");
  h1->GetYaxis()->SetTitle("Counts");
  h1->Draw();
}
