

/*****************************************************************************
#                                                                            *
# Copyright 2020 AT&T Intellectual Property                                  *
# Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved.      *
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

#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include <iterator>
#include <vector>
#include <GlobalE2node-eNB-ID.h>
#include <GlobalE2node-ng-eNB-ID.h>
#include <GlobalE2node-en-gNB-ID.h>


#include "encode_e2apv1.hpp"

extern "C" {

#include "e2ap_asn1c_codec.h"
#include "GlobalE2node-ID.h"
#include "GlobalE2node-gNB-ID.h"
#include "GlobalgNB-ID.h"
#include "OCTET_STRING.h"
#include "asn_application.h"
#include "GNB-ID-Choice.h"
#include "ProtocolIE-Field.h"
#include "E2setupRequest.h"
#include "RICaction-ToBeSetup-Item.h"
#include "RICactions-ToBeSetup-List.h"
#include "RICeventTriggerDefinition.h"
#include "RICsubscriptionRequest.h"
#include "RICsubscriptionResponse.h"
#include "ProtocolIE-SingleContainer.h"
#include "RANfunctions-List.h"
#include "RICindication.h"
#include "RICsubsequentActionType.h"
#include "RICsubsequentAction.h"  
#include "RICtimeToWait.h"
#include "E2nodeComponentInterfaceNG.h"
}

long encoding::get_function_id_from_subscription(E2AP_PDU_t *e2ap_pdu) {

    RICsubscriptionRequest_t orig_req =
            e2ap_pdu->choice.initiatingMessage->value.choice.RICsubscriptionRequest;


    int count = orig_req.protocolIEs.list.count;
    int size = orig_req.protocolIEs.list.size;

    RICsubscriptionRequest_IEs_t **ies = (RICsubscriptionRequest_IEs_t**)orig_req.protocolIEs.list.array;

    LOG_I("Number of IEs in original request: %d, size: %d", count, size);

    RICsubscriptionRequest_IEs__value_PR pres;

    long func_id;

    for (int i = 0; i < count; i++) {
        RICsubscriptionRequest_IEs_t *next_ie = ies[i];
        pres = next_ie->value.present;
        LOG_D("Next present value: %d, pres RAN func id: %d", pres, RICsubscriptionRequest_IEs__value_PR_RANfunctionID);

        if (pres == RICsubscriptionRequest_IEs__value_PR_RANfunctionID) {
            LOG_E("equal pres to ranfuncid");
            func_id = next_ie->value.choice.RANfunctionID;
        }

    }

    LOG_I("After loop, func_id is %d", func_id);

    return func_id;

}
//** 대충 통과 0211 */
// 밑에는 없는데?
long encoding::get_function_id_from_control_request(E2AP_PDU_t *pdu) {
    auto *orig_req = (RICcontrolRequest_t *) &pdu->choice.initiatingMessage->value.choice.RICcontrolRequest;
    for (RICcontrolRequest_IEs_t *ie = *orig_req->protocolIEs.list.array;
         ie != nullptr; ie++) {
        if (ie->value.present == RICcontrolRequest_IEs__value_PR_RANfunctionID) {
            return ie->value.choice.RANfunctionID;
        }
    }
    return -1;
}

void encoding::generate_e2apv1_service_update(E2AP_PDU_t *e2ap_pdu, std::vector<encoding::ran_func_info> all_funcs) {
    char *ran_function_op_type = getenv("RAN_FUNCTION_OP_TYPE");
    LOG_D("Ran funciton : %s", ran_function_op_type);
    ProtocolIE_ID_t prID;
    if (ran_function_op_type != NULL) {
        if (strcmp(ran_function_op_type, "ADD") == 0) {
            prID = ProtocolIE_ID_id_RANfunctionsAdded;
        } else if (strcmp(ran_function_op_type, "DELETE")) {
            prID = ProtocolIE_ID_id_RANfunctionsDeleted;
        }
    } else {
        prID = ProtocolIE_ID_id_RANfunctionsModified;
    }


    RICserviceUpdate_IEs_t *e2serviceUpdateList = (RICserviceUpdate_IEs_t *) calloc(1, sizeof(RICserviceUpdate_IEs_t));
    e2serviceUpdateList->id = prID;
    e2serviceUpdateList->criticality = Criticality_reject;
    e2serviceUpdateList->value.present = RICserviceUpdate_IEs__value_PR_RANfunctions_List;


    for (int i = 0; i < all_funcs.size(); i++) {

        encoding::ran_func_info nextRanFunc = all_funcs.at(i);
        long nextRanFuncId = nextRanFunc.ranFunctionId;
        OCTET_STRING_t *nextRanFuncDesc = nextRanFunc.ranFunctionDesc;
        long nextRanFuncRev = nextRanFunc.ranFunctionRev;

        auto *itemIes = (RANfunction_ItemIEs_t *) calloc(1, sizeof(RANfunction_ItemIEs_t));
        itemIes->id = ProtocolIE_ID_id_RANfunction_Item;
        itemIes->criticality = Criticality_reject;
        itemIes->value.present = RANfunction_ItemIEs__value_PR_RANfunction_Item;
        itemIes->value.choice.RANfunction_Item.ranFunctionID = 1;

        itemIes->value.choice.RANfunction_Item.ranFunctionDefinition = *nextRanFuncDesc;
        itemIes->value.choice.RANfunction_Item.ranFunctionRevision = nextRanFuncRev + 1;

        ASN_SEQUENCE_ADD(&e2serviceUpdateList->value.choice.RANfunctions_List.list, itemIes);
    }

    RICserviceUpdate_t *ricServiceUpdate = (RICserviceUpdate_t *) calloc(1, sizeof(RICserviceUpdate_t));
    ASN_SEQUENCE_ADD(&ricServiceUpdate->protocolIEs.list, e2serviceUpdateList);

    InitiatingMessage_t *initiatingMessage = (InitiatingMessage_t *) calloc(1, sizeof(InitiatingMessage_t));
    initiatingMessage->criticality = Criticality_reject;
    initiatingMessage->procedureCode = ProcedureCode_id_RICserviceUpdate;
    initiatingMessage->value.present = InitiatingMessage__value_PR_RICserviceUpdate;
    initiatingMessage->value.choice.RICserviceUpdate = *ricServiceUpdate;

    E2AP_PDU_PR pres6 = E2AP_PDU_PR_initiatingMessage;
    e2ap_pdu->present = pres6;
    e2ap_pdu->choice.initiatingMessage = initiatingMessage;
}
// For ns o ran?
void encoding::generate_e2apv1_setup_request_parameterized(E2AP_PDU_t *e2ap_pdu, std::vector<ran_func_info> all_funcs, uint8_t * gnb_id, uint8_t * plmn_id) {
    // CGI encoding
    // jlee0211: below is for simulations
    BIT_STRING_t *gnb_bstring = (BIT_STRING_t *) calloc(1, sizeof(BIT_STRING_t));;
    gnb_bstring->buf = (uint8_t *) calloc(1, 4);
    gnb_bstring->size = 4;
    memcpy(gnb_bstring->buf, gnb_id, 4);
    gnb_bstring->bits_unused = 0; // JLee ? To be changed?
    //gnb_bstring->buf[0] = 0xB5; //at K-release
    //gnb_bstring->buf[1] = 0xC6; //at K-release
    //gnb_bstring->buf[2] = 0x77; //at K-release
    //gnb_bstring->buf[3] = 0x88; //at K-release
    // gnb_bstring->bits_unused = 3; //at K-release

    // PLMNID is the same for every CGI
    // jlee0211: below is for simulations
    // uint8_t *buf2 = (uint8_t *)"747"; //at K-release
    OCTET_STRING_t *plmnid = (OCTET_STRING_t *) calloc(1, sizeof(OCTET_STRING_t));
    plmnid->buf = (uint8_t *) calloc(1, 3);
    memcpy(plmnid->buf, plmn_id, 3);
    plmnid->size = 3;

    GNB_ID_Choice_t *gnbchoice = (GNB_ID_Choice_t *) calloc(1, sizeof(GNB_ID_Choice_t));
    GNB_ID_Choice_PR pres2 = GNB_ID_Choice_PR_gnb_ID;
    gnbchoice->present = pres2;
    gnbchoice->choice.gnb_ID = *gnb_bstring;
    if (gnb_bstring) free(gnb_bstring);

    GlobalgNB_ID_t *gnb = (GlobalgNB_ID_t *) calloc(1, sizeof(GlobalgNB_ID_t));
    gnb->plmn_id = *plmnid;
    gnb->gnb_id = *gnbchoice;
    if (plmnid) free(plmnid);
    if (gnbchoice) free(gnbchoice);

    GlobalE2node_gNB_ID_t *e2gnb = (GlobalE2node_gNB_ID_t *) calloc(1, sizeof(GlobalE2node_gNB_ID_t));
    e2gnb->global_gNB_ID = *gnb;
    if (gnb) free(gnb);


    GlobalE2node_ID_t *globale2nodeid = (GlobalE2node_ID_t*)calloc(1, sizeof(GlobalE2node_ID_t));
    GlobalE2node_ID_PR pres;
    pres = GlobalE2node_ID_PR_gNB;
    globale2nodeid->present = pres;
    globale2nodeid->choice.gNB = e2gnb;

    // jlee0210 to be check 숫자 vs Criticality_reject
    E2setupRequestIEs_t *e2setuprid = (E2setupRequestIEs_t*)calloc(1, sizeof(E2setupRequestIEs_t));
    E2setupRequestIEs__value_PR pres3;
    pres3 = E2setupRequestIEs__value_PR_GlobalE2node_ID;
    e2setuprid->id = 3;
    e2setuprid->criticality = 0;
    e2setuprid->value.choice.GlobalE2node_ID = *globale2nodeid;
    e2setuprid->value.present = pres3;
    if(globale2nodeid) free(globale2nodeid);

    auto *e2txid = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
    e2txid->id = ProtocolIE_ID_id_TransactionID;
    e2txid-> criticality = 0;
    e2txid->value.present = E2setupRequestIEs__value_PR_TransactionID;
    e2txid->value.choice.TransactionID = 1; // To be updated?

    auto *ranFlistIEs = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
    ASN_STRUCT_RESET(asn_DEF_E2setupRequestIEs, ranFlistIEs);
    ranFlistIEs->criticality = 0;
    ranFlistIEs->id = ProtocolIE_ID_id_RANfunctionsAdded;
    ranFlistIEs->value.present = E2setupRequestIEs__value_PR_RANfunctions_List;
  
    for (int i = 0; i < all_funcs.size(); i++) {

        ran_func_info nextRanFunc = all_funcs.at(i);
        long nextRanFuncId = nextRanFunc.ranFunctionId;
        OCTET_STRING_t *nextRanFuncDesc = nextRanFunc.ranFunctionDesc;
        long nextRanFuncRev = nextRanFunc.ranFunctionRev;

        auto *itemIes = (RANfunction_ItemIEs_t *)calloc(1, sizeof(RANfunction_ItemIEs_t));
        itemIes->id = ProtocolIE_ID_id_RANfunction_Item;
        itemIes->criticality = Criticality_reject;
        itemIes->value.present = RANfunction_ItemIEs__value_PR_RANfunction_Item;
        itemIes->value.choice.RANfunction_Item.ranFunctionID = nextRanFuncId;
        itemIes->value.choice.RANfunction_Item.ranFunctionOID = RANfunctionOID_t(*(nextRanFunc.ranFunctionOId)); // jlee0211: Newlly added
        int ranFuncLength = strlen((char*)nextRanFuncDesc); // jlee0211: This is not used 

        itemIes->value.choice.RANfunction_Item.ranFunctionDefinition = *nextRanFuncDesc;
        itemIes->value.choice.RANfunction_Item.ranFunctionRevision = nextRanFuncRev;
        
        ASN_SEQUENCE_ADD(&ranFlistIEs->value.choice.RANfunctions_List.list, itemIes);

    }

    auto *e2configIE = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
    e2configIE->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition;
    e2configIE->criticality = Criticality_reject;
    e2configIE->value.present = E2setupRequestIEs__value_PR_E2nodeComponentConfigAddition_List;
   
    auto *e2configAdditionItem = (E2nodeComponentConfigAddition_ItemIEs_t *)calloc(1, sizeof(E2nodeComponentConfigAddition_ItemIEs_t));
    e2configAdditionItem->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition_Item;
    e2configAdditionItem->criticality = Criticality_reject;
    e2configAdditionItem->value.present = E2nodeComponentConfigAddition_ItemIEs__value_PR_E2nodeComponentConfigAddition_Item;

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentInterfaceType = E2nodeComponentInterfaceType_ng;
    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.present = E2nodeComponentID_PR_e2nodeComponentInterfaceTypeNG;

    auto *intfNG = (E2nodeComponentInterfaceNG_t *) calloc(1, sizeof(E2nodeComponentInterfaceNG_t));

    OCTET_STRING_t nginterf;
    nginterf.buf = (uint8_t*)calloc(1,8);
    memcpy(nginterf.buf, (uint8_t *)"nginterf", 8);

    nginterf.size = 8;
    intfNG->amf_name = (AMFName_t)(nginterf);

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.choice.e2nodeComponentInterfaceTypeNG = intfNG;

    OCTET_STRING_t reqPart;
    reqPart.buf = (uint8_t*)calloc(1,7);
    memcpy(reqPart.buf, (uint8_t *)"reqpart", 7);
    reqPart.size = 7;

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentRequestPart = reqPart;

    OCTET_STRING_t resPart;
    resPart.buf = (uint8_t*)calloc(1,7);
    memcpy(resPart.buf, (uint8_t *)"respart", 7);
    resPart.size = 7;

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentResponsePart = resPart;

    ASN_SEQUENCE_ADD(&e2configIE->value.choice.RANfunctions_List.list, e2configAdditionItem);

    E2setupRequest_t  *e2setupreq = (E2setupRequest_t *) calloc(1, sizeof(E2setupRequest_t));
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2setuprid);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, ranFlistIEs);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2txid);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2configIE);


    InitiatingMessage__value_PR pres4;
    pres4 = InitiatingMessage__value_PR_E2setupRequest;
    InitiatingMessage_t  *initmsg = (InitiatingMessage_t *) calloc(1, sizeof(InitiatingMessage_t));

    initmsg->procedureCode = ProcedureCode_id_E2setup;
    initmsg->criticality = Criticality_reject;
    initmsg->value.present = pres4;
    initmsg->value.choice.E2setupRequest = *e2setupreq;
    if (e2setupreq) free(e2setupreq);

    E2AP_PDU_PR pres5;
    pres5 = E2AP_PDU_PR_initiatingMessage;   

    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.initiatingMessage = initmsg;
}

// This is not in K
OCTET_STRING_t encoding::most_copy_src_ostring_to_dst_ostring ()
{
    const int _size = 2;
    OCTET_STRING_t dst = {0} ;
    dst.buf = (uint8_t*)calloc(_size, sizeof(uint8_t));
    dst.buf[0] = 0x00;
    dst.buf[1] = 0x00;
    dst.size = _size ;
    return dst ;
}

// This is not in K
void encoding::generate_e2apv1_subscription_delete_acknowledge(E2AP_PDU_t *delete_resp_pdu, long reqRequestorId, long reqInstanceId, long ranFuncionId) {

    // RIC request ID. Mandatory
    auto *req_id_ie = (RICsubscriptionDeleteResponse_IEs *) calloc(1, sizeof(RICsubscriptionDeleteResponse_IEs_t));
    auto *ricRequestId = (RICrequestID_t *) calloc(1, sizeof(RICrequestID_t));
    ricRequestId->ricRequestorID= reqRequestorId; //1021 ;
    ricRequestId->ricInstanceID= reqInstanceId; //0;
    req_id_ie->id = ProtocolIE_ID_id_RICrequestID;
    req_id_ie->criticality = Criticality_reject;
    req_id_ie->value.present = RICsubscriptionDeleteResponse_IEs__value_PR_RICrequestID;
    req_id_ie->value.choice.RICrequestID = *ricRequestId;

    // RAN Function ID. Mandatory
    auto *ran_func_id_ie = (RICsubscriptionDeleteResponse_IEs *) calloc(1, sizeof(RICsubscriptionDeleteResponse_IEs_t));
    auto *ranFunctionId = (RANfunctionID_t *) calloc(1, sizeof(RANfunctionID_t));
    *ranFunctionId = ranFuncionId; //2;
    ran_func_id_ie->id = ProtocolIE_ID_id_RANfunctionID;
    ran_func_id_ie->criticality = Criticality_reject;
    ran_func_id_ie->value.present = RICsubscriptionDeleteResponse_IEs__value_PR_RANfunctionID;
    ran_func_id_ie->value.choice.RANfunctionID = *ranFunctionId;

    // Message Type. Mandatory
    auto *riCsubscriptionDeleteAcknowledge = (RICsubscriptionDeleteResponse_t *) calloc(1, sizeof(RICsubscriptionDeleteResponse_t));
    ASN_SEQUENCE_ADD(&riCsubscriptionDeleteAcknowledge->protocolIEs.list, req_id_ie);
    ASN_SEQUENCE_ADD(&riCsubscriptionDeleteAcknowledge->protocolIEs.list, ran_func_id_ie);

    auto *successOutcome = (SuccessfulOutcome_t *) calloc(1, sizeof(SuccessfulOutcome_t));
    successOutcome->procedureCode = ProcedureCode_id_RICsubscriptionDelete;
    successOutcome->criticality = Criticality_reject;
    successOutcome->value.present = SuccessfulOutcome__value_PR_RICsubscriptionDeleteResponse;
    successOutcome->value.choice.RICsubscriptionDeleteResponse = *riCsubscriptionDeleteAcknowledge;

    delete_resp_pdu->present = E2AP_PDU_PR_successfulOutcome;
    delete_resp_pdu->choice.successfulOutcome = successOutcome;

}

// This is not in K

void encoding::generate_e2apv1_ric_control_acknowledge(E2AP_PDU_t *control_resp_pdu, 
                                                    long _reqRequestorId, 
                                                    long _reqInstanceId, 
                                                    long _ranFuncionId) {

    LOG_D(" generate_e2apv1_ric_control_acknowledge ");


    // TODO
    // RIC Request ID. Mandatory
    auto *req_id_ie = (RICcontrolAcknowledge_IEs *) calloc(1, sizeof(RICcontrolAcknowledge_IEs_t));
    auto *ricRequestId = (RICrequestID_t *) calloc(1, sizeof(RICrequestID_t));
    ricRequestId->ricRequestorID= _reqRequestorId; //1023 ;
    ricRequestId->ricInstanceID= _reqInstanceId; //0 ;
    req_id_ie->id = ProtocolIE_ID_id_RICrequestID;
    req_id_ie->criticality = Criticality_reject;
    req_id_ie->value.present = RICcontrolAcknowledge_IEs__value_PR_RICrequestID;
    req_id_ie->value.choice.RICrequestID = *ricRequestId;


    // TODO
    //RAN Function ID. Mandatory
    auto *ran_func_id_ie = (RICcontrolAcknowledge_IEs *) calloc(1, sizeof(RICcontrolAcknowledge_IEs_t));
    auto *ranFunctionId = (RANfunctionID_t *) calloc(1, sizeof(RANfunctionID_t));
    *ranFunctionId = _ranFuncionId; //2;
    ran_func_id_ie->id = ProtocolIE_ID_id_RANfunctionID;
    ran_func_id_ie->criticality = Criticality_reject;
    ran_func_id_ie->value.present = RICcontrolAcknowledge_IEs__value_PR_RANfunctionID;
    ran_func_id_ie->value.choice.RANfunctionID = *ranFunctionId;
     /*jlee 0215 나중에 check
    // RIC Control status. Mandatory
    // TODO fill
    auto *ric_control_status_ie = (RICcontrolAcknowledge_IEs *) calloc(1, sizeof(RICcontrolAcknowledge_IEs_t));
    auto *ricControlStatus = (RICcontrolStatus_t *) calloc(1, sizeof(RICcontrolStatus_t));
    *ricControlStatus = 0;
    ric_control_status_ie->id = ProtocolIE_ID_id_RICcontrolStatus;
    ric_control_status_ie->criticality = Criticality_reject;
    ric_control_status_ie->value.present = RICcontrolAcknowledge_IEs__value_PR_RICcontrolStatus;
    ric_control_status_ie->value.choice.RICcontrolStatus = *ricControlStatus;
   */

    // RIC Control Outcome. Optional
    auto *ric_control_outcome_ie = (RICcontrolAcknowledge_IEs *) calloc(1, sizeof(RICcontrolAcknowledge_IEs_t));
    auto *ricControlOutcome = (RICcontrolOutcome_t *) calloc(1, sizeof(RICcontrolOutcome_t));
    *ricControlOutcome = encoding::most_copy_src_ostring_to_dst_ostring();
    if(true ) {
        ric_control_outcome_ie->id = ProtocolIE_ID_id_RICcontrolOutcome;
        ric_control_outcome_ie->criticality = Criticality_reject;
        ric_control_outcome_ie->value.present = RICcontrolAcknowledge_IEs__value_PR_RICcontrolOutcome;
        ric_control_outcome_ie->value.choice.RICcontrolOutcome = *ricControlOutcome;
    }

    auto *riCcontrolAcknowledge = (RICcontrolAcknowledge_t *) calloc(1, sizeof(RICcontrolAcknowledge_t));
    ASN_SEQUENCE_ADD(&riCcontrolAcknowledge->protocolIEs.list, req_id_ie);
    ASN_SEQUENCE_ADD(&riCcontrolAcknowledge->protocolIEs.list, ran_func_id_ie);
    //ASN_SEQUENCE_ADD(&riCcontrolAcknowledge->protocolIEs.list, ric_control_status_ie); jlee 0215 나중에 check
    ASN_SEQUENCE_ADD(&riCcontrolAcknowledge->protocolIEs.list, ric_control_outcome_ie);

    auto *successOutcome = (SuccessfulOutcome_t *) calloc(1, sizeof(SuccessfulOutcome_t));
    successOutcome->procedureCode = ProcedureCode_id_RICcontrol;
    successOutcome->criticality = Criticality_reject;
    successOutcome->value.present = SuccessfulOutcome__value_PR_RICcontrolAcknowledge;
    successOutcome->value.choice.RICcontrolAcknowledge = *riCcontrolAcknowledge;

    control_resp_pdu->present = E2AP_PDU_PR_successfulOutcome;
    control_resp_pdu->choice.successfulOutcome = successOutcome;

    char *error_buf = (char *) calloc(300, sizeof(char));
    size_t errlen;
    LOG_D("CHECK ERROR 1\n") ;

    auto check_response = asn_check_constraints(&asn_DEF_E2AP_PDU, control_resp_pdu, error_buf, &errlen);
    // Zero means all requirments are constraints are fine.
    if(check_response != 0) {
        LOG_D("ERROR in ASN CHECK\n") ;
    }

    LOG_D("CHECK ERROR 2\n") ;
}

void encoding::generate_e2apv1_setup_response(E2AP_PDU_t *e2ap_pdu) {

    E2setupResponseIEs *resp_ies1 = (E2setupResponseIEs_t*)calloc(1, sizeof(E2setupResponseIEs_t));
    //E2setupResponseIEs *resp_ies2 = (E2setupResponseIEs_t*)calloc(1, sizeof(E2setupResponseIEs_t));
    //E2setupResponseIEs *resp_ies3 = (E2setupResponseIEs_t*)calloc(1, sizeof(E2setupResponseIEs_t));
  
    auto *buf = (uint8_t *) "gnb1";

    BIT_STRING_t *ricid_bstring = (BIT_STRING_t*)calloc(1,sizeof(BIT_STRING_t));
    ricid_bstring->buf = buf;
    ricid_bstring->size = 4;
    ricid_bstring->bits_unused = 0;

    uint8_t *buf2 = (uint8_t *)"plmn3";
    OCTET_STRING_t *plmn = (OCTET_STRING_t*)calloc(1,sizeof(OCTET_STRING_t));
    plmn->buf = buf2;
    plmn->size = 5;

    GlobalRIC_ID_t *globalricid = (GlobalRIC_ID_t*)calloc(1,sizeof(GlobalRIC_ID_t));
    globalricid->pLMN_Identity = *plmn;
    globalricid->ric_ID = *ricid_bstring;

    E2setupResponseIEs__value_PR pres1;
    pres1 = E2setupResponseIEs__value_PR_GlobalRIC_ID;

    resp_ies1->id = ProtocolIE_ID_id_GlobalRIC_ID;
    resp_ies1->criticality = Criticality_reject;
    resp_ies1->value.present = pres1;
    resp_ies1->value.choice.GlobalRIC_ID = *globalricid;

    E2setupResponse_t *e2setupresp = (E2setupResponse_t*)calloc(1,sizeof(E2setupResponse_t));
    int ret = ASN_SEQUENCE_ADD(&e2setupresp->protocolIEs.list, resp_ies1);

    SuccessfulOutcome__value_PR pres;
    pres = SuccessfulOutcome__value_PR_E2setupResponse;
    SuccessfulOutcome_t *successoutcome = (SuccessfulOutcome_t*)calloc(1, sizeof(SuccessfulOutcome_t));
    successoutcome->procedureCode = 1;
    successoutcome->criticality = 0;
    successoutcome->value.present = pres;
    successoutcome->value.choice.E2setupResponse = *e2setupresp;


    E2AP_PDU_PR pres5 = E2AP_PDU_PR_successfulOutcome;
  
    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.successfulOutcome = successoutcome;

}


void encoding::generate_e2apv1_subscription_request(E2AP_PDU *e2ap_pdu) {

    fprintf(stderr, "in sub 1\n");
    RICsubscriptionRequest_IEs_t *ricreqid = (RICsubscriptionRequest_IEs_t*)calloc(1, sizeof(RICsubscriptionRequest_IEs_t));
    fprintf(stderr, "in sub 2\n");  
    ASN_STRUCT_RESET(asn_DEF_RICsubscriptionRequest_IEs, ricreqid);
    fprintf(stderr, "in sub 3\n");  
    auto *ricsubrid = (RICsubscriptionRequest_IEs_t*)calloc(1, sizeof(RICsubscriptionRequest_IEs_t));
    fprintf(stderr, "in sub 4\n");  
    ASN_STRUCT_RESET(asn_DEF_RICsubscriptionRequest_IEs, ricsubrid);
    
    fprintf(stderr, "in generate sub\n");
    auto *buf2 = (uint8_t *) "SubscriptionTriggers";
    fprintf(stderr, "in gen sub 1\n");
    OCTET_STRING_t *triggerdef = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
    triggerdef->buf = (uint8_t *) calloc(1, 20);
    triggerdef->size = 20;
    memcpy(triggerdef->buf, buf2, triggerdef->size);

    fprintf(stderr, "sub1\n");
    ProtocolIE_ID_t proto_id = ProtocolIE_ID_id_RICaction_ToBeSetup_Item;

    RICaction_ToBeSetup_ItemIEs__value_PR pres6;
    pres6 = RICaction_ToBeSetup_ItemIEs__value_PR_RICaction_ToBeSetup_Item;

    printf("sub2\n");

    auto *buf5 = (uint8_t *) "ActionDef";

    OCTET_STRING_t *actdef = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
    actdef->buf = (uint8_t *) calloc(1, 9);
    actdef->size = 9;
    memcpy(triggerdef->buf, buf5, 9);

    auto *sa = (RICsubsequentAction_t *) calloc(1, sizeof(RICsubsequentAction_t));
    ASN_STRUCT_RESET(asn_DEF_RICsubsequentAction, sa);

    sa->ricTimeToWait = RICtimeToWait_w500ms;
    sa->ricSubsequentActionType = RICsubsequentActionType_continue;

    printf("sub3\n");

    RICaction_ToBeSetup_ItemIEs_t *action_item_ies = (RICaction_ToBeSetup_ItemIEs_t *)calloc(1, sizeof(RICaction_ToBeSetup_Item_t));
    action_item_ies->id = proto_id;
    action_item_ies->criticality = 0;

    action_item_ies->value.present = pres6;
    action_item_ies->value.choice.RICaction_ToBeSetup_Item.ricActionID = 5;
    action_item_ies->value.choice.RICaction_ToBeSetup_Item.ricActionType = RICactionType_report;
    action_item_ies->value.choice.RICaction_ToBeSetup_Item.ricActionDefinition = actdef;
    action_item_ies->value.choice.RICaction_ToBeSetup_Item.ricSubsequentAction = sa;

    printf("sub5\n");
    RICsubscriptionRequest_IEs__value_PR pres3;
    printf("sub6.1\n");
    pres3 = RICsubscriptionRequest_IEs__value_PR_RICsubscriptionDetails;
    ricsubrid->id = ProtocolIE_ID_id_RICsubscriptionDetails;
    printf("sub6.2\n");
    ricsubrid->criticality = 0;
    ricsubrid->value.present = pres3;
    printf("sub6.3\n");
  
    ricsubrid->value.choice.RICsubscriptionDetails.ricEventTriggerDefinition = *triggerdef;
    printf("sub6.4\n");

    ASN_SEQUENCE_ADD(&ricsubrid->value.choice.RICsubscriptionDetails.ricAction_ToBeSetup_List.list, action_item_ies);

    printf("sub7\n");

    ricreqid->id = ProtocolIE_ID_id_RICrequestID;
    ricreqid->criticality = Criticality_reject;
    ricreqid->value.present = RICsubscriptionRequest_IEs__value_PR_RICrequestID;
    ricreqid->value.choice.RICrequestID.ricRequestorID = 22;
    ricreqid->value.choice.RICrequestID.ricInstanceID = 6;

    RICsubscriptionRequest_t *ricsubreq = (RICsubscriptionRequest_t*)calloc(1, sizeof(RICsubscriptionRequest_t));

    ASN_SEQUENCE_ADD(&ricsubreq->protocolIEs.list, ricreqid);
    ASN_SEQUENCE_ADD(&ricsubreq->protocolIEs.list, ricsubrid);

    InitiatingMessage__value_PR pres4;
    pres4 = InitiatingMessage__value_PR_RICsubscriptionRequest;
    InitiatingMessage_t *initmsg = (InitiatingMessage_t*)calloc(1, sizeof(InitiatingMessage_t));
    initmsg->procedureCode = ProcedureCode_id_RICsubscription;
    initmsg->criticality = Criticality_reject;
    initmsg->value.present = pres4;
    initmsg->value.choice.RICsubscriptionRequest = *ricsubreq;

    E2AP_PDU_PR pres5;
    pres5 = E2AP_PDU_PR_initiatingMessage;

    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.initiatingMessage = initmsg;

    char error_buf[300] = {0, };
    size_t errlen = 0;

    asn_check_constraints(&asn_DEF_E2AP_PDU, e2ap_pdu, error_buf, &errlen);
    printf("error length %d\n", errlen);
    printf("error buf %s\n", error_buf);
  
}

void encoding::generate_e2apv1_subscription_response_success(E2AP_PDU *e2ap_pdu, long reqActionIdsAccepted[],
                                                             long reqActionIdsRejected[], int accept_size,
                                                             int reject_size,
                                                             long reqRequestorId, long reqInstanceId) {

    RICsubscriptionResponse_IEs_t *respricreqid =
        (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));

    respricreqid->id = ProtocolIE_ID_id_RICrequestID;
    respricreqid->criticality = 0;
    respricreqid->value.present = RICsubscriptionResponse_IEs__value_PR_RICrequestID;
    respricreqid->value.choice.RICrequestID.ricRequestorID = reqRequestorId;
    
    respricreqid->value.choice.RICrequestID.ricInstanceID = reqInstanceId;
    
    RICsubscriptionResponse_IEs_t *respfuncid =
        (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
    respfuncid->id = ProtocolIE_ID_id_RANfunctionID;
    respfuncid->criticality = 0;
    respfuncid->value.present = RICsubscriptionResponse_IEs__value_PR_RANfunctionID;
    respfuncid->value.choice.RANfunctionID = (long)147;


    RICsubscriptionResponse_IEs_t *ricactionadmitted =
    (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
    ricactionadmitted->id = ProtocolIE_ID_id_RICactions_Admitted;
    ricactionadmitted->criticality = 0;
    ricactionadmitted->value.present = RICsubscriptionResponse_IEs__value_PR_RICaction_Admitted_List;

    RICaction_Admitted_List_t* admlist = 
        (RICaction_Admitted_List_t*)calloc(1,sizeof(RICaction_Admitted_List_t));
    ricactionadmitted->value.choice.RICaction_Admitted_List = *admlist;
    if (admlist) free(admlist);

    int numAccept = accept_size;
    int numReject = reject_size;

    for (int i = 0; i < numAccept; i++) {
        fprintf(stderr, "in for loop i = %d\n", i);

        long aid = reqActionIdsAccepted[i];

        RICaction_Admitted_ItemIEs_t *admitie = (RICaction_Admitted_ItemIEs_t*)calloc(1,sizeof(RICaction_Admitted_ItemIEs_t));
        admitie->id = ProtocolIE_ID_id_RICaction_Admitted_Item;
        admitie->criticality = 0;
        admitie->value.present = RICaction_Admitted_ItemIEs__value_PR_RICaction_Admitted_Item;
        admitie->value.choice.RICaction_Admitted_Item.ricActionID = aid;
        
        ASN_SEQUENCE_ADD(&ricactionadmitted->value.choice.RICaction_Admitted_List.list, admitie);
   
    }
    
    RICsubscriptionResponse_t *ricsubresp = (RICsubscriptionResponse_t*)calloc(1,sizeof(RICsubscriptionResponse_t));
    ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, respricreqid);
    ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, respfuncid);
    ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, ricactionadmitted);
    


    // Mina add RICAction NotAdmitted 


// =================================================================================

    RICsubscriptionResponse_IEs_t  *ricactionrejected =
            (RICsubscriptionResponse_IEs_t *) calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
    ricactionrejected->id = ProtocolIE_ID_id_RICactions_NotAdmitted;
    ricactionrejected->criticality = Criticality_reject;
    ricactionrejected->value.present = RICsubscriptionResponse_IEs__value_PR_RICaction_NotAdmitted_List;
    //printf("[Mina] I'm here 1 ") ;
    RICaction_NotAdmitted_List_t *rejectlist =
            (RICaction_NotAdmitted_List_t *) calloc(1, sizeof(RICaction_NotAdmitted_List_t));
                   // printf("[Mina] I'm here 2 \n ") ;

    ricactionrejected->value.choice.RICaction_NotAdmitted_List = *rejectlist;
            printf("[Mina] I'm here 3 \n") ;

    if (numReject > 0) {

        for (int i = 0; i < numReject; i++) {
            fprintf(stderr, "in for loop i = %d\n", i);

            long aid = reqActionIdsRejected[i];

            RICaction_NotAdmitted_ItemIEs_t  *noadmitie = (RICaction_NotAdmitted_ItemIEs_t *) calloc(1, sizeof(RICaction_NotAdmitted_ItemIEs_t));
            noadmitie->id = ProtocolIE_ID_id_RICaction_NotAdmitted_Item;
            noadmitie->criticality = Criticality_reject;
            noadmitie->value.present = RICaction_NotAdmitted_ItemIEs__value_PR_RICaction_NotAdmitted_Item;
            noadmitie->value.choice.RICaction_NotAdmitted_Item.ricActionID = aid;

            ASN_SEQUENCE_ADD(&ricactionrejected->value.choice.RICaction_NotAdmitted_List.list, noadmitie);
        }

    }
   ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, ricactionrejected);

//====================================================================================



/*
    if (numReject > 0) {

        auto *ricactionrejected =
                (RICsubscriptionResponse_IEs_t *) calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
        ricactionrejected->id = ProtocolIE_ID_id_RICactions_NotAdmitted;
        ricactionrejected->criticality = Criticality_reject;
        ricactionrejected->value.present = RICsubscriptionResponse_IEs__value_PR_RICaction_NotAdmitted_List;

        auto *rejectlist =
                (RICaction_NotAdmitted_List_t *) calloc(1, sizeof(RICaction_NotAdmitted_List_t));
        ricactionadmitted->value.choice.RICaction_NotAdmitted_List = *rejectlist;

        for (int i = 0; i < numReject; i++) {
            LOG_D("in for loop i = %d\n", i);

            long aid = reqActionIdsRejected[i];

            auto *noadmitie = (RICaction_NotAdmitted_ItemIEs_t *) calloc(1, sizeof(RICaction_NotAdmitted_ItemIEs_t));
            noadmitie->id = ProtocolIE_ID_id_RICaction_NotAdmitted_Item;
            noadmitie->criticality = Criticality_reject;
            noadmitie->value.present = RICaction_NotAdmitted_ItemIEs__value_PR_RICaction_NotAdmitted_Item;
            noadmitie->value.choice.RICaction_NotAdmitted_Item.ricActionID = aid;

            ASN_SEQUENCE_ADD(&ricactionrejected->value.choice.RICaction_NotAdmitted_List.list, noadmitie);
            ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, ricactionrejected);
        }
    }
   */
    
    SuccessfulOutcome__value_PR pres2;
    pres2 = SuccessfulOutcome__value_PR_RICsubscriptionResponse;
   
    SuccessfulOutcome_t *successoutcome = (SuccessfulOutcome_t*)calloc(1, sizeof(SuccessfulOutcome_t));
    successoutcome->procedureCode = ProcedureCode_id_RICsubscription;
    successoutcome->criticality = 0;
    successoutcome->value.present = pres2;
    successoutcome->value.choice.RICsubscriptionResponse = *ricsubresp;
    if (ricsubresp) free(ricsubresp);

    E2AP_PDU_PR pres5 = E2AP_PDU_PR_successfulOutcome;
  
    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.successfulOutcome = successoutcome;

    char error_buf[300] = {0, };
    size_t errlen = 0;

    asn_check_constraints(&asn_DEF_E2AP_PDU, e2ap_pdu, error_buf, &errlen);

    LOG_I("Subscription response");
    xer_fprint(stderr, &asn_DEF_E2AP_PDU, e2ap_pdu); 
}


void encoding::generate_e2apv1_subscription_response(E2AP_PDU *e2ap_pdu, E2AP_PDU *sub_req_pdu) {

    //Gather details of the request

    RICsubscriptionRequest_t orig_req =
            sub_req_pdu->choice.initiatingMessage->value.choice.RICsubscriptionRequest;
    
    RICsubscriptionResponse_IEs_t *ricreqid =
        (RICsubscriptionResponse_IEs_t*)calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
    
    int count = orig_req.protocolIEs.list.count;
    int size = orig_req.protocolIEs.list.size;

    RICsubscriptionRequest_IEs_t **ies = (RICsubscriptionRequest_IEs_t**)orig_req.protocolIEs.list.array;

    LOG_I("Number of IEs in original request: %d, size: %d", count, size);

    RICsubscriptionRequest_IEs__value_PR pres;

    long responseRequestorId;
    long responseInstanceId;
    long responseActionId;

    std::vector<long> actionIds;

    for (int i = 0; i < count; i++) {
        RICsubscriptionRequest_IEs_t *next_ie = ies[i];
        pres = next_ie->value.present;

        LOG_D("Next present value %d", pres);

        switch(pres) {
            case RICsubscriptionRequest_IEs__value_PR_RICrequestID: {
                RICrequestID_t reqId = next_ie->value.choice.RICrequestID;
                long requestorId = reqId.ricRequestorID;
                long instanceId = reqId.ricInstanceID;

                LOG_I("RequestorId %d, InstanceId: %d", requestorId, instanceId);

                responseRequestorId = requestorId;
                responseInstanceId = instanceId;
                break;
            }
            case RICsubscriptionRequest_IEs__value_PR_RANfunctionID:
                break;
            case RICsubscriptionRequest_IEs__value_PR_RICsubscriptionDetails: {
                RICsubscriptionDetails_t subDetails = next_ie->value.choice.RICsubscriptionDetails;
                RICeventTriggerDefinition_t triggerDef = subDetails.ricEventTriggerDefinition;
                RICactions_ToBeSetup_List_t actionList = subDetails.ricAction_ToBeSetup_List;

                int actionCount = actionList.list.count;
                LOG_I("Action count%d", actionCount);

                auto **item_array = actionList.list.array;

                for (int j = 0; j < actionCount; j++) {
                    //RICaction_ToBeSetup_Item_t
                    auto *next_item = item_array[j];
                    RICactionID_t actionId = ((RICaction_ToBeSetup_ItemIEs *) next_item)->value.choice.RICaction_ToBeSetup_Item.ricActionID;
                    responseActionId = actionId;
                    actionIds.push_back(responseActionId);
                }

                break;
            }
        }

    }

    RICsubscriptionResponse_IEs_t *respricreqid =
        (RICsubscriptionResponse_IEs_t *) calloc(1, sizeof(RICsubscriptionResponse_IEs_t));

    respricreqid->id = ProtocolIE_ID_id_RICrequestID;
    respricreqid->criticality = Criticality_reject;
    respricreqid->value.present = RICsubscriptionResponse_IEs__value_PR_RICrequestID;
    respricreqid->value.choice.RICrequestID.ricRequestorID = responseRequestorId;

    respricreqid->value.choice.RICrequestID.ricInstanceID = responseInstanceId;


    RICsubscriptionResponse_IEs_t *ricactionadmitted =
        (RICsubscriptionResponse_IEs_t *) calloc(1, sizeof(RICsubscriptionResponse_IEs_t));
    ricactionadmitted->id = ProtocolIE_ID_id_RICactions_Admitted;
    ricactionadmitted->criticality = Criticality_reject;
    ricactionadmitted->value.present = RICsubscriptionResponse_IEs__value_PR_RICaction_Admitted_List;

    RICaction_Admitted_List_t* admlist = 
        (RICaction_Admitted_List_t *) calloc(1, sizeof(RICaction_Admitted_List_t));
    ricactionadmitted->value.choice.RICaction_Admitted_List = *admlist;

    for (int i = 0; i < actionIds.size(); i++) {
        long aid = actionIds.at(i);

        RICaction_Admitted_ItemIEs_t *admitie = (RICaction_Admitted_ItemIEs_t*)calloc(1,sizeof(RICaction_Admitted_ItemIEs_t));
        admitie->id = ProtocolIE_ID_id_RICaction_Admitted_Item;
        admitie->criticality = 0;
        admitie->value.present = RICaction_Admitted_ItemIEs__value_PR_RICaction_Admitted_Item;
        admitie->value.choice.RICaction_Admitted_Item.ricActionID = aid;

        ASN_SEQUENCE_ADD(&ricactionadmitted->value.choice.RICaction_Admitted_List.list, admitie);

    }

    RICsubscriptionResponse_t *ricsubresp = (RICsubscriptionResponse_t*)calloc(1,sizeof(RICsubscriptionResponse_t));

    ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, respricreqid);
    ASN_SEQUENCE_ADD(&ricsubresp->protocolIEs.list, ricactionadmitted);

    SuccessfulOutcome__value_PR pres2;
    pres2 = SuccessfulOutcome__value_PR_RICsubscriptionResponse;
    SuccessfulOutcome_t *successoutcome = (SuccessfulOutcome_t*)calloc(1, sizeof(SuccessfulOutcome_t));
    successoutcome->procedureCode = ProcedureCode_id_RICsubscription;
    successoutcome->criticality = 0;
    successoutcome->value.present = pres2;
    successoutcome->value.choice.RICsubscriptionResponse = *ricsubresp;
  
    E2AP_PDU_PR pres5 = E2AP_PDU_PR_successfulOutcome;

    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.successfulOutcome = successoutcome;

    char error_buf[300] = {0, };
    size_t errlen = 0;

    asn_check_constraints(&asn_DEF_E2AP_PDU, e2ap_pdu, error_buf, &errlen);
}

void encoding::generate_e2apv1_indication_request_parameterized(E2AP_PDU *e2ap_pdu,
                                                                long requestorId,
                                                                long instanceId,
                                                                long ranFunctionId,
                                                                long actionId,
                                                                long seqNum,
                                                                uint8_t *ind_header_buf,
                                                                int header_length,
                                                                uint8_t *ind_message_buf,
                                                                int message_length) {


    
    LOG_I("Initializing E2AP request");

    RICindication_IEs_t *ricind_ies = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies2 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies3 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies4 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies5 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies6 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies7 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
    RICindication_IEs_t *ricind_ies8 = (RICindication_IEs_t*)calloc(1, sizeof(RICindication_IEs_t));
  
    RICindication_IEs__value_PR pres3;

    pres3 = RICindication_IEs__value_PR_RICrequestID;
    ricind_ies->id = ProtocolIE_ID_id_RICrequestID;
    ricind_ies->criticality = 0;
    ricind_ies->value.present = pres3;
    ricind_ies->value.choice.RICrequestID.ricRequestorID = requestorId;
    ricind_ies->value.choice.RICrequestID.ricInstanceID = instanceId;
  
    pres3 = RICindication_IEs__value_PR_RANfunctionID;
    ricind_ies2->id = ProtocolIE_ID_id_RANfunctionID;
    ricind_ies2->criticality = 0;
    ricind_ies2->value.present = pres3;
    ricind_ies2->value.choice.RANfunctionID = ranFunctionId;


    ricind_ies3->id = ProtocolIE_ID_id_RICactionID;
    ricind_ies3->criticality = 0;
    pres3 =  RICindication_IEs__value_PR_RICactionID;
    ricind_ies3->value.present = pres3;
    ricind_ies3->value.choice.RICactionID = actionId;

    // This is why 주석?
    //ricind_ies4->id = ProtocolIE_ID_id_RICindicationSN;
 //   ricind_ies4->criticality = Criticality_reject;
 //   ricind_ies4->value.present = RICindication_IEs__value_PR_RICindicationSN;
   // ricind_ies4->value.choice.RICindicationSN = seqNum;

    //Indication type is REPORT
    pres3 = RICindication_IEs__value_PR_RICindicationType;
    ricind_ies5->id = ProtocolIE_ID_id_RICindicationType;
    ricind_ies5->criticality = 0;
    ricind_ies5->value.present = pres3;
    ricind_ies5->value.choice.RICindicationType = 0;

    /*ㅌ
    auto *hdr_str = (OCTET_STRING_t *) calloc(1, sizeof(OCTET_STRING_t));

    hdr_str->buf = (uint8_t *) calloc(1, header_length);
    hdr_str->size = header_length;
    memcpy(hdr_str->buf, ind_header_buf, header_length);
     */

    ricind_ies6->value.choice.RICindicationHeader.buf = (uint8_t *) calloc(1, header_length);

    pres3 = RICindication_IEs__value_PR_RICindicationHeader;
    ricind_ies6->id = ProtocolIE_ID_id_RICindicationHeader;
    ricind_ies6->criticality = 0;
    ricind_ies6->value.present = pres3;
    ricind_ies6->value.choice.RICindicationHeader.size = header_length;
    memcpy(ricind_ies6->value.choice.RICindicationHeader.buf, ind_header_buf, header_length);

    ricind_ies7->value.choice.RICindicationMessage.buf = (uint8_t *) calloc(1, 8192);

    pres3 = RICindication_IEs__value_PR_RICindicationMessage;
    ricind_ies7->id = ProtocolIE_ID_id_RICindicationMessage;
  
    ricind_ies7->criticality = 0;
    ricind_ies7->value.present = pres3;
  
    ricind_ies7->value.choice.RICindicationMessage.size = message_length;
  
    memcpy(ricind_ies7->value.choice.RICindicationMessage.buf, ind_message_buf, message_length);
  
    // TODO : this should be deleted ???
   /* uint8_t *cpid_buf = (uint8_t *) "cpid";
    OCTET_STRING_t cpid_str;

    int cpid_buf_len = strlen((char *) cpid_buf);
    pres3 = RICindication_IEs__value_PR_RICcallProcessID;
    ricind_ies8->id = ProtocolIE_ID_id_RICcallProcessID;

    ricind_ies8->criticality = Criticality_reject;
    ricind_ies8->value.present = RICindication_IEs__value_PR_RICcallProcessID;

    ricind_ies8->value.choice.RICcallProcessID.buf = (uint8_t *) calloc(1, cpid_buf_len);
    ricind_ies8->value.choice.RICcallProcessID.size = cpid_buf_len;

    memcpy(ricind_ies8->value.choice.RICcallProcessID.buf, cpid_buf, cpid_buf_len);
*/
    RICindication_t *ricindication = (RICindication_t*)calloc(1, sizeof(RICindication_t));

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies);

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies2);

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies3);

    //ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies4);

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies5);

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies6);

    ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies7);

  //  ASN_SEQUENCE_ADD(&ricindication->protocolIEs.list, ricind_ies8);


    InitiatingMessage__value_PR pres4;
    pres4 = InitiatingMessage__value_PR_RICindication;
    InitiatingMessage_t *initmsg = (InitiatingMessage_t*)calloc(1, sizeof(InitiatingMessage_t));
    initmsg->procedureCode = 5;
    initmsg->criticality = 1;
    initmsg->value.present = pres4;
    initmsg->value.choice.RICindication = *ricindication;
    if (ricindication) free(ricindication);

    E2AP_PDU_PR pres5;
    pres5 = E2AP_PDU_PR_initiatingMessage;
    
    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.initiatingMessage = initmsg;
    char error_buf[300] = {0, };
    size_t errlen = 0;  

    int ret = asn_check_constraints(&asn_DEF_E2AP_PDU, e2ap_pdu, error_buf, &errlen);
    
    if(ret) {
        LOG_I("Constraint validation of E2AP PDU message failed: %s\n", error_buf);
    }
}


// Update Newlly
void encoding::generate_e2apv1_setup_request_parameterized(E2AP_PDU_t *e2ap_pdu, std::vector<ran_func_info> all_funcs) {

    BIT_STRING_t *gnb_bstring = (BIT_STRING_t*)calloc(1, sizeof(BIT_STRING_t));;
    gnb_bstring->buf = (uint8_t*)calloc(1,4);
    gnb_bstring->size = 4;
    gnb_bstring->buf[0] = 0xB5;
    gnb_bstring->buf[1] = 0xC6;
    gnb_bstring->buf[2] = 0x77;
    gnb_bstring->buf[3] = 0x88;
  
    gnb_bstring->bits_unused = 3;
  
    uint8_t *buf2 = (uint8_t *)"747";
    OCTET_STRING_t *plmn = (OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
    plmn->buf = (uint8_t*)calloc(1,3);
    memcpy(plmn->buf, buf2, 3);
    plmn->size = 3;
  
    GNB_ID_Choice_t *gnbchoice = (GNB_ID_Choice_t*)calloc(1,sizeof(GNB_ID_Choice_t));
    GNB_ID_Choice_PR pres2 = GNB_ID_Choice_PR_gnb_ID;
    gnbchoice->present = pres2;
    gnbchoice->choice.gnb_ID = *gnb_bstring;
    if (gnb_bstring) free(gnb_bstring);
  
    GlobalgNB_ID_t *gnb = (GlobalgNB_ID_t*)calloc(1, sizeof(GlobalgNB_ID_t));
    gnb->plmn_id = *plmn;
    gnb->gnb_id = *gnbchoice;
    if (plmn) free(plmn);
    if (gnbchoice) free(gnbchoice);
  
    GlobalE2node_gNB_ID_t *e2gnb = (GlobalE2node_gNB_ID_t*)calloc(1, sizeof(GlobalE2node_gNB_ID_t));
    e2gnb->global_gNB_ID = *gnb;
    if (gnb) free(gnb);
  
    GlobalE2node_ID_t *globale2nodeid = (GlobalE2node_ID_t*)calloc(1, sizeof(GlobalE2node_ID_t));
    GlobalE2node_ID_PR pres;
    pres = GlobalE2node_ID_PR_gNB;
    globale2nodeid->present = pres;
    globale2nodeid->choice.gNB = e2gnb;
    
    E2setupRequestIEs_t *e2setuprid = (E2setupRequestIEs_t*)calloc(1, sizeof(E2setupRequestIEs_t));
    E2setupRequestIEs__value_PR pres3;
    pres3 = E2setupRequestIEs__value_PR_GlobalE2node_ID;
    e2setuprid->id = 3;
    e2setuprid->criticality = 0;
    e2setuprid->value.choice.GlobalE2node_ID = *globale2nodeid;
    e2setuprid->value.present = pres3;
    if(globale2nodeid) free(globale2nodeid);
  
  
    //seting tx id
    auto *e2txid = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
    e2txid->id = ProtocolIE_ID_id_TransactionID;
    e2txid-> criticality = 0;
    e2txid->value.present = E2setupRequestIEs__value_PR_TransactionID;
    e2txid->value.choice.TransactionID = 1;
  
    auto *ranFlistIEs = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
    ASN_STRUCT_RESET(asn_DEF_E2setupRequestIEs, ranFlistIEs);
    ranFlistIEs->criticality = 0;
    ranFlistIEs->id = ProtocolIE_ID_id_RANfunctionsAdded;
    ranFlistIEs->value.present = E2setupRequestIEs__value_PR_RANfunctions_List;
  
  
    for (int i = 0; i < all_funcs.size(); i++) {
  
      ran_func_info nextRanFunc = all_funcs.at(i);
      long nextRanFuncId = nextRanFunc.ranFunctionId;
      OCTET_STRING_t *nextRanFuncDesc = nextRanFunc.ranFunctionDesc;
      long nextRanFuncRev = nextRanFunc.ranFunctionRev;
  
      auto *itemIes = (RANfunction_ItemIEs_t *)calloc(1, sizeof(RANfunction_ItemIEs_t));
      itemIes->id = ProtocolIE_ID_id_RANfunction_Item;
      itemIes->criticality = Criticality_reject;
      itemIes->value.present = RANfunction_ItemIEs__value_PR_RANfunction_Item;
      itemIes->value.choice.RANfunction_Item.ranFunctionID = nextRanFuncId;
      itemIes->value.choice.RANfunction_Item.ranFunctionOID = RANfunctionOID_t(*(nextRanFunc.ranFunctionOId));
      int ranFuncLength = strlen((char*)nextRanFuncDesc);
  
      itemIes->value.choice.RANfunction_Item.ranFunctionDefinition = *nextRanFuncDesc;
      itemIes->value.choice.RANfunction_Item.ranFunctionRevision = nextRanFuncRev;
      
      ASN_SEQUENCE_ADD(&ranFlistIEs->value.choice.RANfunctions_List.list, itemIes);
  
    }
  
  auto *e2configIE = (E2setupRequestIEs_t *)calloc(1, sizeof(E2setupRequestIEs_t));
  e2configIE->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition;
  e2configIE->criticality = Criticality_reject;
  e2configIE->value.present = E2setupRequestIEs__value_PR_E2nodeComponentConfigAddition_List;
  
  
  auto *e2configAdditionItem = (E2nodeComponentConfigAddition_ItemIEs_t *)calloc(1, sizeof(E2nodeComponentConfigAddition_ItemIEs_t));
  e2configAdditionItem->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition_Item;
  e2configAdditionItem->criticality = Criticality_reject;
  e2configAdditionItem->value.present = E2nodeComponentConfigAddition_ItemIEs__value_PR_E2nodeComponentConfigAddition_Item;
  
  e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentInterfaceType = E2nodeComponentInterfaceType_ng;
  e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.present = E2nodeComponentID_PR_e2nodeComponentInterfaceTypeNG;
  
  auto *intfNG = (E2nodeComponentInterfaceNG_t *) calloc(1, sizeof(E2nodeComponentInterfaceNG_t));
    
  OCTET_STRING_t nginterf;
  nginterf.buf = (uint8_t*)calloc(1,8);
  memcpy(nginterf.buf, (uint8_t *)"nginterf", 8);
  
  nginterf.size = 8;
  intfNG->amf_name = (AMFName_t)(nginterf);
  
  e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.choice.e2nodeComponentInterfaceTypeNG = intfNG;
  
  OCTET_STRING_t reqPart;
  reqPart.buf = (uint8_t*)calloc(1,7);
  memcpy(reqPart.buf, (uint8_t *)"reqpart", 7);
  reqPart.size = 7;
  
  e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentRequestPart = reqPart;
  
  OCTET_STRING_t resPart;
  resPart.buf = (uint8_t*)calloc(1,7);
  memcpy(resPart.buf, (uint8_t *)"respart", 7);
  resPart.size = 7;
  e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentResponsePart = resPart;
  
  
  ASN_SEQUENCE_ADD(&e2configIE->value.choice.RANfunctions_List.list, e2configAdditionItem);
    E2setupRequest_t *e2setupreq = (E2setupRequest_t*)calloc(1, sizeof(E2setupRequest_t));
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2txid);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2setuprid);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, ranFlistIEs);
    ASN_SEQUENCE_ADD(&e2setupreq->protocolIEs.list, e2configIE);
  
  
    InitiatingMessage__value_PR pres4;
    pres4 = InitiatingMessage__value_PR_E2setupRequest;
    InitiatingMessage_t *initmsg = (InitiatingMessage_t*)calloc(1, sizeof(InitiatingMessage_t));
  
    initmsg->procedureCode = ProcedureCode_id_E2setup;
    initmsg->criticality = Criticality_reject;
    initmsg->value.present = pres4;
    initmsg->value.choice.E2setupRequest = *e2setupreq;
    if (e2setupreq) free(e2setupreq);
  
    E2AP_PDU_PR pres5;
    pres5 = E2AP_PDU_PR_initiatingMessage;
    
  
    e2ap_pdu->present = pres5;
    e2ap_pdu->choice.initiatingMessage = initmsg;  
  
  }

  void encoding::generate_e2apv2_reset_request(E2AP_PDU *e2ap_pdu) {
    E2AP_PDU_PR pr = E2AP_PDU_PR_initiatingMessage;
    e2ap_pdu->present = pr;
  
    e2ap_pdu->choice.initiatingMessage = (InitiatingMessage_t*)calloc(1, sizeof(InitiatingMessage_t));
    e2ap_pdu->choice.initiatingMessage->procedureCode = ProcedureCode_id_Reset;
    e2ap_pdu->choice.initiatingMessage->criticality = Criticality_reject;
  
    e2ap_pdu->choice.initiatingMessage->value.present = InitiatingMessage__value_PR_ResetRequest;
  
    auto *rrIEs1 = (ResetRequestIEs_t *)calloc(1, sizeof(ResetRequestIEs_t));
    rrIEs1->id = ProtocolIE_ID_id_Cause;
    rrIEs1->criticality = Criticality_ignore;
  
    rrIEs1->value.present = ResetRequestIEs__value_PR_Cause;
    rrIEs1->value.choice.Cause.present = Cause_PR_e2Node;
    rrIEs1->value.choice.Cause.choice.ricRequest = CauseE2node_e2node_component_unknown;
  
    auto *rrIEs2 = (ResetRequestIEs_t *)calloc(1, sizeof(ResetRequestIEs_t));
    rrIEs2->id = ProtocolIE_ID_id_TransactionID;
    rrIEs2->criticality = Criticality_ignore;
    
    rrIEs2->value.present = ResetRequestIEs__value_PR_TransactionID;
    rrIEs2->value.choice.TransactionID = 1;
  
    ASN_SEQUENCE_ADD(&e2ap_pdu->choice.initiatingMessage->value.choice.ResetRequest.protocolIEs.list, rrIEs2);
    ASN_SEQUENCE_ADD(&e2ap_pdu->choice.initiatingMessage->value.choice.ResetRequest.protocolIEs.list, rrIEs1);
  }
  
void encoding::generate_e2apv2_reset_response(E2AP_PDU *e2ap_pdu) {
    e2ap_pdu->present = E2AP_PDU_PR_successfulOutcome;
    e2ap_pdu->choice.successfulOutcome = (SuccessfulOutcome_t*)calloc(1, sizeof(SuccessfulOutcome));

    e2ap_pdu->choice.successfulOutcome->procedureCode = ProcedureCode_id_Reset;
    e2ap_pdu->choice.successfulOutcome->criticality = Criticality_ignore;

    e2ap_pdu->choice.successfulOutcome->value.present = SuccessfulOutcome__value_PR_ResetResponse;

    // auto *rrIEs1 = (ResetResponseIEs_t *)calloc(1, sizeof(ResetResponseIEs_t));
    // rrIEs1->id = ProtocolIE_ID_id_CriticalityDiagnostics;
    // rrIEs1->criticality = Criticality_ignore;

    // rrIEs1->value.present = ResetResponseIEs__value_PR_CriticalityDiagnostics;
    // rrIEs1->value.choice.CriticalityDiagnostics.procedureCode = &ProcedureCode_id_Reset;
    // rrIEs1->value.choice.CriticalityDiagnostics.choice.ricRequest = CauseE2node_e2node_component_unknown;

    auto *rrIEs2 = (ResetRequestIEs_t *)calloc(1, sizeof(ResetRequestIEs_t));
    rrIEs2->id = ProtocolIE_ID_id_TransactionID;
    rrIEs2->criticality = Criticality_ignore;
    
    rrIEs2->value.present = ResetRequestIEs__value_PR_TransactionID;
    rrIEs2->value.choice.TransactionID = 1;

    ASN_SEQUENCE_ADD(&e2ap_pdu->choice.successfulOutcome->value.choice.ResetResponse.protocolIEs.list, rrIEs2);
}

void encoding::generate_e2apv2_config_update(E2AP_PDU_t *e2ap_pdu) {
      // txid
    auto *e2txidIE = (E2nodeConfigurationUpdate_IEs_t *)calloc(1, sizeof(E2nodeConfigurationUpdate_IEs_t));
    e2txidIE->id = ProtocolIE_ID_id_TransactionID;
    e2txidIE-> criticality = Criticality_reject;
    e2txidIE->value.present = E2nodeConfigurationUpdate_IEs__value_PR_TransactionID;
    e2txidIE->value.choice.TransactionID = 1;

    /// config update id for addtion list  
    auto *e2configIE = (E2nodeConfigurationUpdate_IEs_t *)calloc(1, sizeof(E2nodeConfigurationUpdate_IEs_t));
    e2configIE->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition;
    e2configIE->criticality = Criticality_reject;
    e2configIE->value.present = E2nodeConfigurationUpdate_IEs__value_PR_E2nodeComponentConfigAddition_List;


    auto *e2configAdditionItem = (E2nodeComponentConfigAddition_ItemIEs_t *)calloc(1, sizeof(E2nodeComponentConfigAddition_ItemIEs_t));
    e2configAdditionItem->id = ProtocolIE_ID_id_E2nodeComponentConfigAddition_Item;
    e2configAdditionItem->criticality = Criticality_reject;
    e2configAdditionItem->value.present = E2nodeComponentConfigAddition_ItemIEs__value_PR_E2nodeComponentConfigAddition_Item;

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentInterfaceType = E2nodeComponentInterfaceType_ng;
    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.present = E2nodeComponentID_PR_e2nodeComponentInterfaceTypeNG;

    auto *intfNG = (E2nodeComponentInterfaceNG_t *) calloc(1, sizeof(E2nodeComponentInterfaceNG_t));

    OCTET_STRING_t nginterf;
    nginterf.buf = (uint8_t*)calloc(1,8);
    memcpy(nginterf.buf, (uint8_t *)"nginterf", 8);

    nginterf.size = 8;
    intfNG->amf_name = (AMFName_t)(nginterf);

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentID.choice.e2nodeComponentInterfaceTypeNG = intfNG;

    OCTET_STRING_t reqPart;
    reqPart.buf = (uint8_t*)calloc(1,7);
    memcpy(reqPart.buf, (uint8_t *)"reqpart", 7);
    reqPart.size = 7;

    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentRequestPart = reqPart;

    OCTET_STRING_t resPart;
    resPart.buf = (uint8_t*)calloc(1,7);
    memcpy(resPart.buf, (uint8_t *)"respart", 7);
    resPart.size = 7;
    e2configAdditionItem->value.choice.E2nodeComponentConfigAddition_Item.e2nodeComponentConfiguration.e2nodeComponentResponsePart = resPart;

    ASN_SEQUENCE_ADD(&e2configIE->value.choice.E2nodeComponentConfigAddition_List, e2configAdditionItem);

    InitiatingMessage *inititingMsg = (InitiatingMessage *) calloc(1, sizeof(InitiatingMessage));
    inititingMsg->procedureCode = ProcedureCode_id_E2nodeConfigurationUpdate;
    inititingMsg->criticality = Criticality_reject;
    inititingMsg->value.present = InitiatingMessage__value_PR_E2nodeConfigurationUpdate;
    ASN_SEQUENCE_ADD(&inititingMsg->value.choice.E2nodeConfigurationUpdate.protocolIEs.list, e2txidIE);
    ASN_SEQUENCE_ADD(&inititingMsg->value.choice.E2nodeConfigurationUpdate.protocolIEs.list, e2configIE);
    e2ap_pdu->present = E2AP_PDU_PR_initiatingMessage;
    e2ap_pdu->choice.initiatingMessage = inititingMsg;
}
