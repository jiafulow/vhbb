import FWCore.ParameterSet.Config as cms
import Geometry.TrackerGeometryBuilder.trackerGeometryConstants_cfi as trackerGeometryConstants_cfi

#
# This cfi should be included to build the Tracker geometry model.
#
# GF would like to have a shorter name (e.g. TrackerGeometry), but since originally
# there was no name, replace statements in other configs would not work anymore...
TrackerDigiGeometryESModule = cms.ESProducer("TrackerDigiGeometryESModule",
    appendToDataLabel = cms.string(''),
    fromDDD = cms.bool(False),
    applyAlignment = cms.bool(True), # to be abondoned

    alignmentsLabel = cms.string(''),
    trackerGeometryConstants = cms.PSet(trackerGeometryConstants_cfi.trackerGeometryConstants)
)
