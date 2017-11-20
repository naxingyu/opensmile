// This file is part of openSMILE.
// (c) 2016 by audEERING GmbH. All rights reserved.
// See file COPYING for details on licensing terms.
//
// Written by Florian Eyben. fe@audeering.com

[componentInstances:cComponentManager]
instance[bandEnergy].type=cSimpleMessageSender

[bandEnergy:cSimpleMessageSender]
reader.dmLevel = melspec;energyB
sendPeriodically = 2
messageName = energy_act
messageType = energy
messageRecp = jniMessageInterface
enableDebugSender = 0
