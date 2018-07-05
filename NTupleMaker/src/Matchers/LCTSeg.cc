// Code to match EMTF LCTs to CSC reconstructed segments
// Written by Chad Freer March 27, 2018

// Will use EMTFHit and CSCSegment collections and check matching using
// time, endcap, station, ring, and chamber information.

// Once we have narrowed down to same chambers will use rechit strip and wire position
// to find if they are in the same physical location and at the same time

#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/LCTSeg.h"

void LCTSeg::Match(CSCSegInfo & cscSegs, EMTFHitInfo & emtfHits) {

  // Prepare variables for loops over segments/LCTs
  const int n1 = ACCESS(cscSegs.mInts,  "nSegs");
  const int n2 = ACCESS(emtfHits.mInts, "nHits");
  
  // std::cout << "nsegs: " << n1 << ", nLCTs: " << n2 << std::endl;

  for (int i = 0; i < n1; i++) { // Loop over segments
    for (int j = 0; j < n2; j++) { // Loop over LCTs

      if (ACCESS(emtfHits.mVInt, "hit_isCSC").at(j) != 1) continue; // Skip RPC hits
      if (ACCESS(cscSegs.mVInt, "seg_strip_max").at(i) - ACCESS(cscSegs.mVInt, "seg_strip_min").at(i) > 24) continue; // Skip anomalous huge segments
      if ( fabs( ACCESS(cscSegs.mVFlt, "seg_time").at(i) - ACCESS(emtfHits.mVInt, "hit_BX").at(j) * 25. ) > 37.5 ) continue; // Require matching within 1 BX

      // Check to see if the segment and LCT are in the same endcap (EMTFHit: endcap --> +/-1, CSCDetId: endcap --> +/-1)
      int seg_endcap = ACCESS(cscSegs.mVInt,  "seg_endcap").at(i);
      int LCT_endcap = ACCESS(emtfHits.mVInt, "hit_endcap").at(j);
      if (seg_endcap != LCT_endcap) continue;

      // Check to see if the segment and LCT are in the same station (EMTFHit: station --> 1-4, CSCDetId: station --> 1-4)
      int seg_station = ACCESS(cscSegs.mVInt,  "seg_station").at(i);
      int LCT_station = ACCESS(emtfHits.mVInt, "hit_station").at(j);
      if (seg_station != LCT_station) continue;

      // Check to see if the segment and LCT are in the same Ring (EMTFHit: ring --> 1-4, CSCDetId: ring --> 1-4)
      int seg_ring = ACCESS(cscSegs.mVInt,  "seg_ring").at(i);
      int LCT_ring = ACCESS(emtfHits.mVInt, "hit_ring").at(j);
      if (seg_ring != LCT_ring) continue;

      // Check to see if the segment and LCT are in the same Chamber (EMTFHit: chamber --> 1-36, CSCDetId: chamber --> 1-36)
      int seg_chamber = ACCESS(cscSegs.mVInt,  "seg_chamber").at(i);
      int LCT_chamber = ACCESS(emtfHits.mVInt, "hit_chamber").at(j);
      if (seg_chamber != LCT_chamber) continue;
 
      // Grab segment strip and wire information 
      int Seg_strip_min = ACCESS(cscSegs.mVInt, "seg_strip_min").at(i) * 2; // Convert to units of halfstrips
      int Seg_strip_max = ACCESS(cscSegs.mVInt, "seg_strip_max").at(i) * 2; // Convert to units of halfstrips
      int Seg_wire_min  = ACCESS(cscSegs.mVInt, "seg_wire_min").at(i);
      int Seg_wire_max  = ACCESS(cscSegs.mVInt, "seg_wire_max").at(i);

      float Seg_strip_avg = (Seg_strip_min + Seg_strip_max) / 2.;
      float Seg_wire_avg  = (Seg_wire_min  + Seg_wire_max)  / 2.;

      // Now find the LCT strip and wire information for potential matches
      int LCT_strip = ACCESS(emtfHits.mVInt, "hit_strip").at(j) + 2; // Shift one full strip     
      int LCT_wire  = ACCESS(emtfHits.mVInt, "hit_wire").at(j)  + 1; // Shifted because LCT starts counting at 0

      // // Nice for printouts
      // int nRecHits = ACCESS(cscSegs.mVInt, "seg_nRecHits").at(i);
      // std::cout << "\nnumber of Rechits:" << nRecHits   << std::endl;
      // std::cout << "Segment strip:" << Seg_strip_min << "-" << Seg_strip_max << "     LCT strip:" << LCT_strip  << std::endl;
      // std::cout << "Segment wire:" << Seg_wire_min << "-" << Seg_wire_max << "        LCT wire:" << LCT_wire  << "\n" << std::endl;

      // Now start the meat of matching code: check to see if it is within 1 strip and wire
      if (LCT_strip < Seg_strip_min - 1 || LCT_strip > Seg_strip_max + 1) continue;
      if (LCT_wire  < Seg_wire_min  - 1 || LCT_wire  > Seg_wire_max  + 1) continue;

      // std::cout << "We have a full match, segment index " << i << " to LCT index " << j << "!" << std::endl;

      if ( ACCESS(emtfHits.mVInt, "hit_neighbor").at(j) == 1 ) { // Only set LCT to match segment, not segment to match LCT
	INSERT(emtfHits.mVInt, "hit_match_iSeg",  j, i);
	INSERT(emtfHits.mVInt, "hit_match_nSegs", j, ACCESS(emtfHits.mVInt, "hit_match_nSegs").at(j) + 1);
	continue;
      }

      assert( (ACCESS(cscSegs.mVInt,  "seg_match_nHits").at(i) >= 1) == (ACCESS(cscSegs.mVInt,  "seg_match_iHit").at(i) >= 0) );
      assert( (ACCESS(emtfHits.mVInt, "hit_match_nSegs").at(j) >= 1) == (ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j) >= 0) );
      
      INSERT(cscSegs.mVInt,  "seg_match_nHits", i, ACCESS(cscSegs.mVInt,  "seg_match_nHits").at(i) + 1);
      INSERT(emtfHits.mVInt, "hit_match_nSegs", j, ACCESS(emtfHits.mVInt, "hit_match_nSegs").at(j) + 1);

      int l = ACCESS(cscSegs.mVInt,  "seg_match_iHit").at(i); // Old matched hit index
      int k = ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j); // Old matched segment index

      assert(l != j);
      assert(k != i);


      // If no double matching then we save the indices
      if (l < 0) INSERT(cscSegs.mVInt,  "seg_match_iHit", i, j); // Sets i^th value in vector to j
      if (k < 0) INSERT(emtfHits.mVInt, "hit_match_iSeg", j, i); // Sets j^th value in vector to i

      // Check for segments matched to two LCTs
      if (l >= 0) {
	// std::cout << "But segment already had a match with index " << ACCESS(cscSegs.mVInt, "seg_match_iHit").at(i) << "!!!" << std::endl;
	int LCT_strip_l = ACCESS(emtfHits.mVInt, "hit_strip").at(l) + 2; // Old strip
	int LCT_wire_l  = ACCESS(emtfHits.mVInt, "hit_wire").at(l)  + 1; // Old wire

	// Match both LCTs to the segment index but the segment index only to the closer LCT
	if ( abs(LCT_strip - Seg_strip_avg) < abs(LCT_strip_l - Seg_strip_avg) ) {
	  INSERT(cscSegs.mVInt,  "seg_match_iHit",  i, j); // Sets the index to new LCT index
	  INSERT(cscSegs.mVInt,  "seg_match_iHit2", i, l); // Sets the second index to old LCT index
	}
	else if ( abs(LCT_strip - Seg_strip_avg) > abs(LCT_strip_l - Seg_strip_avg) ) {
	  INSERT(cscSegs.mVInt,  "seg_match_iHit2", i, j); // Sets the second index to new LCT index
	}
	else if ( abs(LCT_wire - Seg_wire_avg) < abs(LCT_wire_l - Seg_wire_avg) ) {
	  INSERT(cscSegs.mVInt,  "seg_match_iHit",  i, j); // Sets the index to new LCT index
	  INSERT(cscSegs.mVInt,  "seg_match_iHit2", i, l); // Sets the second index to old LCT index
	}
	else if ( abs(LCT_wire - Seg_wire_avg) > abs(LCT_wire_l - Seg_wire_avg) ) {
	  INSERT(cscSegs.mVInt,  "seg_match_iHit2", i, j); // Sets the second index to new LCT index
	}
	else {
	  // std::cout << "\nBizzare case in CSC segment matching: two distinct LCTs, same distance from the segment!" << std::endl;
	  // PrintSeg(&(cscSegs.mVInt), &(cscSegs.mVFlt), i);
	  // PrintHit(&(emtfHits.mVInt), l);
	  // PrintHit(&(emtfHits.mVInt), j);
	  // std::cout << "dStrip_l = "  << LCT_strip_l << " - " <<  Seg_strip_avg << " = " << (LCT_strip_l - Seg_strip_avg)
	  // 	    << ", dStrip = "  << LCT_strip   << " - " <<  Seg_strip_avg << " = " << (LCT_strip - Seg_strip_avg)
	  // 	    << ", dWire_l = " << LCT_wire_l  << " - " <<  Seg_wire_avg  << " = " << (LCT_wire_l - Seg_wire_avg)
	  // 	    << ", dWire = "   << LCT_wire    << " - " <<  Seg_wire_avg  << " = " << (LCT_wire - Seg_wire_avg) << std::endl;
	  INSERT(cscSegs.mVInt,  "seg_match_iHit2", i, j); // Sets the second index to new LCT index
	}
      }


      // Check for LCTs matched to two segments
      if ( k >= 0 ) {
	// std::cout << "But LCT already had a match with index " << ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j) << "!!!" << std::endl;
        
	// Check which segment has the most rechits and keep that one
	if (ACCESS(cscSegs.mVInt, "seg_nRecHits").at(i) > (ACCESS(cscSegs.mVInt, "seg_nRecHits").at(k))) {
	  INSERT(emtfHits.mVInt, "hit_match_iSeg",  j, i);  // Sets j^th value in vector to i
	  INSERT(emtfHits.mVInt, "hit_match_iSeg2", j, k);  // Sets j^th value in vector to k
	}
	else if (ACCESS(cscSegs.mVInt, "seg_nRecHits").at(i) < (ACCESS(cscSegs.mVInt, "seg_nRecHits").at(k))) {
	  INSERT(emtfHits.mVInt, "hit_match_iSeg2", j, i);  // Sets j^th value in vector to i
	}
	else if (ACCESS(cscSegs.mVFlt, "seg_chi2").at(i) < (ACCESS(cscSegs.mVFlt, "seg_chi2").at(k))) {
	  INSERT(emtfHits.mVInt, "hit_match_iSeg",  j, i);  // Sets j^th value in vector to i
	  INSERT(emtfHits.mVInt, "hit_match_iSeg2", j, k);  // Sets j^th value in vector to k
	}
	else if (ACCESS(cscSegs.mVFlt, "seg_chi2").at(i) > (ACCESS(cscSegs.mVFlt, "seg_chi2").at(k))) {
	  INSERT(emtfHits.mVInt, "hit_match_iSeg2", j, i);  // Sets j^th value in vector to i
	}
	else {
	  std::cout << "\nBizzare case in LCT matching: two distinct segements with the same nRecHits and Chi2!" << std::endl;
	  std::cout << "Hit index " << j << ", old segment index " << k << ", new segment index " << i << std::endl;
	  PrintHit(&(emtfHits.mVInt), j);
	  PrintSeg(&(cscSegs.mVInt), &(cscSegs.mVFlt), k);
	  PrintSeg(&(cscSegs.mVInt), &(cscSegs.mVFlt), i);
	  INSERT(emtfHits.mVInt, "hit_match_iSeg2", j, i); // Sets the second index to new segment index
	}
      }
      

      assert( (ACCESS(cscSegs.mVInt,  "seg_match_nHits").at(i) >= 2) == (ACCESS(cscSegs.mVInt,  "seg_match_iHit2").at(i) >= 0) );
      assert( (ACCESS(emtfHits.mVInt, "hit_match_nSegs").at(j) >= 2) == (ACCESS(emtfHits.mVInt, "hit_match_iSeg2").at(j) >= 0) );

      assert( (ACCESS(cscSegs.mVInt,  "seg_match_nHits").at(i) == 0) ||
	      (ACCESS(cscSegs.mVInt,  "seg_match_iHit2").at(i) != ACCESS(cscSegs.mVInt, "seg_match_iHit").at(i)) );
      assert( (ACCESS(emtfHits.mVInt, "hit_match_nSegs").at(j) == 0) ||
	      (ACCESS(emtfHits.mVInt, "hit_match_iSeg2").at(j) != ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j)) );

    } // End j loop (over LCTs)
  } // End i loop (over segments)

  for (int i = 0; i < n1; i++) { // Loop over segments
    int j = ACCESS(cscSegs.mVInt, "seg_match_iHit").at(i);
    if (j > 0 && ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j) == i) {
      INSERT(cscSegs.mVInt, "seg_hit_match_unique", i, 1);
    }
  }

  for (int j = 0; j < n2; j++) { // Loop over LCTs
    int i = ACCESS(emtfHits.mVInt, "hit_match_iSeg").at(j);
    if (i > 0 && ACCESS(cscSegs.mVInt, "seg_match_iHit").at(i) == j) {
      INSERT(emtfHits.mVInt, "hit_seg_match_unique", j, 1);
    }
  }

  
} // End Fill
