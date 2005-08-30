/*
 *  plugin.cc
 *  CMSSW
 *
 *  Created by Chris Jones on 7/24/05.
 *
 */

#include "CondCore/PluginSystem/interface/registration_macros.h"
#include "CondFormats/EcalObjects/interface/EcalPedestals.h"
#include "CondFormats/DataRecord/interface/EcalPedestalsRcd.h"
#include "CondFormats/EcalObjects/interface/EcalMapping.h"
#include "CondFormats/DataRecord/interface/EcalMappingRcd.h"

DEFINE_SEAL_MODULE();
REGISTER_PLUGIN(EcalPedestalsRcd,EcalPedestals);
REGISTER_PLUGIN(EcalMappingRcd,EcalMapping);
