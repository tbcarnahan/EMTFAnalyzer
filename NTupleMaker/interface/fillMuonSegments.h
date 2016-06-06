//=============================================================
// Fill all REco Muons in the event record for:
// Package:    NTupleMaker
// Class:      NTupleMaker
//
// Written by David Curry
// ============================================================

#include <iostream>
#include <fstream>
#include <set>
#include <cmath>
#include <vector>
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"

#include "../plugins/SegmentLCTMatchBox.h"



using namespace std;
using namespace edm;
using namespace reco;



void fillSegmentsMuons(DataEvtSummary_t &ev, 
		       edm::Handle<reco::MuonCollection> muons, 
		       edm::Handle<CSCSegmentCollection> cscSegs, 
		       edm::ESHandle<CSCGeometry> cscGeom,
		       const edm::Handle<CSCCorrelatedLCTDigiCollection> CSCTFlcts,
		       int printLevel) {


  // class container for the segment and number of hits matching the global muon
  class Segment {
  public:
    const CSCSegment cscsegcand;
    int nMatchedHits;
  Segment(const CSCSegment &seg, int nMHits):cscsegcand(seg), nMatchedHits(nMHits){};
  };
  
  typedef std::vector<Segment*> SegmentVector;
  
  // return a vector containing the segments associated to the muon
  //SegmentVector* SegmentsInMuon(const reco::Muon* muon, const CSCSegmentCollection* segments, edm::ESHandle<CSCGeometry> cscGeom, int printLevel);

  
  // importing directly Ivan's object...
  SegmentLCTMatchBox _matchBox;
  
  _matchBox.setPrintLevel(printLevel);
  
  // given a muon candidate, loop over the segments and
  // find the segments belonging to the muon and see
  // if they are "LCTAble" (i.e. could generate an LCT)

  int whichMuon = 0;
  for (auto muon = muons->begin(); muon != muons->end(); muon++, whichMuon++) {
    
    if (whichMuon > (MAX_MUONS-1) ) {
      cout << "the muon has " << whichMuon << ", but the MAX allowed is "
           << MAX_MUONS << " -> Skipping the muon... " << endl;
      continue;
    }

    // Only Global Muons that were also filled in the event record and has standalone component
    if (!muon->combinedMuon().isNonnull() || !muon->isGlobalMuon() || !muon->isStandAloneMuon()) continue;

    TrackRef trackRef = muon->combinedMuon();

    if (printLevel > 4) {
      printf("************************************************\n");
      printf("M U O N  #%d\n", whichMuon);
      printf("************************************************\n\n");
      printf("%s\n"    , "--------------------------------");
      printf("%s: %d\n", "isGlobalMuon    ()", muon->isGlobalMuon    ());
      printf("%s: %d\n", "isTrackerMuon   ()", muon->isTrackerMuon   ());
      printf("%s: %d\n", "isStandAloneMuon()", muon->isStandAloneMuon());
      printf("%s: %d\n", "combinedMuon    ().isNonnull()", muon->combinedMuon  ().isNonnull());
      printf("%s: %d\n", "track           ().isNonnull()", muon->track         ().isNonnull());
      printf("%s: %d\n", "standAloneMuon  ().isNonnull()", muon->standAloneMuon().isNonnull());
      printf("%s\n\n"  , "--------------------------------");
      
      printf("(GBL) muon->pt(): %10.5f, muon->eta(): %10.5f, muon->phi(): %10.5f\n",
	     trackRef->pt(), trackRef->eta(), trackRef->phi());
    }
    
    // Only fill for known CSC eta range
    if ( abs(trackRef->eta()) < 1.1 || abs(trackRef->eta()) >2.4) continue;
    
    // get the segments which match the muon candidate.  Comes from class defined below
    //SegmentVector *segVect = SegmentsInMuon( &(*muon), &(*cscSegs), cscGeom, printLevel);
    
    SegmentVector *segVect = new SegmentVector();

    bool isMuonStd=false; // has the muon a standalone component
    if (muon->combinedMuon().isNonnull() || muon->standAloneMuon().isNonnull())
      isMuonStd=true;
    
    // return empty vector if the muon is not interesting
    if (!isMuonStd) continue;
    int nMuonMatchedHits=0;
    int icscSegment=0;
    
    // --------- Loop over the CSC segments -------------
    for (auto segIter = cscSegs->begin(); segIter != cscSegs->end(); segIter++){
      
      int nHits=segIter -> nRecHits();
      
      if (printLevel > 4) {
	cout << " ======================================== " << endl;
	cout << "Segment in CSC:" << icscSegment++ << endl;
	cout << "# segs hits:"    << nHits      << endl;
      }

      if (icscSegment > MAX_SEGS_STD-1) continue;
	
      
      const std::vector<CSCRecHit2D>& theHits = segIter -> specificRecHits();
      std::vector<CSCRecHit2D>::const_iterator hitIter;
      
      int iHit=0;
      // loop over the segments hits
      for (hitIter = theHits.begin(); hitIter!= theHits.end(); hitIter++){
	
	if (printLevel>2) std::cout << "iHit:" << iHit++ << ", ";
	
	// check if the hit will match the standalone muon component
	bool isHitMatched=false;
	
	LocalPoint seghitlocal = hitIter -> localPosition();
	
	double segHitX = seghitlocal.x();
	double segHitY = seghitlocal.y();
	
	if (printLevel > 4)
	  std::cout << "segHitX="<<segHitX <<  ", "
		    << "segHitY="<<segHitY;
	
	// The muon now returns segments (2012), while in 2010 it was returning hits...
	for(trackingRecHit_iterator segm  = muon->outerTrack()->recHitsBegin();
	    segm != muon->outerTrack()->recHitsEnd();
	    segm++){
	  
	  // some basic protection
	  if ( !((*segm)->isValid()) ) continue;
	  
	  // Hardware ID of the RecHit (in terms of wire/strips/chambers)
	  DetId detid = (*segm)->geographicalId();
	  
	  // Interested in muon systems only
	  if( detid.det() != DetId::Muon ) continue;
	  
	  //Look only at CSC Hits (CSC id is 2)
	  if (detid.subdetId() != MuonSubdetId::CSC) continue;
	  
	  CSCDetId id(detid.rawId());
	  
	  // another sanity check
	  if  (id.station() < 1) continue;
	  
	  // get the CSCSegment
	  const CSCSegment* cscSegment = dynamic_cast<const CSCSegment*>(&**segm);
	  // check the segment is not NULL
	  if (cscSegment == NULL) continue;
	  
	  // try to get the CSC recHits that contribute to this segment.
	  std::vector<CSCRecHit2D> theseRecHits = (*cscSegment).specificRecHits();
	  
	  // loop over the rechits
	  for ( std::vector<CSCRecHit2D>::const_iterator iRH = theseRecHits.begin();
		iRH != theseRecHits.end(); iRH++) {
	    
	    // get the rechit ID
	    CSCDetId idRH = (CSCDetId)(*iRH).cscDetId();
	    
	    // CSC chamber
	    const CSCChamber* cscchamber = cscGeom->chamber(idRH);
	    if (!cscchamber) continue;
	    
	    // local position
	    LocalPoint rhitlocal = iRH->localPosition();
	    
	    if (segHitX==rhitlocal.x() &&
		segHitY==rhitlocal.y()  )
	      isHitMatched=true;
	  } // end loop over hits of a segment
	  
	} // end loop trackingRecHit_iterator (segments of a muon)
	
	if (printLevel > 4 ){
	  if (isHitMatched) cout<< " -> Matched" << endl;
	  else              cout<< " -> NOT Matched" << endl;
	}
	
	if (isHitMatched) nMuonMatchedHits++;
	
      }
      
      if (printLevel > 4) cout<< "segment has "  << nMuonMatchedHits
			      << " hits out of " << nHits
			      << " matched"      << endl;
      
      // fill the the vector with the matching segments
      if (nMuonMatchedHits!=0) {
	Segment* segment = new Segment(*segIter, nMuonMatchedHits);
	segVect -> push_back(segment);
      }
      
    }  // end loop on the CSC segments
    
    
    if (printLevel > 4)
      cout << "The muon has " << segVect -> size()    << " csc segments"     << endl;
    
    ev.recoNumCscSegs -> push_back(segVect->size());

    // sanity check
    if (segVect -> size() == 0) {
      delete segVect;
      continue;
    }
    
    if (printLevel > 4) cout << "Looping over the CSC segments\n\n";

    int iSegment = 0;
    for (auto segmentCSC = segVect -> begin(); segmentCSC != segVect->end(); segmentCSC++, iSegment++){
      

      if (printLevel > 4) {
        printf("#############\n");
        printf("Segment  #%d\n", iSegment);
        printf("#############\n\n");
	

	if ( iSegment > (MAX_SEGS_STD-1) ) {
          cout << "the muon has " << iSegment+1 << ", but the MAX allowed is "
	       << MAX_SEGS_STD << " -> Skipping the segment... " << endl;
          continue;
        }
      }
      
      // basic info
      CSCDetId id  = (CSCDetId) (*segmentCSC)->cscsegcand.cscDetId();
      const CSCChamber* cscchamber = cscGeom->chamber(id);

      if (!cscchamber) {
        cout << "cscchamber not valid" << endl;
        continue;
      }
      
      ev.recoCscSeg_sector [whichMuon][iSegment] = CSCTriggerNumbering::triggerSectorFromLabels(id);;
      ev.recoCscSeg_endcap [whichMuon][iSegment] = id.endcap();
      ev.recoCscSeg_station[whichMuon][iSegment] = id.station();
      ev.recoCscSeg_ring   [whichMuon][iSegment] = id.ring();
      ev.recoCscSeg_chamber[whichMuon][iSegment] = id.chamber();
      ev.recoCscSeg_nHits  [whichMuon][iSegment] = (*segmentCSC)->cscsegcand.nRecHits();
      
      if (printLevel > 4) {
	std::cout << "Endcap  = " << ev.recoCscSeg_endcap [whichMuon][iSegment] << std::endl;
	std::cout << "Ring    = " << ev.recoCscSeg_station[whichMuon][iSegment] << std::endl;
	std::cout << "Station = " << ev.recoCscSeg_ring   [whichMuon][iSegment] << std::endl;
	std::cout << "Chamber = " << ev.recoCscSeg_chamber[whichMuon][iSegment] << std::endl;
	std::cout << "nRecHits= " << ev.recoCscSeg_nHits  [whichMuon][iSegment] << std::endl << std::endl;
      }

      
      // local segment position
      LocalPoint localPos = (*segmentCSC)->cscsegcand.localPosition();
      LocalVector  segDir = (*segmentCSC)->cscsegcand.localDirection();
      
      /*
      recoCscSeg_loc_x      [whichMuon][iSegment] = localPos.x();
      recoCscSeg_loc_y      [whichMuon][iSegment] = localPos.y();
      recoCscSeg_loc_eta    [whichMuon][iSegment] = localPos.eta();
      recoCscSeg_loc_phi    [whichMuon][iSegment] = localPos.phi();
      recoCscSeg_loc_dir_eta[whichMuon][iSegment] = segDir.eta();
      recoCscSeg_loc_dir_phi[whichMuon][iSegment] = segDir.phi();

      if (printLevel > 2) {
	std::cout << "localPos.x()   = " << recoCscSeg_loc_x      [whichMuon][iSegment] << " cm"<< std::endl;
	std::cout << "localPos.y()   = " << recoCscSeg_loc_y      [whichMuon][iSegment] << " cm"<< std::endl;
	std::cout << "localPos.eta() = " << recoCscSeg_loc_eta    [whichMuon][iSegment] << std::endl;
	std::cout << "localPos.phi() = " << recoCscSeg_loc_phi    [whichMuon][iSegment] << std::endl;
	std::cout << "segDir.eta()   = " << recoCscSeg_loc_dir_eta[whichMuon][iSegment] << std::endl;
	std::cout << "segDir.phi()   = " << recoCscSeg_loc_dir_phi[whichMuon][iSegment] << std::endl << std::endl;
      }
      */

      // global segment position
      GlobalPoint globalPosition   = cscchamber->toGlobal(localPos);
      GlobalVector globalDirection = cscchamber->toGlobal(segDir);

      ev.recoCscSeg_glob_x      [whichMuon][iSegment] = globalPosition.x();
      ev.recoCscSeg_glob_y      [whichMuon][iSegment] = globalPosition.y();
      ev.recoCscSeg_glob_eta    [whichMuon][iSegment] = globalPosition.eta();
      ev.recoCscSeg_glob_phi    [whichMuon][iSegment] = globalPosition.phi();
      ev.recoCscSeg_glob_dir_eta[whichMuon][iSegment] = globalDirection.eta();
      ev.recoCscSeg_glob_dir_phi[whichMuon][iSegment] = globalDirection.phi();

      if (printLevel > 4) {
	std::cout << "globalPosition.x()    = " << ev.recoCscSeg_glob_x      [whichMuon][iSegment] << " cm"<< std::endl;
	std::cout << "globalPosition.y()    = " << ev.recoCscSeg_glob_y      [whichMuon][iSegment] << " cm"<< std::endl;
	std::cout << "globalPosition.eta()  = " << ev.recoCscSeg_glob_eta    [whichMuon][iSegment] << std::endl;
	std::cout << "globalPosition.phi()  = " << ev.recoCscSeg_glob_phi    [whichMuon][iSegment] << std::endl;
	std::cout << "globalDirection.eta() = " << ev.recoCscSeg_glob_dir_eta[whichMuon][iSegment] << std::endl;
	std::cout << "globalDirection.phi() = " << ev.recoCscSeg_glob_dir_phi[whichMuon][iSegment] << std::endl << std::endl;
      }
      

      // is the segment triggerable?
      bool isTriggerAble = _matchBox.isLCTAble( (*segmentCSC)->cscsegcand, 0);
      
      // is the segment matched to an LCT?
      bool isLCTMatched  = _matchBox.isMatched( (*segmentCSC)->cscsegcand, CSCTFlcts , 0 );

      if (printLevel > 4) cout <<"isMatched? = " << isLCTMatched  << endl;

      ev.recoCscSeg_isLctAble[whichMuon][iSegment] = isTriggerAble;
      ev.recoCscSeg_isMatched[whichMuon][iSegment] = isLCTMatched;
      
      vector<int> whichLCT;  // find the corresponding LCT in the list.  This seems to be done by matching CSCDetId for segment lct and list lct
      if (isLCTMatched) {
        
	int iLCT=-1;

        CSCDetId *segDetId = 0;
        const CSCDetId &origId = (*segmentCSC)->cscsegcand.cscDetId();

        // if we're in ME11a, we have to worry about triple-ganging of strips.
        if (origId.ring() == 4){
          segDetId = new CSCDetId ( origId.endcap(), origId.station(), 1,origId.chamber());
        } else {
          segDetId = new CSCDetId ( origId );
	}
	
	// loop over CSC Lcts
        int match_count = 0; // keep track of how many lcts can be matched to the same segment
	
        //for( auto corrLct = CSCTFlcts->cbegin(); corrLct != CSCTFlcts->cend(); corrLct++) {
	std::vector<L1TMuon::TriggerPrimitive> LCT_collection;
	
	auto chamber = CSCTFlcts -> begin();
	auto chend  = CSCTFlcts -> end();
	for( ; chamber != chend; ++chamber ) {
	  auto digi = (*chamber).second.first;
	  auto dend = (*chamber).second.second;
	  for( ; digi != dend; ++digi ) {
	    LCT_collection.push_back(TriggerPrimitive((*chamber).first,*digi));
	  }
	}
	
	auto Lct = LCT_collection.cbegin();
	auto Lctend = LCT_collection.cend();

	for( ; Lct != Lctend; Lct++) {
	
	  iLCT++;
          if (printLevel > 4) cout << "Looping over Lct # " << iLCT << endl;

	  if (Lct->subsystem() != 1 ) continue;
          CSCDetId LctId = Lct->detId<CSCDetId>();

	  if (printLevel > 4) {
	    cout << "*segDetId: " << *segDetId << endl;
	    cout << "LctId    : " << LctId << endl;
	  }
	  
          // find the matching one
          if ( (*segDetId) == LctId) {
	    match_count ++;
            whichLCT.push_back(iLCT);
            if (printLevel > 4 ) cout << "Match is found. Corresponds to LCT number:" << iLCT << endl;
          }
	  
        }  // end loop CSC lcts
	
	if (whichLCT.size() == 0)
	  ev.recoCscSeg_lctId[whichMuon][iSegment] = -999;
	
	else
	  ev.recoCscSeg_lctId[whichMuon][iSegment] = whichLCT.front();

	
	// Only when we have tracks compile the rest
	/*
	// If match has been made to only one or no lcts do the following
        if (match_count == 1) {
          recoCscSeg_lctId[whichMuon][iSegment] = whichLCT.front();
          if (printLevel > 1) cout << "Fill recoCscSeg_lctId with " << whichLCT.front()  << endl;
        }
	
        if (match_count == 0) recoCscSeg_lctId[whichMuon][iSegment] = -999;
	*/
      }
    } // end segment loop
      //delete segVect;
      
  } // end muon loop

} // end fill SegmentsMuon


