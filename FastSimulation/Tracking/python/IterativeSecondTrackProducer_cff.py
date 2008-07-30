import FWCore.ParameterSet.Config as cms

import RecoTracker.TrackProducer.CTFFinalFitWithMaterial_cfi
iterativeSecondTracksWithTriplets = RecoTracker.TrackProducer.CTFFinalFitWithMaterial_cfi.ctfWithMaterialTracks.clone()
iterativeSecondTracks = cms.Sequence(iterativeSecondTracksWithTriplets)
iterativeSecondTracksWithTriplets.src = 'iterativeSecondTrackCandidatesWithTriplets'
iterativeSecondTracksWithTriplets.TTRHBuilder = 'WithoutRefit'
iterativeSecondTracksWithTriplets.Fitter = 'KFFittingSmootherWithOutlierRejection'
iterativeSecondTracksWithTriplets.Propagator = 'PropagatorWithMaterial'


