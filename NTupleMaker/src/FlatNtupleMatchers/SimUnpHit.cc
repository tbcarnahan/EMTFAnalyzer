
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleMatchers/SimUnpHit.h"

void SimUnpHit::Match( EMTFHitInfo & emtfHits, EMTFSimHitInfo & emtfSimHits ) {

  const int nHits    = ACCESS(emtfHits.mInts,    "nHits");
  const int nSimHits = ACCESS(emtfSimHits.mInts, "nSimHits");

  // Set to default values
  for (int i = 0; i < nHits; i++) {
    INSERT(emtfHits.mVInt, "hit_match_iSimHit", DINT);
  }
  for (int j = 0; j < nSimHits; j++) {
    INSERT(emtfSimHits.mVInt, "sim_hit_match_iHit", DINT);
  }

  // Pointer to hit and simHit IDs (integers)
  const std::map<TString, std::vector<int> > * iHit = &(emtfHits.mVInt);
  const std::map<TString, std::vector<int> > * jHit = &(emtfSimHits.mVInt);

  
  // Find matches between EMTF hits and simulated hits
  for (int i = 0; i < nHits; i++) {
    for (int j = 0; j < nSimHits; j++) {
      
      if ( ACCESS(*jHit, "sim_hit_isCSC").at(j)         == ACCESS(*iHit, "hit_isCSC").at(i)        &&
           ACCESS(*jHit, "sim_hit_isRPC").at(j)         == ACCESS(*iHit, "hit_isRPC").at(i)        &&
           ACCESS(*jHit, "sim_hit_BX").at(j)            == ACCESS(*iHit, "hit_BX").at(i)           &&
           ACCESS(*jHit, "sim_hit_endcap").at(j)        == ACCESS(*iHit, "hit_endcap").at(i)       &&
           ACCESS(*jHit, "sim_hit_sector").at(j)        == ACCESS(*iHit, "hit_sector").at(i)       &&
           ACCESS(*jHit, "sim_hit_sector_index").at(j)  == ACCESS(*iHit, "hit_sector_index").at(i) &&
           ACCESS(*jHit, "sim_hit_subsector").at(j)     == ACCESS(*iHit, "hit_subsector").at(i)    &&
           ACCESS(*jHit, "sim_hit_station").at(j)       == ACCESS(*iHit, "hit_station").at(i)      &&
           ACCESS(*jHit, "sim_hit_ring").at(j)          == ACCESS(*iHit, "hit_ring").at(i)         &&
           ACCESS(*jHit, "sim_hit_chamber").at(j)       == ACCESS(*iHit, "hit_chamber").at(i)      &&
           ( ( ACCESS(*jHit, "sim_hit_isCSC").at(j)                                            &&
               ACCESS(*jHit, "sim_hit_CSC_ID").at(j)    == ACCESS(*iHit, "hit_CSC_ID").at(i)   &&
               ACCESS(*jHit, "sim_hit_pattern").at(j)   == ACCESS(*iHit, "hit_pattern").at(i)  &&
               ACCESS(*jHit, "sim_hit_quality").at(j)   == ACCESS(*iHit, "hit_quality").at(i)  &&
               ACCESS(*jHit, "sim_hit_strip").at(j)     == ACCESS(*iHit, "hit_strip").at(i)    &&
               ACCESS(*jHit, "sim_hit_wire").at(j)      == ACCESS(*iHit, "hit_wire").at(i)   )   ||
             ( ACCESS(*jHit, "sim_hit_isRPC").at(j)                                            &&
               ACCESS(*jHit, "sim_hit_roll").at(j)      == ACCESS(*iHit, "hit_roll").at(i)     &&
               ACCESS(*jHit, "sim_hit_strip_hi").at(j)  == ACCESS(*iHit, "hit_strip_hi").at(i) &&
               ACCESS(*jHit, "sim_hit_strip_low").at(j) == ACCESS(*iHit, "hit_strip_low").at(i) ) ) ) {
	
	if ( ACCESS(*iHit, "hit_match_iSimHit").at(i) < 0 ) {
	  INSERT(emtfHits.mVInt, "hit_match_iSimHit", i, j);
	} else {
	  std::cout << "How can EMTF hit have two simulated matches?!?" << std::endl;
	  PrintHit(iHit, i);
	  PrintSimHit(jHit, j);
	  PrintSimHit(jHit, ACCESS(*iHit, "hit_match_iSimHit").at(i));
	}
	
	if ( ACCESS(*jHit, "sim_hit_match_iHit").at(j) < 0 ) {
	  INSERT(emtfSimHits.mVInt, "sim_hit_match_iHit", j, i);
	} else {
	  std::cout << "How can simulated EMTF hit have two matches?!?" << std::endl;
	  PrintSimHit(jHit, j);
	  PrintHit(iHit, i);
	  PrintHit(iHit, ACCESS(*jHit, "sim_hit_match_iHit").at(j));
	}
	
      } // End matching conditional

    } // End loop:  for (int j = 0; j < nSimHits; j++)
  } // End loop:  for (int i = 0; i < nHits; i++)

  
} // End function: void SimUnpHit::Match()
