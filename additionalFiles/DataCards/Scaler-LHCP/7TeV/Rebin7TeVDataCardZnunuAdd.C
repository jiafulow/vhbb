#include "TH1.h"
#include "TH1F.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLatex.h"
#include "math.h"
#include "TMath.h"
#include <iostream>
#include <fstream>
#include "TLine.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "THStack.h"
#include "TList.h"
#include "xsec7TeV.h"
#include "xsec8TeV.h"
#include "filesZH.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "tdrstyle.h"

const int bins = 20;

int kount;
int kount2;
TString IFILE;
int maxCount;

std::string massS[51]={
"110","110_5","111","111_5","112","112_5","113","113_5","114","114_5",
"115","115_5","116","116_5","117","117_5","118","118_5","119","119_5",
"120","120_5","121","121_5","122","122_5","123","123_5","124","124_5",
"125","125_5","126","126_5","127","127_5","128","128_5","129","129_5",
"130","130_5","131","131_5","132","132_5","133","133_5","134","134_5",
"135"};


void makeSystPlot( TFile * f, TString oldFolder, RooWorkspace *WS,  string channel, string syst, int toMassNo, int fromMassNo, int rightBinNo, int addRightBin) //massNo 0-51, see xSec7TeV.h 
{

  RooArgList  * hobs = new RooArgList("hobs");
  RooRealVar BDT("CMS_vhbb_BDT_Znn", "CMS_vhbb_BDT_Znn", -1, 1);///OLD VARIABLE NAME HERE
  hobs->add(*WS->var("CMS_vhbb_BDT_Znn"));  ///NEW VARIABLE NAME HERE
  RooWorkspace *tempWS =  (RooWorkspace*) f->Get(oldFolder.Data());
  TString systT(syst);
  TString chanT(channel);

  if((kount < 3) && (channel=="data_obs"))
  {
    kount++;
    std::string namen  = channel;
    
    std::cout << oldFolder.Data() << std::endl;
    std::cout << namen << std::endl;
    RooDataHist* tempRooDataHistNom = (RooDataHist*)  tempWS->data(namen.c_str());
    TH1 *tempHistNom = tempRooDataHistNom->createHistogram(namen.c_str(),BDT,RooFit::Binning(bins));
    std::cout << namen << std::endl;
 
    if(addRightBin == 1)
    {
     float  err0 = tempHistNom->GetBinError(rightBinNo);
     float  con0 = tempHistNom->GetBinContent(rightBinNo);
 
     float  err1 = tempHistNom->GetBinError(rightBinNo-1);
     float  con1 = tempHistNom->GetBinContent(rightBinNo-1);
       
     tempHistNom->SetBinContent(rightBinNo,0);
     tempHistNom->SetBinError(rightBinNo,0);
     //   tempHistNom->SetBinContent(rightBinNo-1,con0+con1);
     //  tempHistNom->SetBinError(rightBinNo-1,sqrt(err0*err0+err1*err1));
    }

    RooDataHist *DHnom = new RooDataHist(channel.c_str(),"",*hobs,tempHistNom);  
    WS->import(*(new RooHistPdf(channel.c_str(),"",*hobs,*DHnom)));
 
 }

 if (channel!="data_obs")
{
  std::string nameUp; 
  std::string namen; 
  std::string nameDown;


  if((syst == "stat"))
  {
    if(IFILE.Contains("7TeV"))
    { 
      nameUp  = channel + "_CMS_vhbb_stat" + channel + "_" + oldFolder.Data() + "_7TeVUp";
      namen  = channel;
      nameDown  = channel + "_CMS_vhbb_stat" + channel + "_" + oldFolder.Data() + "_7TeVDown";
    }
    if(IFILE.Contains("8TeV"))
    { 
      nameUp  = channel + "_CMS_vhbb_stat" + channel + "_" + oldFolder.Data() + "_8TeVUp";
      namen  = channel;
      nameDown  = channel + "_CMS_vhbb_stat" + channel + "_" + oldFolder.Data() + "_8TeVDown";
    }

  }
  else
  {
  nameUp  = channel + "_CMS_" + syst + "Up";
  namen  = channel;
  nameDown = channel + "_CMS_" + syst + "Down";
  }
  if((syst == "ZJModel"))
  {
    if(IFILE.Contains("7TeV"))
    { 
      nameUp  = channel + "_CMS_vhbb_ZJModel_" + oldFolder.Data() + "_7TeVUp";
      namen  = channel;
      nameDown  = channel + "_CMS_vhbb_ZJModel_" + oldFolder.Data() + "_7TeVDown";
    }
    if(IFILE.Contains("8TeV"))
    { 
      nameUp  = channel + "_CMS_vhbb_ZJModel_" + oldFolder.Data() + "_8TeVUp";
      namen  = channel;
      nameDown  = channel + "_CMS_vhbb_ZJModel_" + oldFolder.Data() + "_8TeVDown";
    }

  }
  

  RooDataHist* tempRooDataHistUp = (RooDataHist*)  tempWS->data(nameUp.c_str());
  RooDataHist* tempRooDataHistDown = (RooDataHist*)  tempWS->data(nameDown.c_str());
  RooDataHist* tempRooDataHistNom = (RooDataHist*)  tempWS->data(namen.c_str());


  std::cout << oldFolder.Data() << std::endl; 
  std::cout << nameUp.c_str() << std::endl; 
  
  TH1 *tempHistUp = tempRooDataHistUp->createHistogram(nameUp.c_str(),BDT,RooFit::Binning(bins));
  TH1 *tempHistDown = tempRooDataHistDown->createHistogram(nameDown.c_str(),BDT,RooFit::Binning(bins));
  TH1 *tempHistNom = tempRooDataHistNom->createHistogram(namen.c_str(),BDT,RooFit::Binning(bins));
  
  if(chanT.Contains("ZH") && IFILE.Contains("7TeV"))
  {
     tempHistUp->Scale(xSec7ZH[toMassNo]/xSec7ZH[fromMassNo]);
     tempHistDown->Scale(xSec7ZH[toMassNo]/xSec7ZH[fromMassNo]);
     tempHistNom->Scale(xSec7ZH[toMassNo]/xSec7ZH[fromMassNo]);
  }  
 
  if(chanT.Contains("ZH") && IFILE.Contains("8TeV"))
  {
     tempHistUp->Scale(xSec8ZH[toMassNo]/xSec8ZH[fromMassNo]);
     tempHistDown->Scale(xSec8ZH[toMassNo]/xSec8ZH[fromMassNo]);
     tempHistNom->Scale(xSec8ZH[toMassNo]/xSec8ZH[fromMassNo]);
  }  
  
  std::cout<< "channel--> " << channel << std::endl;


  tempHistUp->SetLineColor(kRed);
  tempHistUp->SetLineWidth(3);
  tempHistUp->SetFillColor(0);
  
  tempHistDown->SetLineColor(kBlue);
  tempHistDown->SetFillColor(0);
  tempHistDown->SetLineWidth(3);
  
  tempHistNom->SetFillColor(0);
  tempHistNom->SetMarkerStyle(20);
    
  tempHistUp->SetTitle((channel + syst).c_str());

 if(addRightBin == 1)
    {
     float  err0 = tempHistNom->GetBinError(rightBinNo);
     float  con0 = tempHistNom->GetBinContent(rightBinNo);
 
     float  err1 = tempHistNom->GetBinError(rightBinNo-1);
     float  con1 = tempHistNom->GetBinContent(rightBinNo-1);
       
     tempHistNom->SetBinContent(rightBinNo,0);
     tempHistNom->SetBinError(rightBinNo,0);
     //  tempHistNom->SetBinContent(rightBinNo-1,con0+con1);
     // tempHistNom->SetBinError(rightBinNo-1,sqrt(err0*err0+err1*err1));


     err0 = tempHistUp->GetBinError(rightBinNo);
     con0 = tempHistUp->GetBinContent(rightBinNo);
 
     err1 = tempHistUp->GetBinError(rightBinNo-1);
     con1 = tempHistUp->GetBinContent(rightBinNo-1);
       
     tempHistUp->SetBinContent(rightBinNo,0);
     tempHistUp->SetBinError(rightBinNo,0);
     //   tempHistUp->SetBinContent(rightBinNo-1,con0+con1);
     // tempHistUp->SetBinError(rightBinNo-1,sqrt(err0*err0+err1*err1));



     err0 = tempHistDown->GetBinError(rightBinNo);
     con0 = tempHistDown->GetBinContent(rightBinNo);
 
     err1 = tempHistDown->GetBinError(rightBinNo-1);
     con1 = tempHistDown->GetBinContent(rightBinNo-1);
       
     tempHistDown->SetBinContent(rightBinNo,0);
     tempHistDown->SetBinError(rightBinNo,0);
     //   tempHistDown->SetBinContent(rightBinNo-1,con0+con1);
     // tempHistDown->SetBinError(rightBinNo-1,sqrt(err0*err0+err1*err1));

    }



 
  RooDataHist *DHnom;
  RooDataHist *DHup = new RooDataHist(nameUp.c_str(),"",*hobs,tempHistUp);  
  if(kount2 < 3) DHnom = new RooDataHist(namen.c_str(),"",*hobs,tempHistNom);
  RooDataHist *DHdown = new RooDataHist(nameDown.c_str(),"",*hobs,tempHistDown);  

  WS->import(*(new RooHistPdf(nameUp.c_str(),"",*hobs,*DHup)));
  WS->import(*(new RooHistPdf(nameDown.c_str(),"",*hobs,*DHdown)));
  if(kount2 < 3){ WS->import(*(new RooHistPdf(namen.c_str(),"",*hobs,*DHnom))); kount2++;}

 }


}






void Process(TString fname, TString oldFolder, int toMass, int fromMass)
{




  std::string channels[] = {"data_obs", "ZH", "TT",  "WjLF", "WjHF", "ZjLF", "ZjHF" , "VV" , "s_Top"};
  std::string systs[] = {"eff_b", "fake_b", "res_j", "scale_j" , "stat" };

  kount = 0;  

  gROOT->SetStyle("Plain");
  setTDRStyle();

  TFile * file = new TFile(fname.Data(), "READ");
  std::cout << "reading " << fname.Data() << std::endl;
  TString outname(massS[toMass]);
  outname.Append("_June8.root");
  fname.ReplaceAll(".root",outname.Data());
 
///BEGIN CHECK RBIN
  int addRightBin=0, rightBinNo=0;
  float a,overflow;
  RooWorkspace *mytempWS =  (RooWorkspace*) file->Get(oldFolder.Data());
  RooRealVar BDT("CMS_vhbb_BDT_Znn", "BDT", -1, 1);

  float Signal = 0;
  float Background = 0;
  float Data = 0;
  RooDataHist* tempRooDataHistNomS = (RooDataHist*)  mytempWS->data(channels[1].c_str());
  RooDataHist* tempRooDataHistNomD = (RooDataHist*)  mytempWS->data(channels[0].c_str());

  TH1 *tempHistNomS = tempRooDataHistNomS->createHistogram(channels[1].c_str(),BDT,RooFit::Binning(bins));

  TH1 *tempHistNomD = tempRooDataHistNomD->createHistogram(channels[0].c_str(),BDT,RooFit::Binning(bins));



  for(int i = 1; i <= tempHistNomS->GetNbinsX(); i++)
  { 
    std::cout << "############    signal in bin " << i << " is " << tempHistNomS->GetBinContent(i) << std::endl;
    std::cout << "############    data in bin " << i << " is " << tempHistNomD->GetBinContent(i) << std::endl;
    if(tempHistNomS->GetBinContent(i) > 0)
      { rightBinNo = i; Signal = tempHistNomS->GetBinContent(i); Data = tempHistNomD->GetBinContent(i) ;}

  }

  for(int i = 2; i < 9; i++)
  {
   RooDataHist* tempRooDataHistNom = (RooDataHist*)  mytempWS->data(channels[i].c_str());
   TH1 *tempHistNom = tempRooDataHistNom->createHistogram(channels[i].c_str(),BDT,RooFit::Binning(bins));
   Background += tempHistNom->GetBinContent(rightBinNo);
   
   overflow = tempHistNom->GetBinContent(rightBinNo+1) ;
   
   if(tempHistNom->GetBinContent(rightBinNo+1) > 0)
   {
   std::cout << "ARGHGHGHGHGH" << std::endl;
   //   std::cin >> ;
   }
   a+= overflow;
  }

  if( (Background ==0) ) addRightBin = 1;
else  addRightBin = 0;
std::cout << "################# folder" << oldFolder << std::endl;
 std::cout << "################# CHECK RBIN:: right bin n  " << rightBinNo << " signal: " << Signal << " bkg: " << Background <<   " Data:  " << Data <<   " at right there is an overflow of: "<< a << std::endl;
std::cout << "########################### CHECK RBIN:: REBINNING: " << addRightBin << std::endl;  
///END CHECK RBIN
 


  TFile * outfile = new TFile(fname.Data(), "RECREATE");
  using namespace RooFit;
  RooWorkspace *myWS = new RooWorkspace(oldFolder.Data(),oldFolder.Data());
  myWS->factory("CMS_vhbb_BDT_Znn[-1.,1.]"); ///NEW VARIABLE NAME HERE 

  TString oldFolder2(oldFolder.Data());
  oldFolder2.ReplaceAll("1","2");
  RooWorkspace *myWS2 = new RooWorkspace(oldFolder2.Data(),oldFolder2.Data());
  myWS2->factory("CMS_vhbb_BDT_Znn[-1.,1.]"); ///NEW VARIABLE NAME HERE 





///BEGIN CHECK RBIN
  int addRightBin2=0, rightBinNo2=0;
  float a2,overflow2;
  RooWorkspace *mytempWS2 =  (RooWorkspace*) file->Get(oldFolder2.Data());
   RooRealVar BDT2("CMS_vhbb_BDT_Znn", "BDT", -1, 1);

  float Signal2 = 0;
  float Background2 = 0;
  float Data2 = 0;
  RooDataHist* tempRooDataHistNomS2 = (RooDataHist*)  mytempWS2->data(channels[1].c_str());
  RooDataHist* tempRooDataHistNomD2 = (RooDataHist*)  mytempWS2->data(channels[0].c_str());

  TH1 *tempHistNomS2 = tempRooDataHistNomS2->createHistogram(channels[1].c_str(),BDT2,RooFit::Binning(bins));

  TH1 *tempHistNomD2 = tempRooDataHistNomD2->createHistogram(channels[0].c_str(),BDT2,RooFit::Binning(bins));



  for(int i = 1; i <= tempHistNomS2->GetNbinsX(); i++)
  { 
    std::cout << "############    signal in bin " << i << " is " << tempHistNomS2->GetBinContent(i) << std::endl;
    std::cout << "############    data in bin " << i << " is " << tempHistNomD2->GetBinContent(i) << std::endl;
    if(tempHistNomS2->GetBinContent(i) > 0)
      { rightBinNo2 = i; Signal2 = tempHistNomS2->GetBinContent(i); Data2 = tempHistNomD2->GetBinContent(i) ;}

  }

  for(int i = 2; i < 9; i++)
  {
   RooDataHist* tempRooDataHistNom2 = (RooDataHist*)  mytempWS2->data(channels[i].c_str());
   TH1 *tempHistNom2 = tempRooDataHistNom2->createHistogram(channels[i].c_str(),BDT2,RooFit::Binning(bins));
   Background2 += tempHistNom2->GetBinContent(rightBinNo2);
   
   overflow2 = tempHistNom2->GetBinContent(rightBinNo2+1) ;
   
   if(tempHistNom2->GetBinContent(rightBinNo2+1) > 0)
   {
   std::cout << "ARGHGHGHGHGH" << std::endl;
   //   std::cin >> ;
   }
   a2+= overflow2;
  }

  if( (Background2 ==0) ) addRightBin2 = 1;
else  addRightBin2 = 0;
std::cout << "################# folder" << oldFolder2 << std::endl;
 std::cout << "################# CHECK RBIN:: right bin n  " << rightBinNo2 << " signal: " << Signal2 << " bkg: " << Background2 <<   " Data:  " << Data2 <<   " at right there is an overflow of: "<< a2 << std::endl;
std::cout << "########################### CHECK RBIN:: REBINNING: " << addRightBin2 << std::endl;  
///END CHECK RBIN










  
  for (int c =0; c<9; c++)
  {
    kount2 = 0;  
    for (int s =0; s<5 ; s++ ){
      makeSystPlot( file, oldFolder, myWS,  channels[c], systs[s], toMass, fromMass, rightBinNo, addRightBin  );
      makeSystPlot( file, oldFolder2, myWS2,  channels[c], systs[s] , toMass, fromMass, rightBinNo2, addRightBin2 );
    }
  }


  
  makeSystPlot(file, oldFolder, myWS, "ZjLF", "ZJModel",toMass, fromMass, rightBinNo, addRightBin  );
  makeSystPlot(file, oldFolder, myWS, "ZjHF", "ZJModel",toMass, fromMass, rightBinNo, addRightBin  );


  makeSystPlot(file, oldFolder2, myWS2, "ZjLF", "ZJModel",toMass, fromMass, rightBinNo2, addRightBin2 );
  makeSystPlot(file, oldFolder2, myWS2, "ZjHF", "ZJModel",toMass, fromMass, rightBinNo2, addRightBin2 );

  myWS->writeToFile(fname.Data());  
  std::cout << std::endl << std::endl << std::endl << std::endl << "///////////////////////////" << std::endl;
  std::cout << fname.Data() << " written" << std::endl;
  std::cout << "///////////////////////////" << std::endl << std::endl << std::endl;


  outfile->Write();
  outfile->Close();
  fname.ReplaceAll("June8","June82");
  TFile * outfile2 = new TFile(fname.Data(), "RECREATE");
  myWS2->writeToFile(fname.Data());  
  std::cout << std::endl << std::endl << std::endl << std::endl << "///////////////////////////" << std::endl;
  std::cout << fname.Data() << " written" << std::endl;
  std::cout << "///////////////////////////" << std::endl << std::endl << std::endl;


}

  



void Rebin7TeVDataCardZnunuAdd()
{

maxCount=0;




for(int i = 0; i < n; i++)
{
  TString oldFolder;
  IFILE = files[i];
  oldFolder = "Znunu_1";

  if((IFILE.Contains("110")))
  {
     Process(IFILE, oldFolder, 0,0);
     Process(IFILE, oldFolder, 1,0);
     Process(IFILE, oldFolder, 2,0);
     Process(IFILE, oldFolder, 3,0);
     Process(IFILE, oldFolder, 4,0);
  }
  if((IFILE.Contains("115")))
  {
     for(int to = 5; to < 15; to++)   Process(IFILE, oldFolder, to , 10);
  }

  if((IFILE.Contains("120")))
  {
     for(int to = 15; to < 25; to++)   Process(IFILE, oldFolder,to , 20);
  }

  if((IFILE.Contains("125")))
  {
     for(int to = 25; to < 35; to++)   Process(IFILE, oldFolder,to , 30);
  }

  if((IFILE.Contains("130")))
  {
     for(int to = 35; to < 45; to++)   Process(IFILE, oldFolder,to , 40);
  }


  if((IFILE.Contains("135")))
  {
     for(int to = 45; to < 51; to++)   Process(IFILE, oldFolder,to , 50);
  }

  
  
}
}


