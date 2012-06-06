#ifndef Tracker_SiLinearChargeDivider_H
#define Tracker_SiLinearChargeDivider_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimTracker/SiStripDigitizer/interface/SiChargeDivider.h"
#include "SimTracker/Common/interface/SiG4UniversalFluctuation.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

namespace CLHEP{
  class HepRandomEngine;
}

/**
 * Concrete implementation of SiChargeDivider. 
 * It divides the charge on the line connecting entry and exit point of the SimTrack in the Silicon.
 * Effects that are considered here are:
 * - fluctuations of the charge deposit fronm segment to segment
 * - pulse shape ( in peak of deconvolution mode)
 */
class SiLinearChargeDivider : public SiChargeDivider{
 public:

  // constructor
  SiLinearChargeDivider(const edm::ParameterSet& conf, CLHEP::HepRandomEngine&);

  // destructor
  virtual ~SiLinearChargeDivider();

  // main method: divide the charge (from the PSimHit) into several energy deposits in the bulk
  SiChargeDivider::ionization_type* divide(const PSimHit*, const LocalVector&, double, const StripGeomDetUnit& det);

  // set the ParticleDataTable (used to fluctuate the charge properly)
  void setParticleDataTable(const ParticleDataTable * pdt) { theParticleDataTable = pdt; }
  
 private:
  // configuration
  edm::ParameterSet conf_;
  // configuration data
  bool   peakMode;
  bool   fluctuateCharge;
  int    chargedivisionsPerStrip;
  double deltaCut ;
  double cosmicShift;
  const ParticleDataTable * theParticleDataTable;
  // Geant4 engine used by fluctuateEloss()
  SiG4UniversalFluctuation* fluctuate; 
  // random generator
  CLHEP::HepRandomEngine& rndEngine;
  // Change to member data
  ionization_type m_ionization_points;
  std::vector<float> m_eLossVector;
  // utility: drifts the charge to the surface to estimate the number of relevant strips
  inline float driftXPos(const Local3DPoint& pos, const LocalVector& drift, double thickness) { 
    return pos.x()+(thickness/2.-pos.z())*drift.x()/drift.z();
  }
  // fluctuate the Eloss
  void fluctuateEloss(int particleId, float momentum, float eloss, float length, int NumberOfSegmentation, float elossVector[]);
  // time response (from the pulse shape)
  inline float TimeResponse( const PSimHit* hit, const StripGeomDetUnit& det) {
    return (peakMode ? PeakShape(hit,det) : DeconvolutionShape(hit,det));
  } 
  // pulse shape in peak mode
  float PeakShape(const PSimHit*, const StripGeomDetUnit& det);
  // pulse shape in deconvolution mode
  float DeconvolutionShape( const PSimHit*, const StripGeomDetUnit& det);
  // data table for pulse shape in peak mode
  static float peakValues[921];
  // data table for pulse shape in deconvolution mode
  static float decoValues[651];

};

#endif