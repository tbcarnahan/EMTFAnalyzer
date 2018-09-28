
// Contains all the branches in the NTuples
// Make sure it's up to date with the NTuple
#include "EMTFAnalyzer/NTupleMaker/interface/Read_FlatNtuple.h"
// #include "EMTFAnalyzer/NTupleMaker/interface/Read_FlatNtuple_Slim.h"

const bool verbose = false;
const int MAX_EVT = 1000;    // Max number of events to process
const int PRT_EVT = 100; // Print every N events

// RECO muon eta ranges
const std::vector<float> eta_min = {-2.4, -2.1, 1.7, 2.1};
const std::vector<float> eta_max = {-2.1, -1.7, 2.1, 2.4};
// EMTF mode thresholds
const std::vector<int> mode_cuts = {8};
// EMTF pT thresholds
const std::vector<int> pt_cuts = {0};

const TString in_dir = "/afs/cern.ch/work/c/cfreer/public";

// Different files to compare (each one gets separate plots)
std::vector<TString> in_file_names = { "EMTF_ZMu_NTuple_322492_2018_test.root" };

const TString out_dir = "plots";

const int phi_bins[3] = {36, -180, 180};

const float BIT = 0.001;  // Small shift for filling inside bin edges

TString STR(int x) { return std::to_string(abs(x)); }
TString STR(float x) { 
  std::ostringstream out;
  out << std::setprecision(2) << x;
  return out.str();
}


// Main function to make efficiency vs. phi plots
void LCT_efficiency_ME11(std::vector<TString> _in_file_names = {}, TString _label = "") {

  if (_in_file_names.size() > 0) in_file_names = _in_file_names;
   
  std::vector<TChain*> in_chains;
  // List of input files
  for (const auto & file_name : in_file_names) {
    in_chains.push_back( new TChain("FlatNtupleData/tree") );
    in_chains.back()->Add(in_dir+"/"+file_name);
  }

  std::vector<TH1F*> h_phi_num;  // Efficiency vs. phi numerator
  std::vector<TH1F*> h_phi_den;  // Efficiency vs. phi denominator
  std::vector<TEfficiency*> h_phi_eff;  // Efficiency vs. phi

  // Book histograms
  for (int i = 0; i < in_chains.size(); i++) {
    for (int j = 0; j < eta_min.size(); j++) {
      for (const auto & mode_cut : mode_cuts) {
	for (const auto & pt_cut : pt_cuts) {

	  TString h_name = "file_"+STR(i)+"_pos_eta_"+STR(int(eta_min.at(j)*100.1))+"_to_"+STR(int(eta_max.at(j)*100.1))+"_mode_"+STR(mode_cut)+"_pt_"+STR(pt_cut);
	  if (eta_min.at(j) < 0) h_name = h_name.ReplaceAll("_pos_", "_neg_");
	  TString h_title = "from file "+STR(i)+", "+STR(eta_min.at(j))+" < |#eta| < "+STR(eta_max.at(j))+", mode #geq "+STR(mode_cut)+", p_{T} #geq "+STR(pt_cut);
	  
	  h_phi_num.push_back( new TH1F( "h_phi_num_"+h_name, "RECO pT "+h_title+" (numerator)",   phi_bins[0], phi_bins[1], phi_bins[2] ) );
	  h_phi_den.push_back( new TH1F( "h_phi_den_"+h_name, "RECO pT "+h_title+" (denominator)", phi_bins[0], phi_bins[1], phi_bins[2] ) );

	} // End loop: for (const auto & pt_cut : pt_cuts)
      } // End loop: for (const auto & mode_cut : mode_cuts)
    } // End loop: for(int j = 0; j < eta_min.size(); j++)
  } // End loop: for (int i = 0; i < in_chains.size(); i++)

  // Number of histograms under each category
  int hChain = pt_cuts.size() * mode_cuts.size() * eta_min.size();
  int hEta   = pt_cuts.size() * mode_cuts.size();
  int hMode  = pt_cuts.size();


  // Loop over events in each chain
  for (int iChain = 0; iChain < in_chains.size(); iChain++) {

    TChain * in_chain = in_chains.at(iChain);
    int nEvents = (MAX_EVT > 0 ? MAX_EVT : in_chain->GetEntries());

    std::cout << "\n******* About to loop over " << nEvents << " events in file " << iChain << " *******" << std::endl;
    
    InitializeMaps();
    SetBranchAddresses(in_chain);
    std::cout << "Successfully set branch addresses" << std::endl;

    for (int iEvt = 0; iEvt < nEvents; iEvt++) {
      if ( (iEvt % PRT_EVT) == 0 ) {
	std::cout << "\n*************************************" << std::endl;
	std::cout << "Looking at event " << iEvt << " out of " << nEvents << std::endl;
	std::cout << "*************************************" << std::endl;
      }

      in_chain->GetEntry(iEvt);
    
      // From Read_FlatNtuple.h, use 'I("branch_name")' to get an integer branch value, 'F("branch_name") to get a float
      if (verbose) std::cout << "\n" << I("nRecoMuons") << " RECO muons in the event" << std::endl;
	
      // Skip events without HLT fired
      if ( I("nRecoMuonsTrig") == 0 ) continue;
	
      // Loop over RECO muons
      for (int iMu = 0; iMu < I("nRecoMuons"); iMu++) {   

	// Require muon to pass tight ID
	if ( I("reco_ID_tight", iMu) != 1 ) continue;
	// Require muon to be a "probe", i.e. at least one other muon fires the HLT
	if ( I("nRecoMuonsTrig") < 2 && I("reco_trig_ID", iMu) > 0 ) continue;

	for (int iEta = 0; iEta < eta_min.size(); iEta++) {
	  for (int iMode = 0; iMode < mode_cuts.size(); iMode++) {
	    for (int iPt = 0; iPt < pt_cuts.size(); iPt++) {

	      int iHist = (iChain * hChain) + (iEta * hEta) + (iMode * hMode) + iPt;

	      // Fill all RECO muons in eta range
	      if ( F("reco_eta_St1", iMu) > eta_min.at(iEta) && 
		   F("reco_eta_St1", iMu) < eta_max.at(iEta) ) {

		h_phi_den.at(iHist)->Fill( F("reco_phi_St1", iMu) );

		int iTrk = I("reco_dR_match_unp_iTrk", iMu);
		if (iTrk < 0) continue;

		// Fill only RECO muons matched to EMTF track passing time, mode, and pT cuts
		if ( I("unp_trk_BX",   iTrk) == 0 && 
		     I("unp_trk_mode", iTrk) >= mode_cuts.at(iMode) &&
		     F("unp_trk_pt",   iTrk) >= pt_cuts.at(iPt) ) {

		  h_phi_num.at(iHist)->Fill( F("reco_phi_St1", iMu) );

		} // End if matched EMTF track passes cuts

	      } // End if RECO muon falls in eta range

	    } // End loop: for (int iPt = 0; iPt < pt_cuts.size(); iPt++)
	  } // End loop: for (int iMode = 0; iMode < mode_cuts.size(); iMode++)
	} // End loop: for (int iEta = 0; iEta < eta_min.size(); iEta++)

      } // End loop: for (int iMu = 0; iMu < I("nRecoMuons"); iMu++) {   
    } // End loop: for (int iEvt = 0; iEvt < nEvents; iEvt++) {
  } // End loop: for (const int iChain = 0; iChain < in_chains.size(); iChain++)

  std::cout << "\n******* Finished looping over events in all " << in_chains.size() << " files *******" << std::endl;

  TFile out_file(out_dir+"/LCT_efficiency_ME11"+_label+".root", "RECREATE");
  out_file.cd();

  for (int i = 0; i < h_phi_num.size(); i++) {

    TString h_name  = (TString(h_phi_den.at(i)->GetName())).ReplaceAll("_den_", "_eff_");
    TString h_title = (TString(h_phi_den.at(i)->GetTitle())).ReplaceAll("denominator", "efficiency");
    h_phi_eff.push_back( new TEfficiency( (*h_phi_num.at(i)), (*h_phi_den.at(i)) ) );
    h_phi_eff.at(i)->SetName (h_name);
    h_phi_eff.at(i)->SetTitle(h_title);
    h_phi_eff.at(i)->Write();

    h_phi_num.at(i)->SetLineWidth(2);
    h_phi_den.at(i)->SetLineWidth(2);
    h_phi_eff.at(i)->SetLineWidth(2);

    int nHists = eta_min.size() * mode_cuts.size() * pt_cuts.size();

    h_phi_num.at(i)->SetLineColor(4); // kBlue
    h_phi_den.at(i)->SetLineColor(1); // kBlack
    h_phi_eff.at(i)->SetLineColor( 2 + (i/nHists) ); // kRed, kGreen, kBlue ...

    h_phi_num.at(i)->Write();
    h_phi_den.at(i)->Write();
    h_phi_eff.at(i)->Write();

  }

  out_file.Close();
  
} // End function: void 
