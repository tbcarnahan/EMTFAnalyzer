#include "TROOT.h"
#include <vector>

// FWCore
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h" // Necessary for DEFINE_FWK_MODULE

// Output branches
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EventInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/GenMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFSimHitInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/EMTFUnpTrackInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/CSCSegInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoMuonInfo.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Branches/RecoPairInfo.h"

// Object matchers
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/RecoTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/RecoUnpTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/UnpEmuTrkDR.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/SimUnpHit.h"
#include "EMTFAnalyzer/NTupleMaker/interface/Matchers/LCTSeg.h"

// CSC segment geometry
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"

// GEM Copads
#include "DataFormats/GEMDigi/interface/GEMCoPadDigiCollection.h"
//#include "L1Trigger/CSCTriggerPrimitives/src/CSCMotherboardME11GEM.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"
//#include "L1Trigger/CSCCommonTrigger/interface/CSCTriggerGeometry.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/normalizedPhi.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"

//#define DataFormats_L1TMuon_EMTFTrack_h

static const float AVERAGE_GEM_Z(568.6); // [cm]

static const float AVERAGE_GE11_ODD_Z(568.6); // [cm]
static const float AVERAGE_GE11_EVEN_Z(568.6); // [cm]

static const float AVERAGE_ME11_EVEN_Z(585); // [cm]
static const float AVERAGE_ME11_ODD_Z(615); // [cm]


class GEMEMTFMatcher : public edm::EDProducer {

 public:

  // Constructor/destructor
  explicit GEMEMTFMatcher(const edm::ParameterSet&);
  ~GEMEMTFMatcher();

 private:

  virtual void beginJob();
  virtual void beginRun(const edm::Run&,   const edm::EventSetup&);
  virtual void produce (edm::Event&, const edm::EventSetup&);
  virtual void endRun  (const edm::Run&,   const edm::EventSetup&);
  virtual void endJob  ();

  // EDM Tokens
  edm::EDGetTokenT<std::vector<l1t::EMTFHit>>              EMTFHit_token;
  edm::EDGetTokenT<std::vector<l1t::EMTFTrack>>            EMTFTrack_token;
  edm::EDGetTokenT<GEMCoPadDigiCollection>                 GEMCoPad_token;

  /// general interface to propagation
  GlobalPoint propagateToZ(const GlobalPoint &inner_point, const GlobalVector &inner_vector, float z, int charge) const;


}; // End class GEMEMTFMatcher public edm::EDProducer

// Constructor
GEMEMTFMatcher::GEMEMTFMatcher(const edm::ParameterSet& iConfig)
{
  EMTFHit_token      = consumes<std::vector<l1t::EMTFHit>>   (iConfig.getParameter<edm::InputTag>("emtfHitTag"));
  EMTFTrack_token    = consumes<std::vector<l1t::EMTFTrack>> (iConfig.getParameter<edm::InputTag>("emtfTrackTag"));
  GEMCoPad_token = consumes<GEMCoPadDigiCollection> (iConfig.getParameter<edm::InputTag>("gemCoPadTag"));

  produces<std::vector<l1t::EMTFTrack>>();
  produces<std::vector<l1t::EMTFHit>>();

} // End GEMEMTFMatcher::GEMEMTFMatcher

// Destructor
GEMEMTFMatcher::~GEMEMTFMatcher() {}

// Called once per run
void GEMEMTFMatcher::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {} // End GEMEMTFMatcher::beginRun()


// Called once per run
void GEMEMTFMatcher::endRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  std::cout << "\nInside GEMEMTFMatcher::endRun()\n" << std::endl;
}


// Called once per event
void GEMEMTFMatcher::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::unique_ptr<std::vector<l1t::EMTFTrack>> oc(new std::vector<l1t::EMTFTrack>);
  std::unique_ptr<std::vector<l1t::EMTFHit>> oc2(new std::vector<l1t::EMTFHit>);

  edm::Handle<std::vector<l1t::EMTFHit>> emtfHits;
  iEvent.getByToken(EMTFHit_token, emtfHits);

  edm::Handle<std::vector<l1t::EMTFTrack>> emtfTracks;
  iEvent.getByToken(EMTFTrack_token, emtfTracks);

  edm::Handle<GEMCoPadDigiCollection> gemCoPadsH;
  iEvent.getByToken(GEMCoPad_token, gemCoPadsH);
  const GEMCoPadDigiCollection& gemCoPads = *gemCoPadsH.product();

  edm::ESHandle<CSCGeometry> cscGeom;
  iSetup.get<MuonGeometryRecord>().get(cscGeom);

  edm::ESHandle<GEMGeometry> gemGeom;
  iSetup.get<MuonGeometryRecord>().get(gemGeom);

  // first step: copy over the original EMTF Hit collection
  if ( emtfHits.isValid() ) {
    // loop over the tracks
    for (const auto& hit : *emtfHits) {
      oc2->push_back(hit);     
    }

  }

  if ( emtfTracks.isValid() ) {

    // loop over the tracks
    for (const auto& ttrack : *emtfTracks) {

      auto track = ttrack;

      const auto& trackHits = track.Hits();

      double glob_phi;
      double glob_theta;
      double glob_eta;
      double glob_rho;
      int extrapolated_pad;

      // here, need to do the association with GEM hits
      for (const l1t::EMTFHit& emtfHit: trackHits) {
	
        // require ME1/1 stubs!
        if (emtfHit.Is_CSC() == 1 and
            emtfHit.Station() == 1 and
            emtfHit.Ring() == 1) {

	  //std::cout << "ME11 chamber: " << emtfHit.Chamber() << ", ME11 hit Pattern: " << emtfHit.Pattern() << ", Strip: " << emtfHit.Strip() << std::endl;
	  //if(emtfHit.Pattern()!=10){break;} //Ignore patterns!=10 until done bugfixing.

          // ME1/1 detid
          const auto& cscId = emtfHit.CSC_DetId();
          CSCDetId key_id(cscId.endcap(), cscId.station(), cscId.ring(), cscId.chamber(), 3);

          // sector and subsector
          const int sector(CSCTriggerNumbering::triggerSectorFromLabels(key_id));
          const int subsector(CSCTriggerNumbering::triggerSubSectorFromLabels(key_id));

          // ME1/1 chamber
          const auto& cscChamber = cscGeom->chamber(cscId);

          // CSC GP
          const auto& lct = emtfHit.CreateCSCCorrelatedLCTDigi();
          float fractional_strip = lct.getFractionalStrip();
          const auto& layer_geo = cscChamber->layer(3)->geometry();
          // LCT::getKeyWG() also starts from 0
          float wire = layer_geo->middleWireOfGroup(lct.getKeyWG() + 1);
          const LocalPoint& csc_intersect = layer_geo->intersectionOfStripAndWire(fractional_strip, wire);
          const GlobalPoint& csc_gp = cscGeom->idToDet(key_id)->surface().toGlobal(csc_intersect);

            // best copad
          GEMCoPadDigi best;
          GEMDetId bestId;

          // have to consider +1/0/-1 GEM chambers
          //for (int deltaChamber = -1; deltaChamber <= 1; deltaChamber++){
	  for (int deltaChamber = 0; deltaChamber<1; deltaChamber++){

            // corresponding GE1/1 detid
            const GEMDetId gemId(cscId.zendcap(), 1, 1, 0,
                                 (cscId.chamber() + deltaChamber) % 36,
                                 0);

            // copad collection
            const auto& co_pads_in_det = gemCoPads.get(gemId);

            // at most the width of an ME11 chamber
	    float minDPhi = 0.17;
	    float minDPad = 32;

            // loop on the GEM coincidence pads
            // find the closest matching one
            for (auto it = co_pads_in_det.first; it != co_pads_in_det.second; ++it) {
              // pick the first layer in the copad!
              const auto& copad = (*it);

              // select in-time coincidence pads
              if ( std::abs(copad.bx(1)) != 0) continue;

              const GEMDetId gemCoId(cscId.zendcap(), 1, 1, 1,
                                     (cscId.chamber() + deltaChamber) % 36,
                                     copad.roll());

	      /////////////////////////////////////////////////////////////
	      //Linear extrapolation code
	      //First extrapolate the halfstrip to GE1/1
	      //A description of how these LUT values are defined can be found in [link to detector note here].
	      float a;
	      float b;
	      float c;

	      if(emtfHit.Pattern()==2 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-5.0;
                b = emtfHit.Strip()+5.0;
                c = emtfHit.Strip()+15.0;
              }

              if(emtfHit.Pattern()==2 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip());
                b = 192 - (emtfHit.Strip()+10.0);
                c = 192 - (emtfHit.Strip()+20.0);
              }

              if(emtfHit.Pattern()==3 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-8.0;
                b = emtfHit.Strip()-4.0;
                c = emtfHit.Strip();
              }

              if(emtfHit.Pattern()==3 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip()-15.0);
                b = 192 - (emtfHit.Strip()-5.0);
                c = 192 - (emtfHit.Strip()+5.0);
              }

	      if(emtfHit.Pattern()==4 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-8.0;
                b = emtfHit.Strip()+5.0;
                c = emtfHit.Strip()+18.0;
              }

              if(emtfHit.Pattern()==4 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip()-5.0);
                b = 192 - (emtfHit.Strip()+13.0);
                c = 192 - (emtfHit.Strip()+30.0);
              }

	      if(emtfHit.Pattern()==5 and emtfHit.Chamber()%2==0) {  
                a = emtfHit.Strip()-25.0;
                b = emtfHit.Strip()-7.0;
                c = emtfHit.Strip()+11.0;
              }

              if(emtfHit.Pattern()==5 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip()-25.0);
                b = 192 - (emtfHit.Strip()-10.0);
                c = 192 - (emtfHit.Strip()+5.0);
              }

	      if(emtfHit.Pattern()==6 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-6.0;
                b = emtfHit.Strip()+4.0;
                c = emtfHit.Strip()+15.0;
              }

              if(emtfHit.Pattern()==6 and emtfHit.Chamber()%2!=0) {
                a = 192 - emtfHit.Strip();
                b = 192 - (emtfHit.Strip()+15.0);
                c = 192 - (emtfHit.Strip()+30.0);
              }

	      if(emtfHit.Pattern()==7 and emtfHit.Chamber()%2==0) {                           
                a = emtfHit.Strip()-15.0;
                b = emtfHit.Strip()-2.0;
                c = emtfHit.Strip()+10.0;
              }

              if(emtfHit.Pattern()==7 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip()-25.0);
                b = 192 - (emtfHit.Strip()-7.0);
                c = 192 - (emtfHit.Strip()+10.0);
              }

	      if(emtfHit.Pattern()==8 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-6.0;
                b = emtfHit.Strip()+4.0;
                c = emtfHit.Strip()+14.0;
              }

              if(emtfHit.Pattern()==8 and emtfHit.Chamber()%2!=0) {
                a = 192 - (emtfHit.Strip()-5.0);
                b = 192 - (emtfHit.Strip()+6.0);
                c = 192 - (emtfHit.Strip()+18.0);
              }

	      if(emtfHit.Pattern()==9 and emtfHit.Chamber()%2==0) {
                a = emtfHit.Strip()-10.0;
                b = emtfHit.Strip()-1.0;
                c = emtfHit.Strip()+7.0;
              }

              if(emtfHit.Pattern()==9 and emtfHit.Chamber()%2!=0) {                                            
                a = 192 - (emtfHit.Strip()-18.0);
                b = 192 - (emtfHit.Strip()-3.0);
                c = 192 - (emtfHit.Strip()+10.0);
              }

	      if(emtfHit.Pattern()==10 and emtfHit.Chamber()%2==0) {
		a = emtfHit.Strip()-4.0;
		b = emtfHit.Strip();
		c = emtfHit.Strip()+4.0;
	      }

	      if(emtfHit.Pattern()==10 and emtfHit.Chamber()%2!=0) {
		a = 192 - (emtfHit.Strip()-8.0);
                b = 192 - (emtfHit.Strip()+1.0);
                c = 192 - (emtfHit.Strip()+10.0);
	      }
	      
              const LocalPoint& gem_lp = gemGeom->etaPartition(gemCoId)->centreOfPad(copad.pad(1));
              const GlobalPoint& gem_gp = gemGeom->idToDet(gemCoId)->surface().toGlobal(gem_lp);
	      
	      float dPad_a = std::abs((copad.pad(1)*0.67) - a); //0.67 is a scale-factor; a/b/c are HS numbers (192 copads per phi sector but 128 HS per phi sector)
	      float dPad_b = std::abs((copad.pad(1)*0.67) - b);
	      float dPad_c = std::abs((copad.pad(1)*0.67) - c);

              //if (std::abs(currentDPhi) < std::abs(minDPhi)) {
	      if ((dPad_a <= std::abs(minDPad)) or (dPad_b <= std::abs(minDPad)) or (dPad_c <= std::abs(minDPad))) {
                best = copad;
                bestId = gemCoId;
                //minDPhi = currentDPhi;
		if ((dPad_a<dPad_b) and (dPad_a<dPad_c)) {minDPad = dPad_a;}
                if ((dPad_b<dPad_a) and (dPad_b<dPad_c)) {minDPad = dPad_b;}
		if ((dPad_c<dPad_a) and (dPad_c<dPad_b)) {minDPad = dPad_c;}

		glob_phi = emtf::rad_to_deg(gem_gp.phi().value());
		glob_theta = emtf::rad_to_deg(gem_gp.theta().value());
		glob_eta = gem_gp.eta();
		glob_rho = gem_gp.perp();

              }
            }	    

          }

          if (best.isValid()) {
	    l1t::EMTFHit bestEMTFHit;	    

            // create a new EMTFHit with the
            // best matching coincidence pad
            bestEMTFHit.set_subsystem(3);
            bestEMTFHit.SetGEMDetId(bestId);
            bestEMTFHit.set_endcap(bestId.region());
            bestEMTFHit.set_station(1);
            bestEMTFHit.set_ring(1);
            bestEMTFHit.set_chamber(bestId.chamber());
            bestEMTFHit.set_sector(sector);
            bestEMTFHit.set_subsector(subsector);
            bestEMTFHit.set_roll(best.roll());
            bestEMTFHit.set_strip(best.pad(1));
            bestEMTFHit.set_strip_hi(best.pad(1));
            bestEMTFHit.set_strip_low(best.pad(2));
            bestEMTFHit.set_bx(best.bx(1));
            bestEMTFHit.set_valid(1);
	    
	    int fph = emtf::calc_phi_loc_int(glob_phi, sector);                                                               
            int th = emtf::calc_theta_int(glob_theta, bestEMTFHit.Endcap());                                                                   

            if (0 > fph || fph > 4920) {break;}                                                                     
            if (0 > th || th > 32) {break;}
            if (th == 0b11111) {break;}  // RPC hit valid when data is not all ones
            fph <<= 2;                   // upgrade to full CSC precision by adding 2 zeros         
            th <<= 2;                    // upgrade to full CSC precision by adding 2 zeros                                                   
            th = (th == 0) ? 1 : th;     // protect against invalid value

	    bestEMTFHit.set_phi_sim(glob_phi);
	    bestEMTFHit.set_theta_sim(glob_theta);
	    bestEMTFHit.set_eta_sim(glob_eta);
	    bestEMTFHit.set_rho_sim(glob_rho);

	    bestEMTFHit.set_phi_loc(emtf::calc_phi_loc_deg_from_glob(glob_phi, sector));
	    bestEMTFHit.set_phi_glob(glob_phi);
	    bestEMTFHit.set_eta(emtf::calc_eta_from_theta_deg(glob_theta, bestEMTFHit.Endcap() ));
	    bestEMTFHit.set_theta(glob_theta);

	    bestEMTFHit.set_phi_fp(fph);   // Full-precision integer phi
	    bestEMTFHit.set_theta_fp(th);  // Full-precision integer theta

            // push the new hit to the track and to the hit collection
            track.push_Hit(bestEMTFHit);
            oc2->push_back(bestEMTFHit);
	    
            // An EMTF track can only be matched to 1 copad!
            break;
          }
        }
      }

      oc->push_back(track);
    
    }
  }

  //std::cout << "---Next event----" << std::endl;
  // put the new collections in the event
  iEvent.put(std::move(oc));
  iEvent.put(std::move(oc2));
} // End GEMEMTFMatcher::produce

// Called once per job before starting event loop
void GEMEMTFMatcher::beginJob() {
} // End GEMEMTFMatcher::beginJob

  // Called once per job after ending event loop
void GEMEMTFMatcher::endJob() {
}


  // Define as a plugin
DEFINE_FWK_MODULE(GEMEMTFMatcher);
