{
  const double price = 900.00; // Cost of bike in pounds.
  
  TString dir = gSystem->UnixPathName(__FILE__);
  dir.ReplaceAll("tomsBikeRide.C","");
  dir.ReplaceAll("/./","/");
  ifstream infile;
  infile.open(Form("%stoms_bike_ride.txt",dir.Data()));

  int date;
  double miles, speed, elevation, poundspermile;

  TFile *output = new TFile("output.root","RECREATE");
  TTree *tree = new TTree("tree","tree");

  tree->Branch("date",&date);
  tree->Branch("miles",&miles);
  tree->Branch("speed",&speed);
  tree->Branch("elevation",&elevation);
  tree->Branch("poundspermile",&poundspermile);

  double totalmiles = 0; // total miles accumulator
  
  while (infile >> date >> miles >> speed >> elevation){
    totalmiles+=miles;
    poundspermile=price/totalmiles;
    tree->Fill();
  }
  tree->Write();

  int n = tree->Draw("poundspermile:date","","goff");
  TGraph *pricepermile = new TGraph(n, tree->GetV2(), tree->GetV1());
  pricepermile->SetMarkerStyle(20);
  pricepermile->GetXaxis()->SetTimeDisplay(1);
  pricepermile->GetXaxis()->SetTitle("Date");
  pricepermile->GetYaxis()->SetTitle("Cost (in GBP) for total accrued mileage");
  pricepermile->SetTitle("");
  pricepermile->Draw();

  
  std::cout<<totalmiles<<std::endl;

  TText *t = new TText(.5,.5,Form("Total milage accrued: %.1f", totalmiles));
  t->SetNDC();
  t->SetTextAlign(22);
  t->SetTextColor(kRed+2);
  t->SetTextFont(43);
  t->SetTextSize(40);
  t->SetTextAngle(45);
  t->Draw();

  output->Close();
}
