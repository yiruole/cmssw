#include "RecoLuminosity/ROOTSchema/interface/ROOTFileBase.h"
#include "RecoLuminosity/ROOTSchema/interface/FileToolKit.h"

#include <sstream>
#include <iostream>
#include <typeinfo>
#include <iomanip>
#include <vector>
#include <ctime>

#include <stddef.h>

// mkdir
#include <sys/types.h>
#include <sys/stat.h>

#include <TROOT.h>
#include <TChain.h>
#include <TTree.h>
#include <TFile.h>

HCAL_HLX::ROOTFileBase::ROOTFileBase():filePrefix_("CMS_LUMI_RAW"),
				       dirName_("./"),
				       bEtSumOnly_(false),
				       date_(""),
				       fileType_("RAW"),
				       previousRun_(0)
{

  Init();
}

HCAL_HLX::ROOTFileBase::~ROOTFileBase(){

  CleanUp();
}

void HCAL_HLX::ROOTFileBase::Init(){

  lumiSection_     = new HCAL_HLX::LUMI_SECTION;

  Threshold_       = new HCAL_HLX::LUMI_THRESHOLD;
  L1Trigger_       = new HCAL_HLX::LEVEL1_TRIGGER;
  HLT_             = new HCAL_HLX::HLT;
  TriggerDeadtime_ = new HCAL_HLX::TRIGGER_DEADTIME;
  RingSet_         = new HCAL_HLX::LUMI_HF_RING_SET;
}

void HCAL_HLX::ROOTFileBase::CleanUp(){

  delete lumiSection_;
  
  delete Threshold_;
  delete L1Trigger_;
  delete HLT_;
  delete TriggerDeadtime_; 
  delete RingSet_;
}

void HCAL_HLX::ROOTFileBase::SetDir(const std::string& dirName){

  dirName_ = dirName;
}

void HCAL_HLX::ROOTFileBase::SetFileType(const std::string &fileType){
  
  if( !( fileType == "RAW" || fileType == "VDM" || fileType == "ET")){
    fileType_ == "RAW";
  }else{
    fileType_ = fileType;
  }
  
  if( fileType_ == "ET" && !bEtSumOnly_ ){
    SetEtSumOnly( true );
  }
  
  if( fileType_ != "ET" && bEtSumOnly_ ){
    SetEtSumOnly(false);
  }  
}

void HCAL_HLX::ROOTFileBase::SetFileName(unsigned int runNumber, 
					 unsigned int sectionNumber, 
					 bool bCMSLive,
					 const std::string &type,
					 const std::string &date){
 
  if( previousRun_ != runNumber){
    runDir_.str(std::string(""));
    runDir_ << std::setw(9) << std::setfill('0') << runNumber << "/";
    MakeDir(runDir_.str(), 0777 );
  }

  if( date == "" ){
    date_ = TimeStampYYYYMMDD();
  }else{
    date_ = date;
  }

  if( type != "" ){
    SetFileType(type);
  }
  
  std::stringstream fileName;
  fileName.str(std::string());
  
  fileName << "CMS_LUMI_" << fileType_ << "_" << date_ << "_"
	   << std::setfill('0') << std::setw(9) << runNumber << "_"
	   << std::setfill('0') << std::setw(4) << sectionNumber << "_"
	   << bCMSLive << ".root";
  
  fileName_ = fileName.str();
  
  CreateTree();
}

void HCAL_HLX::ROOTFileBase::SetFileName(const HCAL_HLX::LUMI_SECTION &lumiSection){

  SetFileName( lumiSection.hdr.runNumber,
	       lumiSection.hdr.sectionNumber,
	       lumiSection.hdr.bCMSLive,
	       fileType_,
	       TimeStampYYYYMMDD(lumiSection.hdr.timestamp));
}

void HCAL_HLX::ROOTFileBase::SetEtSumOnly( bool bEtSumOnly ){

  bEtSumOnly_ = bEtSumOnly;
  if( fileType_ != "ET" && bEtSumOnly_ ){
    SetFileType("ET");
  }
} 
