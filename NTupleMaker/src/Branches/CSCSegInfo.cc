// Code to add segment information to branch
// Written by Chad Freer, March, 2018
//
// Uses CSCDetIds for location in detector,
// segments for location and direction in chamber
// and rechits for strip and wire information

#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCSegInfo.h"

void CSCSegInfo::Initialize() {
  for (auto & str : ints) mInts.insert( std::pair<TString, int>(str, DINT) );
  for (auto & str : vFlt) mVFlt.insert( std::pair<TString, std::vector<float> >(str, DVFLT) );
  for (auto & str : vInt) mVInt.insert( std::pair<TString, std::vector<int> >  (str, DVINT) );
}

void CSCSegInfo::Reset() {
  for (auto & it : mInts) it.second = DINT;
  for (auto & it : mVFlt) it.second.clear();
  for (auto & it : mVInt) it.second.clear();
  INSERT(mInts, "nSegs", 0);
  INSERT(mInts, "nSegsBX0", 0);
}

void CSCSegInfo::Fill(const CSCSegment cscSeg, edm::ESHandle<CSCGeometry> cscGeom) {
  // Set CSCDetId to get position of segment in detector
  CSCDetId id  = (CSCDetId)(cscSeg).cscDetId();

  // Get CSC segment global position from CSC geometry
  // See https://github.com/aminnj/CSCValidationRunning/blob/me42/RecoLocalMuon/CSCValidation/src/CSCValidation.cc#L1153
  LocalPoint locPos = (cscSeg).localPosition();
  const CSCChamber * cscChamb = cscGeom->chamber(id);
  GlobalPoint globPos = cscChamb->toGlobal(locPos);
  
  float chi2 =       (cscSeg).chi2();
  float time =       (cscSeg).time();
  float eta =        globPos.eta();
  float theta =      globPos.theta() * 180. / M_PI;
  if (theta > 90) theta = 180 - theta;
  float phi =        globPos.phi() * 180. / M_PI;
  float globX =      globPos.x();
  float globY =      globPos.y();
  float globZ =      globPos.z();
  float locX =       locPos.x();
  float locY =       locPos.y();
  float dirX =       (cscSeg).localDirection().x();
  float dirY =       (cscSeg).localDirection().y();
  float dirZ =       (cscSeg).localDirection().z();
  int nRecHits =     (cscSeg).nRecHits();

  float bend_phi   = TMath::ATan2( dirX, abs(dirZ) );
  float bend_theta = TMath::ATan2( dirY, abs(dirZ) );

  int endcap =  id.zendcap();
  int ring =    id.ring();
  int station = id.station();
  int chamber = id.chamber();
  int sector =  id.triggerSector();
  int CSC_ID =  id.triggerCscId();

  int wire_max  = -1;
  int wire_min  = 1000;
  int strip_max = -1;
  int strip_min = 1000;
  const std::vector<CSCRecHit2D> recHits = cscSeg.specificRecHits();

  for (int iHit = 0; iHit < cscSeg.nRecHits(); iHit++) {
    const CSCRecHit2D recHit = recHits.at(iHit);
    if (wire_max < recHit.hitWire()) wire_max = recHit.hitWire();
    if (wire_min > recHit.hitWire()) wire_min = recHit.hitWire();
    for (int i = 0; i < 3; i++) {
      if (strip_max < recHit.channels(i)) strip_max = recHit.channels(i);
      if (strip_min > recHit.channels(i)) strip_min = recHit.channels(i);
    }
  }

  int nDuplicates = 0;
  for (int i = 0; i < ACCESS(mInts, "nSegs"); i++) {

    if ( endcap  == ACCESS(mVInt, "seg_endcap").at(i)  &&
	 ring    == ACCESS(mVInt, "seg_ring").at(i)    &&
	 station == ACCESS(mVInt, "seg_station").at(i) &&
	 chamber == ACCESS(mVInt, "seg_chamber").at(i) &&
	 // Total overlap in strip and wire range (one contained inside the other, in both strip and wire)
	 ( ( (strip_min  >= ACCESS(mVInt, "seg_strip_min").at(i) &&
	      strip_max  <= ACCESS(mVInt, "seg_strip_max").at(i)) ||
	     (strip_min  <= ACCESS(mVInt, "seg_strip_min").at(i) &&
	      strip_max  >= ACCESS(mVInt, "seg_strip_max").at(i)) ) &&
	   ( (wire_min   >= ACCESS(mVInt, "seg_wire_min").at(i) &&
	      wire_max   <= ACCESS(mVInt, "seg_wire_max").at(i)) ||
	     (wire_min   <= ACCESS(mVInt, "seg_wire_min").at(i) &&
	      wire_max   >= ACCESS(mVInt, "seg_wire_max").at(i)) ) ) ) {
      // std::cout << "\nSegment with index " << ACCESS(mInts, "nSegs") << " identical to segment with index " << i << " - skipping one" << std::endl;
      // PrintSeg(&(mVInt), &(mVFlt), i);
      // std::cout << "* CSCSeg time " << time << ", endcap " << endcap << ", sector " << sector << ", station " << station << ", ring " << ring
      // 		<< ", CSC ID " << CSC_ID << ", chamber " << chamber << ", nRecHits " << nRecHits << ", chi2 " << chi2 << ", strips " << strip_min
      // 		<< " - " << strip_max << ", wires " << wire_min << " - " << wire_max << ", eta " << eta << ", phi " << phi << "\n" << std::endl;
      if ( ( nRecHits  > ACCESS(mVInt, "seg_nRecHits").at(i) ) ||
	   ( nRecHits == ACCESS(mVInt, "seg_nRecHits").at(i) && chi2 < ACCESS(mVFlt, "seg_chi2").at(i) + 0.001 ) ) {
	// std::cout << "LATER DUPLICATE SEGMENT APPEARS TO BE BETTER QUALITY!!!" << std::endl;
	DELETE(mVInt, i);
	DELETE(mVFlt, i);
	INSERT(mInts, "nSegs", ACCESS(mInts, "nSegs") - 1);
	if (abs(time) < 12.5) {
	  INSERT(mInts, "nSegsBX0", ACCESS(mInts, "nSegsBX0") - 1);
	}
      } else {
	nDuplicates += 1;
      }
    }
  }
  // if (nDuplicates > 1) std::cout << "Found " << nDuplicates << " duplicate segments" << std::endl;
  if (nDuplicates > 0) return;

  INSERT(mInts, "nSegs", ACCESS(mInts, "nSegs") + 1);
  if (abs(time) < 12.5) {
    INSERT(mInts, "nSegsBX0", ACCESS(mInts, "nSegsBX0") + 1);
  }

  INSERT(mVFlt, "seg_chi2",       chi2 );
  INSERT(mVFlt, "seg_time",       time );
  INSERT(mVFlt, "seg_eta",        eta );
  INSERT(mVFlt, "seg_theta",      theta );
  INSERT(mVFlt, "seg_phi",        phi );
  INSERT(mVFlt, "seg_globX",      globX );
  INSERT(mVFlt, "seg_globY",      globY );
  INSERT(mVFlt, "seg_globZ",      globZ );
  INSERT(mVFlt, "seg_locX",       locX );
  INSERT(mVFlt, "seg_locY",       locY );
  INSERT(mVFlt, "seg_dirX",       dirX );
  INSERT(mVFlt, "seg_dirY",       dirY );
  INSERT(mVFlt, "seg_dirZ",       dirZ );
  INSERT(mVFlt, "seg_bend_phi",   bend_phi );
  INSERT(mVFlt, "seg_bend_theta", bend_theta );
  INSERT(mVInt, "seg_nRecHits",   nRecHits );

  INSERT(mVInt, "seg_endcap",   endcap );
  INSERT(mVInt, "seg_ring",     ring );
  INSERT(mVInt, "seg_station",  station );
  INSERT(mVInt, "seg_chamber",  chamber );
  INSERT(mVInt, "seg_sector",   sector );
  INSERT(mVInt, "seg_CSC_ID",   CSC_ID );

  INSERT(mVInt, "seg_wire_max",  wire_max );
  INSERT(mVInt, "seg_wire_min",  wire_min );
  INSERT(mVInt, "seg_strip_max", strip_max );
  INSERT(mVInt, "seg_strip_min", strip_min );

  INSERT(mVInt, "seg_match_iHit",       DINT );
  INSERT(mVInt, "seg_match_iHit2",      DINT );
  INSERT(mVInt, "seg_match_nHits",      0 );
  INSERT(mVInt, "seg_hit_match_unique", 0 );

} // End function: CSCSegInfo::Fill(const l1t::CSCSeg & cscSeg)
