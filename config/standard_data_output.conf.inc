///////////////////////////////////////////////////////////////////////////////////////
///////// > openSMILE configuration file<                            //////////////////
/////////                                                            //////////////////
///////// (c) audEERING UG (haftungsbeschränkt),                     //////////////////
/////////     All rights reserverd.                                  //////////////////
///////////////////////////////////////////////////////////////////////////////////////


/*
   This file can be included as data output file for standard feature
   extraction configuration files. It provides commandline options
   for the batch extraction GUI, and supports LLD and Functionals (summaries)
   saving.

   It requires the main extrator configuration file to provide the following
   data memory levels:  lld, lld_de, and func
 */

  //////////////////////////////////////////////////////////////////////
 ///////////////////  data output configuration  //////////////////////
//////////////////////////////////////////////////////////////////////

;;;;;;;;; output LLD features to CSV ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[componentInstances:cComponentManager]
instance[lldsink].type=cCsvSink
instance[arffsink].type=cArffSink
instance[csvsink].type=cCsvSink

[lldsink:cCsvSink]
reader.dmLevel = lld;lld_de
filename=\cm[lldoutput(D){?}:output csv file for LLD, disabled by default ?, only written if filename given]
instanceName=\cm[instname(N){unknown}:instance name]
append = \cm[appendlld{0}:set to 1 to append to the LLD output csv file, default is not to append]
timestamp = 1
number = 0
printHeader = 1
errorOnNoOutput = 1

[arffsink:cArffSink]
reader.dmLevel=func
 ; do not print "frameNumber" attribute to ARFF file
frameIndex = 0
frameTime = 0
 ; name of output file as commandline option
filename=\cm[output(O){output.arff}:name of WEKA Arff output file]
 ; name of @relation in the ARFF file
relation=\cm[relation{openSMILE_features}:arff relation attribute, feature set name and/or corpus name]
 ; name of the current instance (usually file name of input wave file)
instanceName=\cm[instname]
 ;; use this line instead of the above to always set the instance name to the
 ;; name of the input wave file
 ;instanceName=\cm[inputfile]
\{\cm[arfftargetsfile{arff_targets.conf.inc}:name of arff targets include file]}
append=\cm[appendstaticarff{1}:set to 0 to disable appending to an existing arff parameter summary file, given by the arffoutput option]
errorOnNoOutput = 0

[csvsink:cCsvSink]
reader.dmLevel = func
filename=\cm[csvoutput{?}:output CSV file for summarised parameters, set to a valid filename to enable this output sink, data is appended if file exists]
append=\cm[appendstaticcsv{1}:set to 0 to disable appending to an existing csv parameter summary file, given by the csvoutput option]
frameIndex=0
frameTime=0
instanceName=\cm[instname]
errorOnNoOutput = 1

