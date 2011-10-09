import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
import FWCore.ParameterSet.Config as cms

process = cms.Process("FWLitePlots")

#fileNames   = cms.vstring('file:2l2bMetEdmNtuples.root'),         ## mandatory
process.fwliteInput = cms.PSet(
    fileNames   = cms.vstring(
"/gpfs/gpfsddn/cms/user/arizzi/Hbb/V9/CMSSW_4_2_8_patch3/src/VHbbAnalysis/HbbAnalyzer/test/PAT.edm.root"

),

    PUmcfileName = cms.string("ttbarPU.root"),
    PUdatafileName = cms.string("PUdist_160404-167151_7TeV_PromptReco_Collisions11.root"),
    maxEvents   = cms.int32(-1),                             ## optional
    skipEvents   = cms.int32(0),                             ## optional
    outputEvery = cms.uint32(0),                            ## optional
    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange()),
    )

# get JSON file 
#JSONfile = 'Input.json'
#lumiList = LumiList.LumiList (filename = JSONfile).getCMSSWString().split(',')

#Uncomment to run with JSON
#process.fwliteInput.lumisToProcess.extend(lumiList)


channel =  "DYpt100"
import os
dirnameOld = "//pnfs/pi.infn.it/data/cms/store/user/bortigno/DoubleMu/HBB_EDMNtupleV3_ProcV1_may/07fb60889166b64f474d8d0aa162db69/"




#for i in range(len(channels)):
 

#dirname =  dirnameOld 
#dirlist = os.listdir(dirname)
#basenamelist = os.listdir(dirname + "/")
#for basename in basenamelist:
#   process.fwliteInput.fileNames.append("root://cmsdcache" + dirname + "/" + basename)
print "Number of files to process is %s" %(len(process.fwliteInput.fileNames)) 
    
    



#


fname = 'Test' + channel + '.root'

process.fwliteOutput = cms.PSet(
    
    fileName  = cms.string(fname),## mandatory
    )

process.Analyzer = cms.PSet(
    triggers = cms.vstring(
	"HLT_IsoMu17_v.*" , #0
	"HLT_DoubleMu7_v.*", #1
	"HLT_Mu13_Mu8_v.*", #2
	"HLT_Ele27_CaloIdVT_CaloIsoT_TrkId_TrkIsoT_v.*", #3
	"HLT_Ele27_WP80_PFMHT50_v.*", #4
        "HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v.*", #5
        "HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v.*", #6
        "HLT_DiCentralJet20_BTagIP_MET65_v.*", #7
	"HLT_MET120_v.*", #8
	"HLT_CentralJet80_MET80_v.*", #9
	"HLT_PFMHT150_v.*", #10
	"HLT_DiCentralJet20_MET80_v.*", #11
        "HLT_DiCentralJet20_MET100_HBHENoiseFiltered_v.*", #12
        "HLT_IsoMu20_v.*", #13
        "HLT_IsoMu24_v.*", #14
        "HLT_IsoMu30_eta2p1_v.*", #15
        "HLT_Mu17_Mu8_v.*", #16
        "HLT_Ele17_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15_v.*", #17
        "HLT_Ele22_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT20_v.*" #18
   ),
    isMC =     cms.bool(True),
    verbose = cms.bool(False),
    readFromCandidates = cms.bool(False),
    jetPtThresholdZ = cms.double(20),
    jetPtThresholdW = cms.double(20),
    bJetCountThreshold = cms.double(0.898),
    useHighestPtHiggsW = cms.bool(True),
    useHighestPtHiggsZ = cms.bool(False),
    idMuFileName = cms.string("ScaleEffs42.root"),
    hltMuFileName = cms.string("IsoToHLT42.root"),

    hltEle1FileName = cms.string("Ele17.root"),
    hltEle2FileName = cms.string("Ele8NotEle17.root"),
    idEle80FileName = cms.string("PFElectronToWP80.root"),
    idEle95FileName = cms.string("PFElectronToWP95.root"),
    hltJetEle1FileName = cms.string("TriggerEfficiency_JetNo30_Jet25.root"),
    hltJetEle2FileName = cms.string("TriggerEfficiency_Jet30.root"),
    recoEleFileName = cms.string("EleReco.root"),
    hltSingleEleMayFileName = cms.string("TriggerEfficiency_Electrons_May10.root"),
    hltSingleEleV4FileName = cms.string("TriggerEfficiency_Electrons_PromptV4.root"),
    idEleFileName = cms.string("ScaleFactor_PFElectrons_DataMontecarlo.root"),

    btagEffFileName = cms.string("btag_generic.txt")
    )

    
  
    

