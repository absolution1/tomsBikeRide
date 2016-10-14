{
  TString dir = gSystem->UnixPathName(__FILE__);
  dir.ReplaceAll("tomsBikeRide.C","");
  dir.ReplaceAll("/./","/");
  ifstream infile;
  infile.open(Form("%stoms_bike_ride.txt",dir.Data()));

  int date;
  double miles, speed, elevation;

  TFile *output = new TFile("output.root","RECREATE");
  TTree *tree = new TTree("tree","tree");

  tree->Branch("date",&date);
  tree->Branch("miles",&miles);
  tree->Branch("speed",&speed);
  tree->Branch("elevation",&elevation);

  while (infile >> date >> miles >> speed >> elevation){
    tree->Fill();
  }
  tree->Write();
  output->Close();
}
