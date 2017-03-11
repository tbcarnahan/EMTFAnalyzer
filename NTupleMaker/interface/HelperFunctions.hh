
#ifndef HelperFunctions_hh
#define HelperFunctions_hh

// Default constants
float PI   = 3.14159265359;

// Integer theta-to-eta conversion table
static const int GMT_eta_from_theta[128] = {
  239, 235, 233, 230, 227, 224, 222, 219, 217, 214, 212, 210, 207, 205, 203, 201,
  199, 197, 195, 193, 191, 189, 187, 186, 184, 182, 180, 179, 177, 176, 174, 172,
  171, 169, 168, 166, 165, 164, 162, 161, 160, 158, 157, 156, 154, 153, 152, 151,
  149, 148, 147, 146, 145, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133,
  132, 131, 130, 129, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117,
  116, 116, 115, 114, 113, 112, 111, 110, 110, 109, 108, 107, 106, 106, 105, 104,
  103, 102, 102, 101, 100,  99,  99,  98,  97,  96,  96,  95,  94,  93,  93,  92,
  91,  91,  90,  89,  89,  88,  87,  87,  86,  85,  84,  84,  83,  83,  82,  81
};


inline int calc_GMT_eta_from_theta(int theta, int endcap) {  
  
  if (theta < 0)
    return 0;
  if (endcap == 1 && theta > 127)
    return 239;
  if (endcap != 1 && theta > 127)
    return -240;
  
  int eta = GMT_eta_from_theta[theta];
  if (endcap != 1)
    eta = -eta;
  return eta;
}

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


#endif /* define HelperFunctions_hh */
