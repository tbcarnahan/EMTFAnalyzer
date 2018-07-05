
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/SimUnpHit.h"

void SimUnpHit::Match( EMTFHitInfo & emtfHits, EMTFSimHitInfo & emtfSimHits ) {

  const int nHits    = ACCESS(emtfHits.mInts,    "nHits");
  const int nSimHits = ACCESS(emtfSimHits.mInts, "nSimHits");

  // Pointer to hit and simHit IDs (integers)
  std::map<TString, std::vector<int> > * iHit = &(emtfHits.mVInt);    // Can't be "const" because we modify emtfHits below
  std::map<TString, std::vector<int> > * jHit = &(emtfSimHits.mVInt); // Can't be "const" because we modify emtfSimHits below

  // Arrays storing the number of hits per BX in each chamber
  int nCSCs[9][1152]    = {{0}};
  int nSimCSCs[9][1152] = {{0}};
  int nRPCs[9][1152]    = {{0}};
  int nSimRPCs[9][1152] = {{0}};

  // Count CSC and RPC hits per BX, endcap, station, ring, and chamber
  for (int i = 0; i < nHits; i++) {
    if ( ACCESS(*iHit, "hit_isCSC").at(i) )
      nCSCs [ACCESS(*iHit, "hit_BX").at(i) + 4] [HitChamberID(iHit, i)] += 1;
    if ( ACCESS(*iHit, "hit_isRPC").at(i) )
      nRPCs [ACCESS(*iHit, "hit_BX").at(i) + 4] [HitChamberID(iHit, i)] += 1;
  }
  for (int j = 0; j < nSimHits; j++) {
    if ( ACCESS(*jHit, "sim_hit_isCSC").at(j) )
      nSimCSCs [ACCESS(*jHit, "sim_hit_BX").at(j) + 4] [SimHitChamberID(jHit, j)] += 1;
    if ( ACCESS(*jHit, "sim_hit_isRPC").at(j) )
      nSimRPCs [ACCESS(*jHit, "sim_hit_BX").at(j) + 4] [SimHitChamberID(jHit, j)] += 1;
  }

  // Find matches between EMTF hits and simulated hits
  for (int i = 0; i < nHits; i++) {
    for (int j = 0; j < nSimHits; j++) {

      bool foundMatch = false;

      int hitBX    = ACCESS(*iHit, "hit_BX").at(i);
      int simHitBX = ACCESS(*jHit, "sim_hit_BX").at(j);

      int hitID    = HitChamberID(iHit, i);
      int simHitID = SimHitChamberID(jHit, j);

      // To be a poissible match, must be of the same type (CSC or RPC), in the same chamber, and within 1 BX
      // SameChamberHits, MatchingHits, and IdenticalHits defined in interface/Matchers/HelperFunctions.h
      if (ACCESS(*jHit, "sim_hit_isCSC").at(j) != ACCESS(*iHit, "hit_isCSC").at(i)) continue;
      if (ACCESS(*jHit, "sim_hit_isRPC").at(j) != ACCESS(*iHit, "hit_isRPC").at(i)) continue;
      if (SameChamberHits(iHit, i, jHit, j) == false) continue;
      if (abs(simHitBX - hitBX) > 1) continue;

      // Matching based on nearby strip/wire or theta/phi
      if (MatchingHits(iHit, i, jHit, j)) {
	foundMatch = true;
      }
      // Matching if these are the only possible matches in the chamber within +/1 BX
      else if ( ACCESS(*iHit, "hit_isCSC").at(i) == 1 ) {

	if ( (nSimCSCs[hitBX + 3][hitID]    + nSimCSCs[hitBX + 4][hitID]    + nSimCSCs[hitBX + 5][hitID])    == 1 &&
	     (nCSCs[simHitBX + 3][simHitID] + nCSCs[simHitBX + 4][simHitID] + nCSCs[simHitBX + 5][simHitID]) == 1 ) foundMatch = true;
      }
      else if ( ACCESS(*iHit, "hit_isRPC").at(i) == 1 ) {

	if ( (nSimRPCs[hitBX + 3][hitID]    + nSimRPCs[hitBX + 4][hitID]    + nSimRPCs[hitBX + 5][hitID])    == 1 &&
	     (nRPCs[simHitBX + 3][simHitID] + nRPCs[simHitBX + 4][simHitID] + nRPCs[simHitBX + 5][simHitID]) == 1 ) foundMatch = true;
      }

      // Fill hit and sim_hit with matching information
      if (foundMatch) {

	// New sim_hit is only match or better match
	if ( ( ACCESS(*iHit, "hit_match_iSimHit")  .at(i)  < 0 ) ||
	     ( ACCESS(*iHit, "hit_sim_match_exact").at(i) == 0 && IdenticalHits(iHit, i, jHit, j) ) ) {
	  INSERT(emtfHits.mVInt, "hit_match_iSimHit",   i, j);
	  INSERT(emtfHits.mVInt, "hit_sim_match_exact", i, IdenticalHits(iHit, i, jHit, j));
	}
	// Two sim_hits equally well matched
	else if ( ACCESS(*iHit, "hit_sim_match_exact").at(i) == IdenticalHits(iHit, i, jHit, j) ) {
	  std::cout << "\nHow can EMTF hit have two simulated matches?!?" << std::endl;
	  std::cout << "nCSCs in BX " << hitBX - 1 << "/" << hitBX << "/" << hitBX + 1
		    << " = " << nCSCs[hitBX + 3][hitID]
		    << "/"   << nCSCs[hitBX + 4][hitID]
		    << "/"   << nCSCs[hitBX + 5][hitID] << std::endl;
	  std::cout << "nSimCSCs in BX " << hitBX - 1 << "/" << hitBX << "/" << hitBX + 1
		    << " = " << nSimCSCs[hitBX + 3][hitID]
		    << "/"   << nSimCSCs[hitBX + 4][hitID]
		    << "/"   << nSimCSCs[hitBX + 5][hitID] << std::endl;
	  std::cout << "nRPCs in BX " << hitBX - 1 << "/" << hitBX << "/" << hitBX + 1
		    << " = " << nRPCs[hitBX + 3][hitID]
		    << "/"   << nRPCs[hitBX + 4][hitID]
		    << "/"   << nRPCs[hitBX + 5][hitID] << std::endl;
	  std::cout << "nSimRPCs in BX " << hitBX - 1 << "/" << hitBX << "/" << hitBX + 1
		    << " = " << nSimRPCs[hitBX + 3][hitID]
		    << "/"   << nSimRPCs[hitBX + 4][hitID]
		    << "/"   << nSimRPCs[hitBX + 5][hitID] << std::endl;
	  PrintHit(iHit, i);
	  PrintSimHit(jHit, j);
	  PrintSimHit(jHit, ACCESS(*iHit, "hit_match_iSimHit").at(i));
	  std::cout << "(Chamber IDs = " << hitID << ", " << simHitID << ", " << SimHitChamberID(jHit, ACCESS(*iHit, "hit_match_iSimHit").at(i)) << ")\n" << std::endl;
	}

	// New hit is only match or better match
	if ( ( ACCESS(*jHit, "sim_hit_match_iHit") .at(j)  < 0 ) ||
	     ( ACCESS(*jHit, "sim_hit_match_exact").at(j) == 0 && IdenticalHits(iHit, i, jHit, j) ) ) {
	  INSERT(emtfSimHits.mVInt, "sim_hit_match_iHit",  j, i);
	  INSERT(emtfSimHits.mVInt, "sim_hit_match_exact", j, IdenticalHits(iHit, i, jHit, j));
	}
	// Two hits are equally well matched
	else if ( ACCESS(*jHit, "sim_hit_match_exact").at(j) == IdenticalHits(iHit, i, jHit, j) ) {
	  std::cout << "\nHow can simulated EMTF hit have two matches?!?" << std::endl;
	  std::cout << "nCSCs in BX " << simHitBX - 1 << "/" << simHitBX << "/" << simHitBX + 1
		    << " = " << nCSCs[simHitBX + 3][simHitID]
		    << "/"   << nCSCs[simHitBX + 4][simHitID]
		    << "/"   << nCSCs[simHitBX + 5][simHitID] << std::endl;
	  std::cout << "nSimCSCs in BX " << simHitBX - 1 << "/" << simHitBX << "/" << simHitBX + 1
		    << " = " << nSimCSCs[simHitBX + 3][simHitID]
		    << "/"   << nSimCSCs[simHitBX + 4][simHitID]
		    << "/"   << nSimCSCs[simHitBX + 5][simHitID] << std::endl;
	  std::cout << "nRPCs in BX " << simHitBX - 1 << "/" << simHitBX << "/" << simHitBX + 1
		    << " = " << nRPCs[simHitBX + 3][simHitID]
		    << "/"   << nRPCs[simHitBX + 4][simHitID]
		    << "/"   << nRPCs[simHitBX + 5][simHitID] << std::endl;
	  std::cout << "nSimRPCs in BX " << simHitBX - 1 << "/" << simHitBX << "/" << simHitBX + 1
		    << " = " << nSimRPCs[simHitBX + 3][simHitID]
		    << "/"   << nSimRPCs[simHitBX + 4][simHitID]
		    << "/"   << nSimRPCs[simHitBX + 5][simHitID] << std::endl;
	  PrintSimHit(jHit, j);
	  PrintHit(iHit, i);
	  PrintHit(iHit, ACCESS(*jHit, "sim_hit_match_iHit").at(j));
	  std::cout << "(Chamber IDs = " << simHitID << ", " << hitID << ", " << HitChamberID(iHit, ACCESS(*jHit, "sim_hit_match_iHit").at(j)) << ")\n" << std::endl;
	}

      } // End bool if (foundMatch)

    } // End loop:  for (int j = 0; j < nSimHits; j++)
  } // End loop:  for (int i = 0; i < nHits; i++)

} // End function: void SimUnpHit::Match()
