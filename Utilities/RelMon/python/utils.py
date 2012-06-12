################################################################################
# RelMon: a tool for automatic Release Comparison                              
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/RelMon
#
# $Author: dpiparo $
# $Date: 2012/03/09 13:55:23 $
# $Revision: 1.17 $
#
#                                                                              
# Danilo Piparo CERN - danilo.piparo@cern.ch                                   
#                                                                              
################################################################################


import array
from cPickle import load
import os
from os.path import dirname,basename,join,isfile
from threading import Thread
from time import asctime 
import sys

theargv=sys.argv
sys.argv=[]
from ROOT import *
import ROOT
ROOT.gErrorIgnoreLevel=1001
ROOT.gROOT.SetBatch(True)
sys.argv=theargv

if os.environ.has_key("RELMON_SA"):
  from definitions import *
  from urllib2  import Request,build_opener,urlopen
  from authentication import X509CertOpen
  from utils import __file__ as this_module_name  
else:
  from Utilities.RelMon.definitions import *
  from Utilities.RelMon.urllib2  import Request,build_opener,urlopen
  from Utilities.RelMon.authentication import X509CertOpen
  from Utilities.RelMon.utils import __file__ as this_module_name

#ROOT.gErrorIgnoreLevel=1001


_log_level=10
def logger(msg_level,message):
  if msg_level>=_log_level:
    print "[%s] %s" %(asctime(),message)

#-------------------------------------------------------------------------------
def setTDRStyle():  
  this_dir=dirname(this_module_name)
  this_dir_one_up=this_dir[:this_dir.rfind("/")+1]
  #this_dir_two_up=this_dir_one_up[:this_dir_one_up.rfind("/")+1]
  style_file=''
  if os.environ.has_key("RELMON_SA"):
    style_file=this_dir_one_up+"data/tdrstyle_mod.C"
  else:
    style_file="%s/Utilities/RelMon/data/tdrstyle_mod.C"%(os.environ["CMSSW_BASE"])
  try:
    gROOT.ProcessLine(".L %s" %style_file)
    gROOT.ProcessLine("setTDRStyle()")
  except:
    "Print could not set the TDR style. File %s not found?" %style_file
    

#-------------------------------------------------------------------------------
def literal2root (literal,rootType):
  bitsarray = array.array('B')
  bitsarray.fromstring(literal.decode('hex'))

  tbuffer=0
  try:  
      tbuffer = TBufferFile(TBufferFile.kRead, len(bitsarray), bitsarray, False,0)
  except:
      print "could not transform to object array:"
      print [ i for i in  bitsarray ]
  
  # replace a couple of shortcuts with the real root class name
  if rootType == 'TPROF':
      rootType = 'TProfile'
  if rootType == 'TPROF2D':
      rootType = 'TProfile2D'
  
  root_class=eval(rootType+'.Class()')
  
  return tbuffer.ReadObject(root_class)
  
#-------------------------------------------------------------------------------

def getNbins(h):
  biny=h.GetNbinsY()
  if biny>1:biny+=1
  binz=h.GetNbinsZ()
  if binz>1:binz+=1
  return (h.GetNbinsX()+1)*(biny)*(binz)

#-------------------------------------------------------------------------------


class StatisticalTest(object):
  def __init__(self,threshold):
    self.name=""
    self.h1=None
    self.h2=None
    self.threshold=float(threshold)
    self.rank=-1
    self.is_init=False

  def set_operands(self,h1,h2):
    self.h1=h1
    self.h2=h2

  def get_rank(self):
    if not self.is_init:
      if self.rank<0:
        type1=type(self.h1)
        type2=type(self.h2)
        if (type1 != type2):
          logger(1,"*** ERROR: object types in comparison don't match: %s!=%s" %(type1,type2))
          self.rank=test_codes["DIFF_TYPES"]
        elif not self.h2.InheritsFrom("TH1"):
          logger(1,"*** ERROR: object type is not histogram but a %s" %(type1))
          self.rank=test_codes["NO_HIST"]    
        # if histos are empty
        #elif self.h1.InheritsFrom("TH2") and not "BinToBin" in self.name:
          ## 2D!
          #return test_codes["2D"]
        else:
          is_empty1=is_empty(self.h1)
          is_empty2=is_empty(self.h2)
          are_empty=is_empty1 and is_empty2
          one_empty=is_empty1 or is_empty2

          Nbins1= getNbins(self.h1)
          Nbins2= getNbins(self.h2)

          if are_empty:
            #return -103
            # Conversation with JeanRoch and David 5 April
            return 1
          elif one_empty:
            #return -103
            # Conversation with JeanRoch and David 5 April
            return 1

          # if histos have different number of bins
          if Nbins1!=Nbins2:
            return test_codes["DIFF_BIN"]

      self.rank=self.do_test()
    self.is_init=True
    return self.rank

  def get_status(self):
    status = SUCCESS
    if self.get_rank()<0:
      status=NULL
      logger(0,"+++ Test %s FAILED: rank is %s and threshold is %s ==> %s" %(self.name, self.rank, self.threshold, status))  
    elif self.get_rank()<=self.threshold:
      status=FAIL
    logger(0,"+++ Test %s: rank is %s and threshold is %s ==> %s" %(self.name, self.rank, self.threshold, status))
    return status

  def do_test(self):
    pass

#-------------------------------------------------------------------------------

def is_empty(h):
  for i in xrange(1,getNbins(h)):
    if h.GetBinContent(i)!=0: return False
  return True
  #return h.GetSumOfWeights()==0

#-------------------------------------------------------------------------------

def is_sparse(h):
  filled_bins=0.
  nbins=h.GetNbinsX()
  for ibin in xrange(nbins):
    if h.GetBinContent(ibin)>0:
      filled_bins+=1
  #print "%s %s --> %s" %(filled_bins,nbins,filled_bins/nbins)
  if filled_bins/nbins < .5:
    return True
  else:
    return False

#-------------------------------------------------------------------------------

class KS(StatisticalTest):
  def __init__(self, threshold):
   StatisticalTest.__init__(self,threshold)
   self.name="KS"

  def do_test(self):

    # Calculate errors if not there...
    for h in self.h1,self.h2:
      w2s=h.GetSumw2()
      if w2s.GetSize()==0:
        h.Sumw2()

    ## If errors are 0:
    #zero_errors=True
    #for h in self.h1,self.h2:
      #for ibin in xrange(Nbins1):
        #if h.GetBinError(ibin+1) >0:
          #zero_errors=False
          #break
    #if zero_errors:
      #return test_codes["ZERO_ERR"]

    return self.h1.KolmogorovTest(self.h2)

#-------------------------------------------------------------------------------
import array
def profile2histo(profile):
  if not profile.InheritsFrom("TH1"):
    return profile
    
  bin_low_edges=[]
  n_bins=profile.GetNbinsX()
  
  for ibin in xrange(1,n_bins+2):
    bin_low_edges.append(profile.GetBinLowEdge(ibin))
  bin_low_edges=array.array('f',bin_low_edges)
  histo=TH1F(profile.GetName(),profile.GetTitle(),n_bins,bin_low_edges)
  for ibin in xrange(0,n_bins+1):
    histo.SetBinContent(ibin,profile.GetBinContent(ibin))
    histo.SetBinError(ibin,profile.GetBinError(ibin))    
  
  return histo
#-------------------------------------------------------------------------------

class Chi2(StatisticalTest):
  def __init__(self, threshold):
    StatisticalTest.__init__(self,threshold)
    self.name="Chi2"     

  def check_filled_bins(self,min_filled):
    nbins=self.h1.GetNbinsX()
    n_filled_l=[]
    for h in self.h1,self.h2:
      nfilled=0.
      for ibin in xrange(1,nbins+1):
        if h.GetBinContent(ibin)>0:
          nfilled+=1
      n_filled_l.append(nfilled)
    return len(filter (lambda x:x>=min_filled,n_filled_l) )>0

  def absval(self):
    nbins=getNbins(self.h1)
    binc=0
    for i in xrange(1,nbins):
      for h in self.h1,self.h2:
        binc=h.GetBinContent(i)
        if binc<0:
          h.SetBinContent(i,-1*binc)
        if h.GetBinError(i)==0 and binc!=0:
          #print "Histo ",h.GetName()," Bin:",i,"-Content:",h.GetBinContent(i)," had zero error"
          h.SetBinContent(i,0)


  def do_test(self):
    self.absval()
    if self.check_filled_bins(3):
      if self.h1.InheritsFrom("TProfile") or  (self.h1.GetEntries()!=self.h1.GetSumOfWeights()):
        chi2=self.h1.Chi2Test(self.h2,'WW')
        #if chi2==0: print "DEBUG",self.h1.GetName(),"Chi2 is:", chi2
        return chi2
      else:
        return self.h1.Chi2Test(self.h2,'UU')
    else:
      return 1
      #return test_codes["FEW_BINS"]

#-------------------------------------------------------------------------------

class BinToBin(StatisticalTest):
  """The bin to bin comparison builds a fake pvalue. It is 0 if the number of 
  bins is different. It is % of corresponding bins otherwhise.
  A threshold of 1 is needed to require a 1 to 1 correspondance between 
  hisograms.
  """
  def __init__(self, threshold=1):
    StatisticalTest.__init__(self, threshold)
    self.name='BinToBin'
    self.epsilon= 0.000001

  def checkBinningMatches(self):
    if self.h1.GetNbinsX() != self.h2.GetNbinsX() \
           or self.h1.GetNbinsY() != self.h2.GetNbinsY() \
           or self.h1.GetNbinsZ() != self.h2.GetNbinsZ() \
           or abs(self.h1.GetXaxis().GetXmin() - self.h2.GetXaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetYaxis().GetXmin() - self.h2.GetYaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetZaxis().GetXmin() - self.h2.GetZaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetXaxis().GetXmax() - self.h2.GetXaxis().GetXmax()) >self.epsilon \
           or abs(self.h1.GetYaxis().GetXmax() - self.h2.GetYaxis().GetXmax()) >self.epsilon \
           or abs(self.h1.GetZaxis().GetXmax() - self.h2.GetZaxis().GetXmax()) >self.epsilon:
      return False
    return True

  def do_test(self):
    # fist check that binning matches
    if not self.checkBinningMatches():
      return test_codes["DIFF_BIN"]
    # then do the real check
    equal = 1
    nbins = getNbins(self.h1)
    n_ok_bins=0.0
    for ibin in xrange(0,nbins):
      ibin+=1
      h1bin=self.h1.GetBinContent(ibin)
      h2bin=self.h2.GetBinContent(ibin)
      bindiff=h1bin-h2bin

      binavg=.5*(h1bin+h2bin)

      if binavg==0 or abs(bindiff) < self.epsilon:
        n_ok_bins+=1
        #print "Bin %ibin: bindiff %s" %(ibin,bindiff)
      else:
        print "Bin %ibin: bindiff %s" %(ibin,bindiff)

      #if abs(bindiff)!=0 :
        #print "Bin %ibin: bindiff %s" %(ibin,bindiff)
    
    rank=n_ok_bins/nbins
    
    if rank!=1:
      print "Histogram %s differs: nok: %s ntot: %s" %(self.h1.GetName(),n_ok_bins,nbins)
    
    return rank

#-------------------------------------------------------------------------------

class BinToBin1percent(StatisticalTest):
  """The bin to bin comparison builds a fake pvalue. It is 0 if the number of 
  bins is different. It is % of corresponding bins otherwhise.
  A threshold of 1 is needed to require a 1 to 1 correspondance between 
  hisograms.
  """
  def __init__(self, threshold=1):
    StatisticalTest.__init__(self, threshold)
    self.name='BinToBin1percent'
    self.epsilon= 0.000001
    self.tolerance= 0.01

  def checkBinningMatches(self):
    if self.h1.GetNbinsX() != self.h2.GetNbinsX() \
           or self.h1.GetNbinsY() != self.h2.GetNbinsY() \
           or self.h1.GetNbinsZ() != self.h2.GetNbinsZ() \
           or abs(self.h1.GetXaxis().GetXmin() - self.h2.GetXaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetYaxis().GetXmin() - self.h2.GetYaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetZaxis().GetXmin() - self.h2.GetZaxis().GetXmin()) >self.epsilon \
           or abs(self.h1.GetXaxis().GetXmax() - self.h2.GetXaxis().GetXmax()) >self.epsilon \
           or abs(self.h1.GetYaxis().GetXmax() - self.h2.GetYaxis().GetXmax()) >self.epsilon \
           or abs(self.h1.GetZaxis().GetXmax() - self.h2.GetZaxis().GetXmax()) >self.epsilon:
      return False
    return True

  def do_test(self):
    # fist check that binning matches
    if not self.checkBinningMatches():
      return test_codes["DIFF_BIN"]
    # then do the real check
    equal = 1
    nbins = getNbins(self.h1)
    n_ok_bins=0.0
    for ibin in xrange(0,nbins):
      ibin+=1
      h1bin=self.h1.GetBinContent(ibin)
      h2bin=self.h2.GetBinContent(ibin)
      bindiff=h1bin-h2bin

      binavg=.5*(h1bin+h2bin)

      if binavg==0 or 100*abs(bindiff)/binavg < self.tolerance:
        n_ok_bins+=1
        #print "Bin %i bin: bindiff %s" %(ibin,bindiff)
      else:
        print "-->Bin %i bin: bindiff %s (%s - %s )" %(ibin,bindiff,h1bin,h2bin)

      #if abs(bindiff)!=0 :
        #print "Bin %ibin: bindiff %s" %(ibin,bindiff)
    
    rank=n_ok_bins/nbins
    
    if rank!=1:
      print "%s nok: %s ntot: %s" %(self.h1.GetName(),n_ok_bins,nbins)
    
    return rank
#-------------------------------------------------------------------------------
Statistical_Tests={"KS":KS,
                   "Chi2":Chi2,
                   "BinToBin":BinToBin,
                   "BinToBin1percent":BinToBin1percent,
                   "Bin2Bin":BinToBin,
                   "b2b":BinToBin,}
#-------------------------------------------------------------------------------  

def ask_ok(prompt, retries=4, complaint='yes or no'):
    while True:
        ok = raw_input(prompt)
        if ok in ('y', 'ye', 'yes'):
            return True
        if ok in ('n', 'no'):
            return False
        retries = retries - 1
        if retries < 0:
            raise IOError('refusenik user')
        print complaint

#-------------------------------------------------------------------------------

class unpickler(Thread):
  def __init__(self,filename):
    Thread.__init__(self)
    self.filename=filename
    self.directory=""

  def run(self):
    print "Reading directory from %s" %(self.filename)
    ifile=open(self.filename,"rb")
    self.directory=load(ifile) 
    ifile.close()

#-------------------------------------------------------------------------------    

def wget(url):
  """ Fetch the WHOLE file, not in bunches... To be optimised.
  """
  opener=build_opener(X509CertOpen())  
  datareq = Request(url)
  datareq.add_header('authenticated_wget', "The ultimate wgetter")    
  bin_content=None
  try:
    filename=basename(url)  
    print "Checking existence of file %s on disk..."%filename
    if not isfile("./%s"%filename):      
      bin_content=opener.open(datareq).read()
    else:
      print "File %s exists, skipping.." %filename
  except ValueError:
    print "Error: Unknown url %s" %url
  
  if bin_content!=None:  
    ofile = open(filename, 'wb')
    ofile.write(bin_content)
    ofile.close()

#-------------------------------------------------------------------------------    


