#ifndef CommonTools_Utils_findMethod_h
#define CommonTools_Utils_findMethod_h
#include "Reflex/Member.h"
#include "Reflex/Type.h"
#include <string>
#include "CommonTools/Utils/src/AnyMethodArgument.h"

namespace reco {
  // second pair member is true if a reference is found 
  // of type edm::Ref, edm::RefToBase or edm::Ptr
  std::pair<Reflex::Member, bool> findMethod(const Reflex::Type & type,
						   const std::string & name,
						   const std::vector<reco::parser::AnyMethodArgument> &args,
                                                   std::vector<reco::parser::AnyMethodArgument> &fixuppedArgs,
                                                   const char* where,
                                                   int& oError);
}

#endif