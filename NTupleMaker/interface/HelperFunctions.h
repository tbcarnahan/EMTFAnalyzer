
#ifndef HelperFunctions_h
#define HelperFunctions_h

// Most helper functions already defined in EMTF emulator
#include "L1Trigger/L1TMuonEndCap/interface/TrackTools.h"

inline bool calc_FR_bit(int station, int ring, int chamber) {
  bool result = false;
  bool isOverlapping = !(station == 1 && ring == 3);
  // Not overlapping means back
  if(isOverlapping) {
    bool isEven = (chamber % 2 == 0);
    // odd chambers are bolted to the iron, which faces
    // forward in 1 & 2, backward in 3 & 4, so...
    result = (station < 3) ? isEven : !isEven;
  }
  return result;
}

// Taken from L1Trigger/L1TMuonEndCap/src/AngleCalculation.cc
inline bool isFront(int station, int ring, int chamber, int subsystem) {

  if (subsystem > 2)  // Not CSC and not RPC
    return true;

  bool result = false;
  bool isOverlapping = !(station == 1 && ring == 3);
  // not overlapping means back
  if(isOverlapping) {
    bool isEven = (chamber % 2 == 0);
    // odd chambers are bolted to the iron, which faces
    // forward in 1&2, backward in 3&4, so...
    result = (station < 3) ? isEven : !isEven;
  }
  return result;
}

inline float calc_dPhi(const float phi1, const float phi2) {

  float dPhi = acos( cos(phi2 - phi1) );
  if ( sin(phi2 - phi1) < 0 ) dPhi *= -1;
  return dPhi;
}

inline float calc_dR(const float eta1, const float phi1, const float eta2, const float phi2) {

  float dEta = eta2 - eta1;
  float dPhi = calc_dPhi(phi1, phi2);
  float dR = sqrt( pow(dEta, 2) + pow(dPhi, 2) );

  return dR;
}


#endif /* define HelperFunctions_h */
