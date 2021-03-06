import FWCore.ParameterSet.Config as cms

from RecoJets.JetProducers.CATopJetParameters_cfi import *
from RecoJets.JetProducers.GenJetParameters_cfi import *
from RecoJets.JetProducers.CaloJetParameters_cfi import *
from RecoJets.JetProducers.PFJetParameters_cfi import *
from RecoJets.JetProducers.AnomalousCellParameters_cfi import *


caTopCaloJets = cms.EDProducer("CATopJetProducer",
                               CATopJetParameters,
                               CaloJetParameters,
                               AnomalousCellParameters,
                               jetAlgorithm = cms.string("CambridgeAachen"),
                               rParam = cms.double(0.8)
                               )


caTopGenJets = cms.EDProducer("CATopJetProducer",
                              CATopJetParameters,
                              GenJetParameters,
                              AnomalousCellParameters,
                              jetAlgorithm = cms.string("CambridgeAachen"),
                              rParam = cms.double(0.8)
                              )



caTopPFJets = cms.EDProducer("CATopJetProducer",
                             CATopJetParameters,
                             PFJetParameters,
                             AnomalousCellParameters,
                             jetAlgorithm = cms.string("CambridgeAachen"),
                             rParam = cms.double(0.8)
                             )

