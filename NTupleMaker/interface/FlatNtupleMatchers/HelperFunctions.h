
#ifndef FlatNtupleMatchersHelperFunctions_h
#define FlatNtupleMatchersHelperFunctions_h

#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/FlatNtupleBranches/EMTFSimHitInfo.h"

inline bool SameChamberHits( const std::map<TString, std::vector<int> > * iHit, const int i,
			     const std::map<TString, std::vector<int> > * jHit, const int j ) {

  return ( ACCESS(*jHit, "sim_hit_endcap").at(j)        == ACCESS(*iHit, "hit_endcap").at(i)       &&
	   ACCESS(*jHit, "sim_hit_sector").at(j)        == ACCESS(*iHit, "hit_sector").at(i)       &&
	   ACCESS(*jHit, "sim_hit_sector_index").at(j)  == ACCESS(*iHit, "hit_sector_index").at(i) &&
	   ACCESS(*jHit, "sim_hit_subsector").at(j)     == ACCESS(*iHit, "hit_subsector").at(i)    &&
	   ACCESS(*jHit, "sim_hit_station").at(j)       == ACCESS(*iHit, "hit_station").at(i)      &&
	   (ACCESS(*jHit, "sim_hit_ring").at(j) % 3)    == (ACCESS(*iHit, "hit_ring").at(i) % 3)   &&
	   ACCESS(*jHit, "sim_hit_CSC_ID").at(j)        == ACCESS(*iHit, "hit_CSC_ID").at(i)       &&
	   ACCESS(*jHit, "sim_hit_chamber").at(j)       == ACCESS(*iHit, "hit_chamber").at(i) );
}

inline bool MatchingHits( const std::map<TString, std::vector<int> > * iHit, const int i,
			  const std::map<TString, std::vector<int> > * jHit, const int j ) {
  
  if ( SameChamberHits(iHit, i, jHit, j)                                                       &&
       (ACCESS(*jHit, "sim_hit_isCSC").at(j) == 1) && (ACCESS(*iHit, "hit_isCSC").at(i) == 1)  &&
       abs(ACCESS(*jHit, "sim_hit_BX").at(j)        - ACCESS(*iHit, "hit_BX").at(i))       < 2 &&
       abs(ACCESS(*jHit, "sim_hit_strip").at(j)     - ACCESS(*iHit, "hit_strip").at(i))    < 2 &&
       abs(ACCESS(*jHit, "sim_hit_wire").at(j)      - ACCESS(*iHit, "hit_wire").at(i))     < 2 ) return true;

  if ( SameChamberHits(iHit, i, jHit, j)                                                        &&
       (ACCESS(*jHit, "sim_hit_isRPC").at(j) == 1) && (ACCESS(*iHit, "hit_isRPC").at(i) == 1)   &&
       abs(ACCESS(*jHit, "sim_hit_BX").at(j)        - ACCESS(*iHit, "hit_BX").at(i))        < 2 &&
       abs(ACCESS(*jHit, "sim_hit_theta_int").at(j) - ACCESS(*iHit, "hit_theta_int").at(i)) < 4 &&
       abs(ACCESS(*jHit, "sim_hit_phi_int").at(j)   - ACCESS(*iHit, "hit_phi_int").at(i))   < 4 ) return true;

  return false;
}

inline bool IdenticalHits( const std::map<TString, std::vector<int> > * iHit, const int i,
			   const std::map<TString, std::vector<int> > * jHit, const int j ) {
  
  if ( MatchingHits(iHit, i, jHit, j)                                                 &&
       ACCESS(*jHit, "sim_hit_BX").at(j)      == ACCESS(*iHit, "hit_BX").at(i)        &&
       ACCESS(*jHit, "sim_hit_strip").at(j)   == ACCESS(*iHit, "hit_strip").at(i)     &&
       ACCESS(*jHit, "sim_hit_wire").at(j)    == ACCESS(*iHit, "hit_wire").at(i)      &&
       ACCESS(*jHit, "sim_hit_quality").at(j) == ACCESS(*iHit, "hit_quality").at(i)   &&
       ACCESS(*jHit, "sim_hit_pattern").at(j) == ACCESS(*iHit, "hit_pattern").at(i)   ) return true;

  if ( MatchingHits(iHit, i, jHit, j)                                                   &&
       ACCESS(*jHit, "sim_hit_BX").at(j)        == ACCESS(*iHit, "hit_BX").at(i)        &&
       ACCESS(*jHit, "sim_hit_theta_int").at(j) == ACCESS(*iHit, "hit_theta_int").at(i) &&
       ACCESS(*jHit, "sim_hit_phi_int").at(j)   == ACCESS(*iHit, "hit_phi_int").at(i)   ) return true;

  return false;
}
       
inline int HitChamberID( const std::map<TString, std::vector<int> > * iHit, const int i ) {
  
  int ID = ( (ACCESS(*iHit, "hit_endcap").at(i) == 1) * 4 * 3 +
	     (ACCESS(*iHit, "hit_station").at(i) - 1)     * 3 +
	     (ACCESS(*iHit, "hit_ring").at(i) % 3) );

  int chamb = ACCESS(*iHit, "hit_chamber").at(i) - 1;

  if ( ACCESS(*iHit, "hit_neighbor").at(i) == 1) {
    if ( ACCESS(*iHit, "hit_isCSC").at(i) == 1) {
      chamb = 36 + (ACCESS(*iHit, "hit_sector").at(i) - 1) * 2 + (ACCESS(*iHit, "hit_subsector").at(i) == 2);
    }
    if ( ACCESS(*iHit, "hit_isRPC").at(i) == 1) {
      chamb = 36 + ACCESS(*iHit, "hit_sector").at(i) - 1;
    }
  }

  return ID * 48 + chamb;
}

inline int SimHitChamberID( const std::map<TString, std::vector<int> > * jHit, const int j ) {
  
  int ID = ( (ACCESS(*jHit, "sim_hit_endcap").at(j) == 1) * 4 * 3 +
	     (ACCESS(*jHit, "sim_hit_station").at(j) - 1)     * 3 +
	     (ACCESS(*jHit, "sim_hit_ring").at(j) % 3) );

  int chamb = ACCESS(*jHit, "sim_hit_chamber").at(j) - 1;

  if ( ACCESS(*jHit, "sim_hit_neighbor").at(j) == 1) {
    if ( ACCESS(*jHit, "sim_hit_isCSC").at(j) == 1) {
      chamb = 36 + (ACCESS(*jHit, "sim_hit_sector").at(j) - 1) * 2 + (ACCESS(*jHit, "sim_hit_subsector").at(j) == 2);
    }
    if ( ACCESS(*jHit, "sim_hit_isRPC").at(j) == 1) {
      chamb = 36 + ACCESS(*jHit, "sim_hit_sector").at(j) - 1;
    }
  }

  return ID * 48 + chamb;
}


#endif  // #ifndef FlatNtupleMatchersEMTFHitInfo_h
