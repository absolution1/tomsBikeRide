void plot_time_hist(TTree * tree, const char * vars, const char * cuts, const char * yaxis)
{
  int n = tree->Draw(vars,cuts,"goff");
  TGraph *g = new TGraph(n, tree->GetV2(), tree->GetV1());
  g->SetMarkerStyle(20);
  //TDatime da(1970,01,01,00,00,00);
  //gStyle->SetTimeOffset(da.Convert());
  g->GetXaxis()->SetTimeFormat("%d/%m/%Y%F1970-01-01 00:00:00");
  g->GetXaxis()->SetTimeDisplay(1);
  g->GetXaxis()->SetTitle("Date (UTC)");
  g->GetYaxis()->SetTitle(yaxis);
  g->GetXaxis()->SetTitleSize(.05);
  g->GetYaxis()->SetTitleSize(.05);
  g->GetXaxis()->SetLabelSize(.05);
  g->GetYaxis()->SetLabelSize(.05);
  g->SetTitle("");
  g->Draw();
}

void plot_text(double x, double y, const char * text)
{
  TText *t = new TText(x, y, text);
  t->SetNDC();
  t->SetTextAlign(22);
  t->SetTextColor(kRed+2);
  t->SetTextFont(43);
  t->SetTextSize(20);
  t->SetTextAngle(45);
  t->Draw();
}

void tomsBikeRide()
{
  double pricetemp = 900.00;  // Cost of bike in pounds.
  pricetemp += 300; // cost of accessories, clothes, etc. (estimate)
  pricetemp -= 250; // saving from cycleschme (estimate)
  pricetemp += 80; // price to buy from the cyclescheme people (estimate)
  pricetemp += 250; // price of insurance excess
  const double price = pricetemp;
  
  TString dir = gSystem->UnixPathName(__FILE__);
  dir.ReplaceAll("tomsBikeRide.C","");
  dir.ReplaceAll("/./","/");
  ifstream infile;
  infile.open(Form("%stoms_bike_ride.txt",dir.Data()));

  int date, bikeId;
  double miles, speed, elevation, poundspermile, poundsperhour, commutefraction;
  bool commute;

  TFile *output = new TFile("output.root","RECREATE");
  TTree *tree = new TTree("tree","tree");

  tree->Branch("date",&date);
  tree->Branch("miles",&miles);
  tree->Branch("speed",&speed);
  tree->Branch("elevation",&elevation);
  tree->Branch("poundspermile",&poundspermile);
  tree->Branch("poundsperhour",&poundsperhour);
  tree->Branch("commute",&commute);
  tree->Branch("commutefraction",&commutefraction);
  tree->Branch("bikeId", &bikeId);

  double totalmiles = 0; // total miles accumulator
  double totalmiles_commute = 0;
  double totalhours = 0;
  
  while (infile >> date >> miles >> speed >> elevation >> commute >> bikeId){
    if(bikeId == 0 || bikeId == 4) {
      totalmiles+=miles;
      totalhours+=miles/speed;
      poundspermile=price/totalmiles;
      poundsperhour=price/totalhours;
      if(commute)
	totalmiles_commute += miles;
      commutefraction = totalmiles_commute / totalmiles;
    }
    tree->Fill();
  }
  tree->Write();

  std::cout << totalmiles << " miles at £"
	    << TString::Format("%.2f", poundspermile) << " pounds per mile"
	    << std::endl;
  std::cout << totalhours << " hours at £"
	    << TString::Format("%.2f", poundsperhour) << " pounds per hour"
	    << std::endl;

  //plot the pounds per mile
  TCanvas * c = new TCanvas("c","c",700,1200);
  c->Divide(1,3);
  c->SetGrid();

  c->cd(1);
  plot_time_hist(tree, "poundspermile:date", "bikeId==0||bikeId==4", "Cost (in GBP) for total accrued mileage");
  plot_text(.3,.5, Form("Total: %.1f miles", totalmiles));
  plot_text(.7,.5, TString::Format("£%.2f per mile", poundspermile).Data());

  c->cd(2);
  plot_time_hist(tree, "poundsperhour:date", "bikeId==0||bikeId==4", "Cost (in GBP) for total accrued time");
  plot_text(.3,.5, Form("Total fun: %.1f miles", totalmiles - totalmiles_commute));
  plot_text(.7,.5, Form("Total time: %.1f hours", totalhours));

  c->cd(3);
  plot_time_hist(tree, "commutefraction:date", "bikeId==0||bikeId==4", "Commute fraction");
  plot_text(.3,.5, Form("Total commute: %.1f miles", totalmiles_commute));
  plot_text(.7,.5, Form("Commute fraction: %.2f", commutefraction));

  output->Close();

  cout << Form("£%.2f per mile", poundspermile) << endl;
}
