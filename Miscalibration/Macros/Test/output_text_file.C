{
  for(int j=0;j<10;j++){
    ofstream output_file;
    TString file_name = Form("./Output_text_test/test_output_text_%d.txt",j);
    output_file.open(file_name);
    for(int i=0;i<10;i++){
      TString str = Form("\\text 1 for loop %d for file %d\n\\text 2 for loop %d for file %d\n",i,j,i,j);
      output_file<<str;
    }
    output_file.close();
  }
}
