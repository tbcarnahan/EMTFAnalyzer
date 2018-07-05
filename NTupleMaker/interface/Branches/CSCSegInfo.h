
#ifndef BranchesCSCSegInfo_h
#define BranchesCSCSegInfo_h

// Common branch info
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/Common.h"

// Helpful tools
#include "EMTFAnalyzer/NTupleMaker/interface/HelperFunctions.h"

// CSC segment classes
#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"

// CSC segment global geometry
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"

/////////////////////////////////
///  CSC segment information  ///
/////////////////////////////////
struct CSCSegInfo {
  std::vector<TString> ints = {{"nSegs", "nSegsBX0"}};
  std::vector<TString> vFlt = {{"seg_chi2", "seg_time", "seg_eta", "seg_theta", "seg_phi", "seg_globX", "seg_globY", "seg_globZ",
				"seg_locX", "seg_locY", "seg_dirX", "seg_dirY", "seg_dirZ", "seg_bend_theta", "seg_bend_phi"}};
  std::vector<TString> vInt = {{"seg_endcap", "seg_ring", "seg_station", "seg_chamber", "seg_sector", "seg_CSC_ID",
				"seg_nRecHits", "seg_wire_max", "seg_wire_min", "seg_strip_max", "seg_strip_min",
				"seg_match_iHit", "seg_match_iHit2", "seg_match_nHits", "seg_hit_match_unique"}};
  std::map<TString, int> mInts;
  std::map<TString, std::vector<float> > mVFlt;
  std::map<TString, std::vector<int> > mVInt;

  void Initialize();
  void Reset();
  inline void CheckSize() { CHECKSIZE(mVFlt); CHECKSIZE(mVInt); }
  void Fill(const CSCSegment cscSeg, edm::ESHandle<CSCGeometry> cscGeom);
};

inline void PrintSeg( const std::map<TString, std::vector<int> > *   iSeg,
		      const std::map<TString, std::vector<float> > * fSeg, const int i) {

  std::cout << "* CSCSeg time " << ACCESS(*fSeg, "seg_time").at(i)
	    << ", endcap " << ACCESS(*iSeg, "seg_endcap").at(i) << ", sector " << ACCESS(*iSeg, "seg_sector").at(i)
	    << ", station " << ACCESS(*iSeg, "seg_station").at(i) << ", ring " << ACCESS(*iSeg, "seg_ring").at(i)
	    << ", CSC ID " << ACCESS(*iSeg, "seg_CSC_ID").at(i) << ", chamber " << ACCESS(*iSeg, "seg_chamber").at(i)
	    << ", nRecHits " << ACCESS(*iSeg, "seg_nRecHits").at(i) << ", chi2 " << ACCESS(*fSeg, "seg_chi2").at(i)
	    << ", strips " << ACCESS(*iSeg, "seg_strip_min").at(i) << " - " << ACCESS(*iSeg, "seg_strip_max").at(i)
	    << ", wires " << ACCESS(*iSeg, "seg_wire_min").at(i) << " - " << ACCESS(*iSeg, "seg_wire_max").at(i)
	    << ", eta " << ACCESS(*fSeg, "seg_eta").at(i) << ", phi " << ACCESS(*fSeg, "seg_phi").at(i) << std::endl;

}

#endif  // #ifndef BranchesCSCSegInfo_h
