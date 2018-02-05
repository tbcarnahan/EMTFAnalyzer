#ifndef RecoMuonDataFormat_h
#define RecoMuonDataFormat_h

//-------------------------------------------------------------------------------
// Adapted from cmssw/L1Trigger/L1TNtuples/interface/L1AnalysisRecoMuon2DataFormat.h
// Wei Shi
//-------------------------------------------------------------------------------

#include <vector>

  struct RecoMuonDataFormat
  {
    RecoMuonDataFormat(){Reset();};
    RecoMuonDataFormat(){Reset();};

    void Reset()
    {
    nMuons=0;

    pt.clear();
    eta.clear();
    phi.clear();
    isLooseMuon.clear();
    isMediumMuon.clear();
    isTightMuon.clear();
    charge.clear();
    etaSt1.clear();
    phiSt1.clear();
    etaSt2.clear();
    phiSt2.clear();
    }

    unsigned short nMuons;
    std::vector<float> pt;
    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<bool> isLooseMuon;
    std::vector<bool> isMediumMuon;
    std::vector<bool> isTightMuon;
    std::vector<int> charge;
    std::vector<float> etaSt1;
    std::vector<float> phiSt1;
    std::vector<float> etaSt2;
    std::vector<float> phiSt2;
  };
}
#endif
