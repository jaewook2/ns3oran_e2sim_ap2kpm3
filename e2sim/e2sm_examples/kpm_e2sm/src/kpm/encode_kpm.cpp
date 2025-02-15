/*****************************************************************************
#                                                                            *
# Copyright 2020 AT&T Intellectual Property                                  *
#                                                                            *
# Licensed under the Apache License, Version 2.0 (the "License");            *
# you may not use this file except in compliance with the License.           *
# You may obtain a copy of the License at                                    *
#                                                                            *
#      http://www.apache.org/licenses/LICENSE-2.0                            *
#                                                                            *
# Unless required by applicable law or agreed to in writing, software        *
# distributed under the License is distributed on an "AS IS" BASIS,          *
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
# See the License for the specific language governing permissions and        *
# limitations under the License.                                             *
#                                                                            *
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include "encode_kpm.hpp"
#include "e2sim_defs.h" // added this 

using namespace std;

const char* performance_measurements[] = {
  "DRB.RlcSduTransmittedVolumeDL",
  "DRB.RlcSduTransmittedVolumeUL",
  "DRB.PerDataVolumeDLDist.Bin",
  "DRB.PerDataVolumeULDist.Bin",
  "DRB.RlcPacketDropRateDLDist",
  "DRB.PacketLossRateULDist",
  "L1M.DL-SS-RSRP.SSB",
  "L1M.DL-SS-SINR.SSB",
  "L1M.UL-SRS-RSRP"
  };

int NUMBER_MEASUREMENTS = 9;

// change this 
void encode_kpm_function_description(E2SM_KPM_RANfunction_Description_t* ranfunc_desc) {
  uint8_t* short_name = (uint8_t*)"ORAN-E2SM-KPM";
  uint8_t* func_desc = (uint8_t*)"KPM Monitor";
  uint8_t* e2sm_odi = (uint8_t*)"OID123";

  LOG_I("short_name: %s, func_desc: %s, e2sm_odi: %s", short_name, func_desc, e2sm_odi);
  ASN_STRUCT_RESET(asn_DEF_E2SM_KPM_RANfunction_Description, ranfunc_desc);


  ranfunc_desc->ranFunction_Name.ranFunction_ShortName.size = strlen((char*)short_name);
  ranfunc_desc->ranFunction_Name.ranFunction_ShortName.buf =
      (uint8_t*)calloc(strlen((char*)short_name), sizeof(uint8_t));
  memcpy(ranfunc_desc->ranFunction_Name.ranFunction_ShortName.buf, short_name,
         ranfunc_desc->ranFunction_Name.ranFunction_ShortName.size);

  ranfunc_desc->ranFunction_Name.ranFunction_Description.buf =
      (uint8_t*)calloc(1, strlen((char*)func_desc));
  memcpy(ranfunc_desc->ranFunction_Name.ranFunction_Description.buf, func_desc,
         strlen((char*)func_desc));
  ranfunc_desc->ranFunction_Name.ranFunction_Description.size = strlen((char*)func_desc);
  ranfunc_desc->ranFunction_Name.ranFunction_Instance = (long *)calloc(1, sizeof(long));
  *ranfunc_desc->ranFunction_Name.ranFunction_Instance = 1;

  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf =
      (uint8_t*)calloc(1, strlen((char*)e2sm_odi));
  memcpy(ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.buf, e2sm_odi,
         strlen((char*)e2sm_odi));
  ranfunc_desc->ranFunction_Name.ranFunction_E2SM_OID.size = strlen((char*)e2sm_odi);

  LOG_I("Initialize event trigger style list structure");
  
/*
  RIC_EventTriggerStyle_List_t *trigger_style =
    (RIC_EventTriggerStyle_List_t*)calloc(1, sizeof(RIC_EventTriggerStyle_List_t));
  trigger_style->ric_EventTriggerStyle_Type = 1;
  uint8_t* style_name = (uint8_t*)"Periodic Report";
  trigger_style->ric_EventTriggerStyle_Name.buf = (uint8_t*)calloc(1,strlen((char*)style_name));
  memcpy(trigger_style->ric_EventTriggerStyle_Name.buf, style_name, strlen((char*)style_name));
  trigger_style->ric_EventTriggerStyle_Name.size = strlen((char*)style_name);
  trigger_style->ric_EventTriggerFormat_Type = 1;
*/
  RIC_EventTriggerStyle_Item_t* trigger_style =
  (RIC_EventTriggerStyle_Item_t*)calloc(1, sizeof(RIC_EventTriggerStyle_Item_t));
  trigger_style->ric_EventTriggerStyle_Type = 1;
  uint8_t* style_name = (uint8_t*)"Periodic Report";
  trigger_style->ric_EventTriggerStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)style_name));
  memcpy(trigger_style->ric_EventTriggerStyle_Name.buf, style_name, strlen((char*)style_name));
  trigger_style->ric_EventTriggerStyle_Name.size = strlen((char*)style_name);
  trigger_style->ric_EventTriggerFormat_Type = 1;

  ranfunc_desc->ric_EventTriggerStyle_List =
    (E2SM_KPM_RANfunction_Description::E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List*)
        calloc(1, sizeof(E2SM_KPM_RANfunction_Description::E2SM_KPM_RANfunction_Description__ric_EventTriggerStyle_List));

  int ret = ASN_SEQUENCE_ADD(&ranfunc_desc->ric_EventTriggerStyle_List->list, trigger_style);

  LOG_I("Initialize report style structure");

  MeasurementInfo_Action_List_t* measInfo_Action_List =
      (MeasurementInfo_Action_List_t*)calloc(1, sizeof(MeasurementInfo_Action_List_t));

  for (int i = 0; i < NUMBER_MEASUREMENTS; i++) {
    uint8_t* metrics = (uint8_t *)performance_measurements[i];
    MeasurementInfo_Action_Item_t* measItem =
        (MeasurementInfo_Action_Item_t*)calloc(1, sizeof(MeasurementInfo_Action_Item_t));
    measItem->measName.buf = (uint8_t*)calloc(1, strlen((char*)metrics));
    memcpy(measItem->measName.buf, metrics, strlen((char*)metrics));

    measItem->measName.size = strlen((char*)metrics);

    measItem->measID = (MeasurementTypeID_t*)calloc(1, sizeof(MeasurementTypeID_t));
    *measItem->measID = i+1;

    ASN_SEQUENCE_ADD(&measInfo_Action_List->list, measItem);
  }

  RIC_ReportStyle_Item_t* report_style1 = (RIC_ReportStyle_Item_t*)calloc(1, sizeof(RIC_ReportStyle_Item_t));
  report_style1->ric_ReportStyle_Type = 1;
  uint8_t* buf5 = (uint8_t*)"E2 Node Measurement";
  report_style1->ric_ReportStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)buf5));
  memcpy(report_style1->ric_ReportStyle_Name.buf, buf5, strlen((char*)buf5));
  report_style1->ric_ReportStyle_Name.size = strlen((char*)buf5);
  report_style1->ric_ActionFormat_Type = 1;
  report_style1->ric_IndicationHeaderFormat_Type = 1;
  report_style1->ric_IndicationMessageFormat_Type = 1;
  report_style1->measInfo_Action_List = *measInfo_Action_List;

  RIC_ReportStyle_Item_t* report_style2 = (RIC_ReportStyle_Item_t*)calloc(1, sizeof(RIC_ReportStyle_Item_t));
  report_style2->ric_ReportStyle_Type = 2;
  uint8_t* buf6 = (uint8_t*)"E2 Node Measurement for a single UE";
  report_style2->ric_ReportStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)buf6));
  memcpy(report_style2->ric_ReportStyle_Name.buf, buf6, strlen((char*)buf6));
  report_style2->ric_ReportStyle_Name.size = strlen((char*)buf6);
  report_style2->ric_ActionFormat_Type = 2;
  report_style2->ric_IndicationHeaderFormat_Type = 1;
  report_style2->ric_IndicationMessageFormat_Type = 1;
  report_style2->measInfo_Action_List = *measInfo_Action_List;

  RIC_ReportStyle_Item_t* report_style3 = (RIC_ReportStyle_Item_t*)calloc(1, sizeof(RIC_ReportStyle_Item_t));
  report_style3->ric_ReportStyle_Type = 3;
  uint8_t* buf7 = (uint8_t*)"Condition-based, UE-level E2 Node Measurement";
  report_style3->ric_ReportStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)buf7));
  memcpy(report_style3->ric_ReportStyle_Name.buf, buf7, strlen((char*)buf7));
  report_style3->ric_ReportStyle_Name.size = strlen((char*)buf7);
  report_style3->ric_ActionFormat_Type = 3;
  report_style3->ric_IndicationHeaderFormat_Type = 1;
  report_style3->ric_IndicationMessageFormat_Type = 2;
  report_style3->measInfo_Action_List = *measInfo_Action_List;

  RIC_ReportStyle_Item_t* report_style4 =(RIC_ReportStyle_Item_t*)calloc(1, sizeof(RIC_ReportStyle_Item_t));
  report_style4->ric_ReportStyle_Type = 4;
  uint8_t* buf8 = (uint8_t*)"Common Condition-based, UE-level Measurement";
  report_style4->ric_ReportStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)buf8));
  memcpy(report_style4->ric_ReportStyle_Name.buf, buf8, strlen((char*)buf8));
  report_style4->ric_ReportStyle_Name.size = strlen((char*)buf8);
  report_style4->ric_ActionFormat_Type = 4;
  report_style4->ric_IndicationHeaderFormat_Type = 1;
  report_style4->ric_IndicationMessageFormat_Type = 3;
  report_style4->measInfo_Action_List = *measInfo_Action_List;

  RIC_ReportStyle_Item_t* report_style5 = (RIC_ReportStyle_Item_t*)calloc(1, sizeof(RIC_ReportStyle_Item_t));
  report_style5->ric_ReportStyle_Type = 5;
  uint8_t* buf9 = (uint8_t*)"E2 Node Measurement for multiple UEs";
  report_style5->ric_ReportStyle_Name.buf = (uint8_t*)calloc(1, strlen((char*)buf9));
  memcpy(report_style5->ric_ReportStyle_Name.buf, buf9, strlen((char*)buf9));
  report_style5->ric_ReportStyle_Name.size = strlen((char*)buf9);
  report_style5->ric_ActionFormat_Type = 5;
  report_style5->ric_IndicationHeaderFormat_Type = 1;
  report_style5->ric_IndicationMessageFormat_Type = 3;
  report_style5->measInfo_Action_List = *measInfo_Action_List; 

  ranfunc_desc->ric_ReportStyle_List = (E2SM_KPM_RANfunction_Description::E2SM_KPM_RANfunction_Description__ric_ReportStyle_List*) calloc(1, sizeof(E2SM_KPM_RANfunction_Description::E2SM_KPM_RANfunction_Description__ric_ReportStyle_List));
  
  ASN_SEQUENCE_ADD(&ranfunc_desc->ric_ReportStyle_List->list, report_style1);
  ASN_SEQUENCE_ADD(&ranfunc_desc->ric_ReportStyle_List->list, report_style2);
  ASN_SEQUENCE_ADD(&ranfunc_desc->ric_ReportStyle_List->list, report_style3);
  ASN_SEQUENCE_ADD(&ranfunc_desc->ric_ReportStyle_List->list, report_style4);
  ASN_SEQUENCE_ADD(&ranfunc_desc->ric_ReportStyle_List->list, report_style5);

  //xer_fprint(stderr, &asn_DEF_E2SM_KPM_RANfunction_Description, ranfunc_desc);

}


void kpm_report_indication_header_initialized(E2SM_KPM_IndicationHeader_t* ihead,
                                              uint8_t* plmnid_buf, uint8_t* sst_buf,
                                              uint8_t* sd_buf, long fqival, long qcival,
                                              uint8_t* nrcellid_buf, uint8_t* gnbid_buf,
                                              int gnbid_unused, uint8_t* cuupid_buf,
                                              uint8_t* duid_buf, uint8_t* cuupname_buf) {

  LOG_I("Start initializing mocked indication header");
  E2SM_KPM_IndicationHeader_Format1_t* ind_header =
  (E2SM_KPM_IndicationHeader_Format1_t*)calloc(1, sizeof(E2SM_KPM_IndicationHeader_Format1_t));

  ASN_STRUCT_RESET(asn_DEF_E2SM_KPM_IndicationMessage_Format1, ind_header);

  uint8_t* buf2 = (uint8_t*)"ORANSim";
  ind_header->senderName = (PrintableString_t*)calloc(1, sizeof(PrintableString_t));
  ind_header->senderName->buf = (uint8_t*)calloc(1, strlen((char*)buf2));
  memcpy(ind_header->senderName->buf, buf2, strlen((char*)buf2));
  ind_header->senderName->size = strlen((char*)buf2);

  uint8_t* buf3 = (uint8_t*)"simulator";
  ind_header->senderType = (PrintableString_t*)calloc(1, sizeof(PrintableString_t));
  ind_header->senderType->buf = (uint8_t*)calloc(1, strlen((char*)buf3));
  memcpy(ind_header->senderType->buf, buf3, strlen((char*)buf3));
  ind_header->senderType->size = strlen((char*)buf3);

  uint8_t* buf4 = (uint8_t*)"ORAN-SC";
  ind_header->vendorName = (PrintableString_t*)calloc(1, sizeof(PrintableString_t));
  ind_header->vendorName->buf = (uint8_t*)calloc(1, strlen((char*)buf4));
  memcpy(ind_header->vendorName->buf, buf4, strlen((char*)buf4));
  ind_header->vendorName->size = strlen((char*)buf4);

  uint8_t* buf = (uint8_t*)"20200613";
  TimeStamp_t* ts = (TimeStamp_t*)calloc(1, sizeof(TimeStamp_t));
  ts->buf = (uint8_t*)calloc(strlen((char*)buf), 1);
  ts->size = strlen((char*)buf);
  memcpy(ts->buf, buf, ts->size);

  ind_header->colletStartTime = *ts;
  if (ts) free(ts);

  ihead->indicationHeader_formats.present = E2SM_KPM_IndicationHeader__indicationHeader_formats_PR_indicationHeader_Format1;
  ihead->indicationHeader_formats.choice.indicationHeader_Format1 = ind_header;
}

void ue_meas_kpm_report_indication_message_initialized( E2SM_KPM_IndicationMessage_t* indicationmessage, 
                                                        uint8_t* nrcellid_buf, uint8_t* crnti_buf,
                                                        const uint8_t* serving_buf, const uint8_t* neighbor_buf) {

  MeasurementRecord_t* measDataItem_record = (MeasurementRecord_t*)calloc(1, sizeof(MeasurementRecord_t));
  for (size_t i = 0; i < NUMBER_MEASUREMENTS; i++)
  {
    MeasurementRecordItem_t* measDataRecordItem = (MeasurementRecordItem_t*)calloc(1, sizeof(MeasurementRecordItem_t));
    measDataRecordItem->present = MeasurementRecordItem_PR_integer;
    measDataRecordItem->choice.integer = 1;

    ASN_SEQUENCE_ADD(&measDataItem_record->list, measDataRecordItem);
  }

  MeasurementDataItem_t* measDataItem = (MeasurementDataItem_t*)calloc(1, sizeof(MeasurementDataItem_t));
  measDataItem->measRecord = *measDataItem_record;
  measDataItem->incompleteFlag = (long*)calloc(1, sizeof(long));
  *measDataItem->incompleteFlag = 0;

  MeasurementData_t* measData = (MeasurementData_t*)calloc(1, sizeof(MeasurementData_t));
  ASN_SEQUENCE_ADD(&measData->list, measDataItem);

  MeasurementInfoList_t* measList = (MeasurementInfoList_t*)calloc(1, sizeof(MeasurementInfoList_t));
  for (size_t i = 0; i < NUMBER_MEASUREMENTS; i++)
  {
    MeasurementLabel_t* measLabel = (MeasurementLabel_t*)calloc(1, sizeof(MeasurementLabel_t));
    measLabel->noLabel = (long *)calloc(1, sizeof(long));
    *measLabel->noLabel = 0;

    LabelInfoItem_t* labelItem = (LabelInfoItem_t*)calloc(1, sizeof(LabelInfoItem_t));
    labelItem->measLabel = *measLabel;

    LabelInfoList_t* labelList = (LabelInfoList_t*)calloc(1, sizeof(LabelInfoList_t));
    ASN_SEQUENCE_ADD(&labelList->list, labelItem);

    MeasurementType_t measType;
    measType.present = MeasurementType_PR_measID;
    uint8_t* metrics = (uint8_t *)performance_measurements[i];
    measType.choice.measName.buf = (uint8_t*)calloc(1, strlen((char*)metrics));
    memcpy(measType.choice.measName.buf, metrics, strlen((char*)metrics));
    measType.choice.measName.size = strlen((char*)metrics);

    MeasurementInfoItem_t* measItem = (MeasurementInfoItem_t*)calloc(1, sizeof(MeasurementInfoItem_t));
    measItem->measType = measType;
    measItem->labelInfoList = *labelList;
    if (labelList) free(labelList);

    ASN_SEQUENCE_ADD(&measList->list, measItem);
  }

  E2SM_KPM_IndicationMessage_Format1_t* format = (E2SM_KPM_IndicationMessage_Format1_t*)calloc(1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
  format->granulPeriod = (GranularityPeriod_t*)calloc(1, sizeof(GranularityPeriod_t));
  *format->granulPeriod = 1;
  format->measData = *measData;
  format->measInfoList = measList;

  E2SM_KPM_IndicationMessage__indicationMessage_formats_PR pres =
  E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;

  indicationmessage->indicationMessage_formats.present = pres;
  indicationmessage->indicationMessage_formats.choice.indicationMessage_Format1 = format;

  char error_buf[300] = {
    0,
  };
  size_t errlen = 0;

  int ret = asn_check_constraints(&asn_DEF_E2SM_KPM_IndicationMessage, indicationmessage, error_buf,
                                  &errlen);

  if (ret) {
    LOG_I("Constraint validation of indication message failed: %s", error_buf);
    exit(1);
  }
}

void cell_meas_kpm_report_indication_message_style_1_initialized(
      E2SM_KPM_IndicationMessage_t* indicationmessage, long fiveqi, long dl_prb_usage,
      long ul_prb_usage, uint8_t* nrcellid_buf, long* dl_prbs, long* ul_prbs) {

  LOG_I("Preparing indication message for cell measurement report");

  asn_codec_ctx_t* opt_cod;

  GranularityPeriod_t period = 1;
  MeasurementData_t* measData = (MeasurementData_t*)calloc(1, sizeof(MeasurementData_t));
  MeasurementDataItem_t* measDataItem =(MeasurementDataItem_t*)calloc(1, sizeof(MeasurementDataItem_t));
  MeasurementRecord_t* measDataItem_record =(MeasurementRecord_t*)calloc(1, sizeof(MeasurementRecord_t));
  MeasurementRecordItem_t* measDataRecordItem = (MeasurementRecordItem_t*)calloc(1, sizeof(MeasurementRecordItem_t));

  measDataRecordItem->present = MeasurementRecordItem_PR_integer;
  measDataRecordItem->choice.integer = 1;

  ASN_SEQUENCE_ADD(&measDataItem_record->list, measDataRecordItem);

  measDataItem->measRecord = *measDataItem_record;
  measDataItem->incompleteFlag = (long*)calloc(1, sizeof(long));
  *measDataItem->incompleteFlag = 0;
  ASN_SEQUENCE_ADD(&measData->list, measDataItem);

  E2SM_KPM_IndicationMessage_Format1_t* format = (E2SM_KPM_IndicationMessage_Format1_t*)calloc(
  1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
  format->granulPeriod = (GranularityPeriod_t*)calloc(1, sizeof(GranularityPeriod_t));
  *format->granulPeriod = period;
  format->measData = *measData;

  MeasurementInfoList_t* measList = (MeasurementInfoList_t*)calloc(1, sizeof(MeasurementInfoList_t));
  MeasurementInfoItem_t* measItem = (MeasurementInfoItem_t*)calloc(1, sizeof(MeasurementInfoItem_t));
  LabelInfoList_t* labelList = (LabelInfoList_t*)calloc(1, sizeof(LabelInfoList_t));
  LabelInfoItem_t* labelItem = (LabelInfoItem_t*)calloc(1, sizeof(LabelInfoItem_t));
  MeasurementLabel_t* measLabel = (MeasurementLabel_t*)calloc(1, sizeof(MeasurementLabel_t));

  uint8_t* plmnid_buf = (uint8_t*)"747";
  uint8_t* sst_buf = (uint8_t*)"1";
  uint8_t* sd_buf = (uint8_t*)"100";

  S_NSSAI_t* snssai = (S_NSSAI_t*)calloc(1, sizeof(S_NSSAI_t));
  snssai->sST.buf = (uint8_t*)calloc(strlen((char*)sst_buf), sizeof(uint8_t));
  snssai->sST.size = strlen((char*)sst_buf);
  memcpy(snssai->sST.buf, sst_buf, strlen((char*)sst_buf));

  snssai->sD = (SD_t*)calloc(1, sizeof(SD_t));
  snssai->sD->buf = (uint8_t*)calloc(strlen((char*)sd_buf), sizeof(uint8_t));
  snssai->sD->size = strlen((char*)sd_buf);
  memcpy(snssai->sD->buf, sd_buf, strlen((char*)sd_buf));

  int plmnid_size = strlen((char*)plmnid_buf);
  PLMNIdentity_t* plmnidstr = (PLMNIdentity_t*)calloc(1, sizeof(PLMNIdentity_t));
  plmnidstr->buf = (uint8_t*)calloc(plmnid_size, 1);
  plmnidstr->size = plmnid_size;
  memcpy(plmnidstr->buf, plmnid_buf, plmnidstr->size);

  measLabel->plmnID = plmnidstr;
  measLabel->sliceID = snssai;

  labelItem->measLabel = *measLabel;
  ASN_SEQUENCE_ADD(&labelList->list, labelItem);
  measItem->labelInfoList = *labelList;
  if (labelList) free(labelList);
  MeasurementType_t measType;
  measType.present = MeasurementType_PR_measID;
  measType.choice.measID = 1;
  measItem->measType = measType;
  ASN_SEQUENCE_ADD(&measList->list, measItem);

  format->measInfoList = measList;

  E2SM_KPM_IndicationMessage__indicationMessage_formats_PR pres = E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;

  indicationmessage->indicationMessage_formats.present = pres;
  indicationmessage->indicationMessage_formats.choice.indicationMessage_Format1 = format;

  char error_buf[300] = {
        0,
      };
  size_t errlen = 0;

  int ret = asn_check_constraints(&asn_DEF_E2SM_KPM_IndicationMessage, indicationmessage, error_buf, &errlen);

  if (ret) {
    LOG_I("Constraint validation of indication message failed: %s\n", error_buf);
    exit(1);
  }
}

void encode_kpm_report_style1(E2SM_KPM_IndicationMessage_t* indicationmessage) {

  long fiveqi = 8;
  long dl_prb_usage = 50;
  long ul_prb_usage = 70;

  asn_codec_ctx_t* opt_cod;

  GranularityPeriod_t period = 1;
  MeasurementData_t measData;
  MeasurementDataItem_t measDataItem;
  MeasurementRecord_t measDataItem_record;
  MeasurementRecordItem_t measDataRecordItem;

  measDataRecordItem.present = MeasurementRecordItem_PR_real;
  measDataRecordItem.choice.real = 1.5;

  ASN_SEQUENCE_ADD(&measDataItem_record.list, &measDataRecordItem);

  measDataItem.measRecord = measDataItem_record;
  ASN_SEQUENCE_ADD(&measData.list, &measDataItem);

  E2SM_KPM_IndicationMessage_Format1_t* format = (E2SM_KPM_IndicationMessage_Format1_t*)calloc(1, sizeof(E2SM_KPM_IndicationMessage_Format1_t));
  ASN_STRUCT_RESET(asn_DEF_E2SM_KPM_IndicationMessage_Format1, format);

  format->granulPeriod = &period;
  format->measData = measData;

  E2SM_KPM_IndicationMessage__indicationMessage_formats_PR pres =
  E2SM_KPM_IndicationMessage__indicationMessage_formats_PR_indicationMessage_Format1;

  indicationmessage->indicationMessage_formats.present = pres;

  indicationmessage->indicationMessage_formats.choice.indicationMessage_Format1 = format;
  if (format) free(format);

  char error_buf[300] = {
  0,
  };
  size_t errlen = 0;

  int ret = asn_check_constraints(&asn_DEF_E2SM_KPM_IndicationMessage, indicationmessage, error_buf, &errlen);

  if (ret) {
    LOG_I("Constraint validation of indication message failed: %s\n", error_buf);
    exit(1);
  }

  uint8_t e2smbuffer[8192] = {
    0,
  };
  size_t e2smbuffer_size = 8192;

  asn_enc_rval_t er = asn_encode_to_buffer(opt_cod, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2SM_KPM_IndicationMessage,
                                            indicationmessage, e2smbuffer, e2smbuffer_size);

  if (er.encoded == -1) {
    LOG_E("Failed to serialize message. Detail: %s.\n", asn_DEF_E2SM_KPM_IndicationMessage.name);
    exit(1);
  } else if (er.encoded > e2smbuffer_size) {
    LOG_E("Buffer of size %zu is too small for %s, need %zu\n", e2smbuffer_size, asn_DEF_E2SM_KPM_IndicationMessage.name, er.encoded);
    exit(1);
  }
}

void encode_kpm(E2SM_KPM_IndicationMessage_t* indicationmessage) {
}