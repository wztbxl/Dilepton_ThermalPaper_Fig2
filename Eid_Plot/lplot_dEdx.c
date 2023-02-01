// #include <stdio>
// #include <stdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include "TROOT.h"
#include "TH1.h"
#include "TGraph.h"
#include "draw.C"

class Bichsel;
Bichsel *m_Bichsel = 0;

const Int_t NMasses = 8;
const Double_t Masses[NMasses] = {0.51099906e-3, 0.13957, 0.493677, 0.9382723, 1.875613, 2.80925, 2.80923, 3.727417};
const Char_t *Names[NMasses] = {"e", "#pi", "K", "p", "d", "t", "^{3}He", "#Alpha"};
const Char_t *Filenames[NMasses] = {"Electron", "Pion", "Kaon", "Proton", "Deuteron", "Triton", "^{3}Helium", "^{4}Helium"};
const Int_t charge[NMasses] = {1,1,1, 1, 1, 1, 2, 2};

const Int_t NMasses2 = 5;
const Double_t Masses2[NMasses2] = {0.51099906e-3, 0.13957, 0.493677, 0.9382723, 1.875613};
const Char_t *Names2[NMasses2] = {"e", "#pi", "K", "p", "d"};
const Char_t *Filenames2[NMasses2] = {"Electron", "Pion", "Kaon", "Proton", "Deuteron"};
const Int_t charge2[NMasses2] = {1, 1, 1, 1, 1};

const Char_t *FNames[5] = {"Girrf","Sirrf","Bz","B70","B60"};
const Int_t Nlog2dx = 3;
const Double_t log2dx[Nlog2dx] = {0,1,2};
Int_t flag;

Double_t bichsel70M(Double_t *x,Double_t *par) {
	Double_t pove   = x[0];
	Double_t poverm = pove/par[0];
	Double_t charge = 1.; 
	if(par[1] > 1.0) {
		charge = 2;
		poverm *= charge;
	}
	//if(charge == 1)return charge*charge*m_Bichsel->GetI70M(TMath::Log10(poverm),1.); //(For dE/dx in KeV/cm)
	//if(charge == 2)return charge*charge*m_Bichsel->GetI70M(TMath::Log10(poverm),2.321928); //(For dE/dx in KeV/cm)
	if(charge == 1)return charge*charge*TMath::Exp(m_Bichsel->GetMostProbableZ(TMath::Log10(poverm), 1.0)); //(For dE/dx in KeV/cm)
	if(charge == 2)return charge*charge*TMath::Exp(m_Bichsel->GetMostProbableZ(TMath::Log10(poverm), 3.0)); //(For dE/dx in KeV/cm)
}

Double_t bichsel70M2(Double_t *x, Double_t *par){
	Double_t pove = -x[0];
	Double_t poverm = pove/par[0];
	Double_t charge = 1;
	if(par[1] > 1.0){
		charge = 2;
		poverm *= charge;
	}
	//if(charge ==1) return charge*charge*m_Bichsel->GetI70M(TMath::Log10(poverm), 1);
	//if(charge ==2) return charge*charge*m_Bichsel->GetI70M(TMath::Log10(poverm), 2.321928);
	if(charge == 1)return charge*charge*TMath::Exp(m_Bichsel->GetMostProbableZ(TMath::Log10(poverm), 1.0)); //(For dE/dx in KeV/cm)
	if(charge == 2)return charge*charge*TMath::Exp(m_Bichsel->GetMostProbableZ(TMath::Log10(poverm), 3.0)); //(For dE/dx in KeV/cm)

}

void lplot_dEdx(){

	gStyle->SetOptStat(00000);
	gStyle->SetOptTitle(0);
	gStyle->SetOptFit(101);
	gStyle->SetStatX(0.86);
	gStyle->SetStatY(0.94);
	gStyle->SetStatW(0.15);
	gStyle->SetStatH(0.15);

	gStyle->SetPalette(55);

	ofstream out[NMasses],out1[NMasses];
	out[0].open("P10data/e.txt");
	out[1].open("P10data/pi.txt");
	out[2].open("P10data/K.txt");
	out[3].open("P10data/p.txt");
	out[4].open("P10data/d.txt");
	out[5].open("P10data/t.txt");
	out[6].open("P10data/he3.txt");
	out[7].open("P10data/alpha.txt");
	out1[0].open("P10data/dEdx_e.h");
	out1[1].open("P10data/dEdx_pi.h");
	out1[2].open("P10data/dEdx_K.h");
	out1[3].open("P10data/dEdx_p.h");
	out1[4].open("P10data/dEdx_d.h");
	out1[5].open("P10data/dEdx_t.h");
	out1[6].open("P10data/dEdx_he3.h");
	out1[7].open("P10data/dEdx_alpha.h");
	TCanvas *c1 = new TCanvas("c1","c1",0,0,750,600);
	c1->SetLogz();
	c1->SetFillColor(10);
	c1->SetFrameFillColor(10);
	c1->SetBorderMode(0);
	c1->SetBorderSize(2);
	c1->SetFrameBorderMode(0);
	c1->SetLeftMargin(0.15);
	c1->SetLeftMargin(0.15);
	c1->SetRightMargin(0.15);
	//c1->SetTopMargin(0.02);
	c1->SetBottomMargin(0.15);

	TPad *pad = new TPad("pad", "pad",0,0,1,1);
	//pad->SetBorderMode(0);
	pad->SetFillColor(0);
	pad->SetTopMargin(0.06);
	pad->SetBottomMargin(0.12);
	pad->SetLeftMargin(0.14);
	pad->SetRightMargin(0.06);
	pad->SetBorderMode(0);
	pad->SetTickx();
	//	 pad->SetLogx();
	pad->SetLogy();
	pad->SetTicky();
	pad->SetGrid(0,0);
	//pad->SetFrameLineWidth(2);
	pad->Draw();
	pad->cd();
	//ploa band
	//TFile *infile = new TFile("data/7gev_qa.root");
    TFile *infile = new TFile("data/isobar_run_QA.root");
	TH2F *f26=(TH2F*)infile->Get("hdedxvsp");
    gPad->SetLogz();// get logz
	f26->SetStats(0);
	f26->GetXaxis()->SetMoreLogLabels();
	f26->GetYaxis()->SetMoreLogLabels();
	f26->GetYaxis()->SetNdivisions(609);
	f26->GetYaxis()->SetRangeUser(1.5,100);
	f26->GetXaxis()->SetRangeUser(-3.2, 3.2);
	f26->GetXaxis()->SetTitle("Rigidity p/q (GeV/c)");
	f26->GetXaxis()->SetTitleFont(42);
	f26->GetXaxis()->CenterTitle();
	f26->GetYaxis()->SetTitle("dE/dx in TPC (keV/cm)");
	f26->GetYaxis()->SetTitleFont(42);
	f26->GetYaxis()->CenterTitle();
	f26->GetXaxis()->SetTitleSize(0.06);
	f26->GetXaxis()->SetTitleOffset(0.8);
	f26->GetYaxis()->SetTitleSize(0.06);
	f26->GetYaxis()->SetTitleOffset(0.7);
	f26->GetYaxis()->SetLabelSize(0.0);
	f26->GetXaxis()->SetLabelFont(42);
	f26->Draw("col");

	drawText(-3.38, 1.88, "2",  42, 0.04, 0);
	drawText(-3.38, 2.81, "3",  42, 0.04, 0);
	drawText(-3.38, 4.71, "5",  42, 0.04, 0);
	drawText(-3.50, 9.51, "10",  42, 0.04, 0);
	drawText(-3.50, 28.6, "30",  42, 0.04, 0);

	//plot Bichsel functions
	if (gClassTable->GetID("StBichsel") < 0 || !m_Bichsel) {
		gSystem->Load("libTable");
		gSystem->Load("St_base");
		gSystem->Load("StarClassLibrary");
		gSystem->Load("StBichsel");
		m_Bichsel = new Bichsel();
	}

	leg1 = new TLegend(0.18,0.5,0.3,0.83);
	leg1->SetTextAlign(12);
	leg1->SetTextFont(132);
	leg1->SetTextSize(0.03);
	leg1->SetFillColor(kWhite);
	leg1->SetFillStyle(1111);
	leg1->SetLineColor(0);
	leg1->Draw("same");

	drawText(-2.9, 34, "Isobar 200 GeV", 42, 0.04, 0);
	tex1 = new TLegend(0.18, 0.85, 0.4, 0.9, "Au+Au 3 GeV");
	tex1->SetTextFont(42);
	tex1->SetTextSize(0.04);
	tex1->SetFillColor(kWhite);
	tex1->SetLineColor(0);
	//tex1->Draw("same");
	//for(int h=0; h < NMasses; h++){
	for(int h=0; h < 7; h++){
		Int_t funId = 3;
		Int_t dx = 1;

		Char_t *FunName = Form("%s%s%i",FNames[funId],Names[h],(int)log2dx[dx]);
		cout << "Make \t" << FunName << endl;


		TF1 *func = new TF1(FunName, bichsel70M, 0.15,20 , 2);
		func->SetParameter(0,Masses[h]);//par[0]=>mass
		func->SetParameter(1,1.);//par[1]=>charge
		if(h >= 6)func->SetParameter(1,2.);//Set charge for He++
		func->SetLineWidth(2);
		func->SetLineStyle(9);
		func->Draw("same"); 
		Int_t color;
		//if(h==0) color=kBlack;//e
		if(h==0) color=kViolet+1;//pi
		if(h==1) color=kBlue+1;//K
		if(h==2) color=kCyan+1;//p
		if(h==3) color=kGreen;//d
		if(h==4) color=kYellow-6;//t
		if(h==5) color=kOrange+1;//3He
		if(h==6) color=kRed+1;
		//if(h==7) color=kRed+1;
		func->SetLineColor(color);
		out1[h]<<"Float_t dEdx_"<<Filenames[h]<<"[19990] = {"<<endl;
		// for(Int_t npt=100;npt<20000;npt++)	{    
		// 	//cout<<npt*0.001<<'\t'<<func->Eval(npt*0.001)<<endl;
		// 	out[h]<<npt*0.001<<'\t'<<func->Eval(npt*0.001)<<endl;
		// 	//cout << -1*npt*0.001<<'\t'<<func->Eval(-1*npt*0.001)<<endl;
		// 	out1[h]<<func->Eval(npt*0.001);
		// 	if(npt!=19999)out1[h]<<",";
		// 	out1[h]<<endl;}
		// out1[h]<<"};";
		leg1->AddEntry(func,Filenames[h], "l");
	}
	for (int h = 0; h < NMasses2; h++) { // Masses
		Int_t funId = 3;
		Int_t dx = 1;

		Char_t *FunName = Form("%s%s%i",FNames[funId],Names2[h],(int)log2dx[dx]);
		cout << "Make \t" << FunName << endl;
		TF1 *func2 = new TF1(FunName, bichsel70M2, -20,-0.15 , 2);
		func2->SetParameter(0,Masses2[h]);//par[0]=>mass
		func2->SetParameter(1,1.);//par[1]=>charge
		//if(h >= 6)func2->SetParameter(1,2.);//Set charge for He++
		//if(h==0) color=kBlack;//e
		if(h==0) color=kViolet+1;//pi
		if(h==1) color=kBlue+1;//K
		if(h==2) color=kCyan+1;//p
		if(h==3) color=kGreen;//d
		if(h==4) color=kYellow-6;//t
		func2->SetLineWidth(2);
		func2->SetLineColor(color);
		func2->SetLineStyle(9);
		func2->Draw("same");
	}
	//pad->Print("~/protected/19gev/QA/DeDx/7gevDedx.pdf");
	//pad->Print("~/protected/19gev/QA/DeDx/7gevDedx.gif");
	//pad->Print("~/protected/19gev/QA/DeDx/7gevDedx.eps");
    pad->Print("isobarDedx.pdf");
    pad->Print("isobarDedx.eps");
}

