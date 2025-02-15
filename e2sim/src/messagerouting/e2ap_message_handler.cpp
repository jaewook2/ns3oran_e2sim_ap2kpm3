

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
#include "e2ap_message_handler.hpp"

//#include <iostream>
//#include <vector>
#include <stdexcept>
#include <unistd.h>

#include "encode_e2apv1.hpp"


#include <unistd.h>
#include <ProtocolIE-Field.h>

// to be deleted/
bool e2ap_handle_sctp_data(int &socket_fd, sctp_buffer_t &data, E2Sim *e2sim) {
    LOG_D("in e2ap_handle_sctp_data()");
    //decode the data into E2AP-PDU
    auto *pdu = (E2AP_PDU_t *) calloc(1, sizeof(E2AP_PDU));
    ASN_STRUCT_RESET(asn_DEF_E2AP_PDU, pdu);
    
    asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;

    //e2ap_asn1c_decode_pdu(pdu, syntax, data.buffer, data.len); JLEE0210 CHNAGE LIKE BLEOW
    // JLEE0210 ADD
    auto rval = asn_decode(nullptr, syntax, &asn_DEF_E2AP_PDU, (void**)&pdu, data.buffer, data.len);
    
    switch (rval.code) {
        case RC_WMORE:
        case RC_FAIL:
        LOG_E("Failed to decode E2AP data from SCTP connection");
        ASN_STRUCT_FREE(asn_DEF_E2AP_PDU, pdu);
        exit(1);
        default:
        break;
    }
    // **

    //E2AP_PDU_PR pr_type_of_message = pdu->present;
    //e2ap_asn1c_print_pdu(pdu);
    //long procedureCode = e2ap_asn1c_get_procedureCode(pdu);
    // JLEE0210 ADD
    int procedureCode = e2ap_asn1c_get_procedureCode(pdu);
    int index = (int)pdu->present;
    LOG_D("Unpacked E2AP-PDU: index = %d, procedureCode = %d", index, procedureCode);

    //
    switch (procedureCode) {
        case ProcedureCode_id_E2setup: // Procedure code: 1
            LOG_I("Received a message of E2 setup procedure"); // ADD
            switch (index) {
                case E2AP_PDU_PR_initiatingMessage:
                    e2ap_handle_E2SetupRequest(pdu, socket_fd);
                    LOG_I("[E2AP] Received SETUP-REQUEST");
                    break;

                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2AP] Received SETUP-RESPONSE-SUCCESS");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2AP] Received SETUP-RESPONSE-FAILURE");
                    break;

                default: LOG_E("[E2AP] Invalid message index=%d in E2AP-PDU", index);
                    break;
            }
            break;
        /*
        // JLEE0210 dELETED?
        case ProcedureCode_id_ErrorIndication: // Procedure code: 2
            switch (pr_type_of_message) {
                case E2AP_PDU_PR_initiatingMessage:
                    e2ap_handle_E2SetupRequest(pdu, socket_fd);
                    LOG_I("[E2AP] Received ERROR-INDICATION");
                    break;

                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2AP] Received ERROR-INDICATION SUCCESS");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2AP] Received ERROR-INDICATION FAILURE");
                    break;

                default: LOG_E("[E2AP] Invalid message index=%d in E2AP-PDU", pr_type_of_message);
                    break;
            }
            break;
        */
        case ProcedureCode_id_Reset: // RESET = 3 ==> rEST = 7
            LOG_I("Received a message of Reset procedure"); // ADD
            switch (index) {
                case E2AP_PDU_PR_initiatingMessage:
                    LOG_I("[E2AP] Received RESET-REQUEST");
                    break;
                case E2AP_PDU_PR_successfulOutcome:
                    break;
                case E2AP_PDU_PR_unsuccessfulOutcome:
                    break;
                default: LOG_E("[E2AP] Invalid message index=%d in E2AP-PDU", index);
                    break;
            }
            break;
        /* ==> 필요할거 같음.
        case ProcedureCode_id_RICcontrol: // Procedure code = 4
            switch (pr_type_of_message) {
                case E2AP_PDU_PR_initiatingMessage: {
                    LOG_I("*******[E2AP] Received RIC-CONTROL-REQUEST******");

                    e2ap_handle_RICControlRequest(pdu, socket_fd, e2sim);
                    break;
                }
                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2SM] Received RIC-CONTROL-RESPONSE");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2SM] Received RIC-CONTROL-FAILURE");
                    break;

                default: LOG_E("[E2SM] Invalid message index=%d in PDU %ld", pr_type_of_message,
                               ProcedureCode_id_RICcontrol);
                    break;
            }
            break;
        */
        case ProcedureCode_id_RICindication: // 5 ==> 205
            LOG_I("Received a message of RIC indication procedure"); // jlee add

            switch (index) {
                case E2AP_PDU_PR_initiatingMessage: //initiatingMessage
                    LOG_I("[E2AP] Received RIC-INDICATION-REQUEST");
                    // e2ap_handle_RICSubscriptionRequest(pdu, socket_fd);
                    break;
                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2AP] Received RIC-INDICATION-RESPONSE");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2AP] Received RIC-INDICATION-FAILURE");
                    break;

                default:           
                    LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICindication);
                    break;
            }
            break;

        case ProcedureCode_id_RICserviceQuery: // 6 ==> ?
            LOG_I("Received a message of RIC service query procedure");
            switch (index) {
                case E2AP_PDU_PR_initiatingMessage: 
                    LOG_I("[E2AP] Received RIC-Service-Query")
                    e2ap_handle_E2SeviceRequest(pdu, socket_fd, e2sim);
                    break;

                default:           
                    LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICserviceQuery);
                    break;
            }
            break;

        case ProcedureCode_id_RICserviceUpdate: // 7 ==> ?
            LOG_I("Received a message of RIC service update procedure"); // add
            switch (index) {
                case E2AP_PDU_PR_successfulOutcome: 
                    LOG_I("[E2AP] Received RIC-SERVICE-UPDATE-SUCCESS")
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: 
                    LOG_I("[E2AP] Received RIC-SERVICE-UPDATE-FAILURE")
                    break;

                default: 
                    LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICserviceUpdate);
                    break;
            }
            break;

        case ProcedureCode_id_RICsubscription: // RIC SUBSCRIPTION = 8 ==> ric subscription = 201
            LOG_I("Received a message of RIC subscription procedure"); // jlee0210 add
            switch (index) {
                case E2AP_PDU_PR_initiatingMessage: { //initiatingMessage
                    LOG_I("[E2AP] Received RIC-SUBSCRIPTION-REQUEST");
                    //          e2ap_handle_RICSubscriptionRequest(pdu, socket_fd);
                    long func_id = encoding::get_function_id_from_subscription(pdu);
                    LOG_I("Received RIC subscription request for function with ID %d", func_id);
                    /*
                    // change like below
                    SubscriptionCallback cb;
                    bool func_exists = true;
                    try {
                        cb = e2sim->get_subscription_callback(func_id);
                    } catch (const std::out_of_range &e) {
                        func_exists = false;
                    }

                    if (func_exists) {
                        LOG_D("Calling callback function\n");
                        cb(pdu);
                    } else {
                        LOG_E("Error: No RAN Function with this ID exists\n");
                    }
                    */
                    try {
                        SubscriptionCallback cb;
                        cb = e2sim->get_subscription_callback(func_id);
                        LOG_I("Calling callback subscription function to handle subscription request to function with ID %d", func_id);
                        cb(pdu);
                    } catch (const std::out_of_range& e) {
                        LOG_E("No RAN Function with this ID exists\n");
                    }
                    //	  callback_kpm_subscription_request(pdu, socket_fd);

                }
                    break;

                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2AP] Received RIC-SUBSCRIPTION-RESPONSE");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2AP] Received RIC-SUBSCRIPTION-FAILURE");
                    break;

                default: LOG_E("[E2AP] Invalid message index=%d in E2AP-PDU", index);
                    break;
            }
            break;
        /*
        case ProcedureCode_id_RICsubscriptionDelete: // Procedure code: 9
            switch (pr_type_of_message) {
                case E2AP_PDU_PR_initiatingMessage:{
                    LOG_I("[E2AP] Received RIC-SUBSCRIPTION-DELETE");
                    // TODO: Mostafa

                    CallbackFunction cb;

                    LOG_I("****** e2ap_handle_RICSubscriptionDeleteRequest ******");

                    bool func_exists = true;
                    try {
                        cb = e2sim->get_callback(1);

                    } catch (const std::out_of_range &e) {
                        func_exists = false;
                    }

                    if (func_exists) {
                        LOG_D("Calling Delete callback function");
                        cb();
                    } else {
                        LOG_E("Error: No RAN Function with this ID exists");
                    }

                    e2ap_handle_RICSubscriptionDeleteRequest(pdu, socket_fd, e2sim);

                    return true;
                    }


                case E2AP_PDU_PR_successfulOutcome: LOG_I("[E2AP] Received SUBSCRIPTION-DELETE SUCCESS");
                    break;

                case E2AP_PDU_PR_unsuccessfulOutcome: LOG_I("[E2AP] Received SUBSCRIPTION-DELETE FAILURE");
                    break;

                default: LOG_E("[E2AP] Invalid message index=%d in E2AP-PDU", pr_type_of_message);
                    break;
            }
            break;
*/
        case ProcedureCode_id_E2nodeConfigurationUpdate:
            LOG_I("Received a message of E2 node configuration update procedure");
            switch (index) {
                case E2AP_PDU_PR_successfulOutcome:
                LOG_I("Received E2nodeConfigurationUpdate")
                break;

                default:
                LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_E2nodeConfigurationUpdate);
                break;
            }
            break;        

        default: 
            LOG_E("[E2AP] No available handler for procedureCode=%ld", procedureCode);
            break;
    }
    ASN_STRUCT_FREE(asn_DEF_E2AP_PDU, pdu); // add
    return false;
}

void e2ap_handle_sctp_data(int& socket_fd, sctp_buffer_t& data, bool xmlenc, E2Sim* e2sim) {
  E2AP_PDU_t* pdu = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));
  ASN_STRUCT_RESET(asn_DEF_E2AP_PDU, pdu);

  asn_transfer_syntax syntax;

  syntax = ATS_ALIGNED_BASIC_PER;
  auto rval = asn_decode(nullptr, syntax, &asn_DEF_E2AP_PDU, (void**)&pdu, data.buffer, data.len);

  switch (rval.code) {
    case RC_WMORE:
    case RC_FAIL:
      LOG_E("Failed to decode E2AP data from SCTP connection");
      ASN_STRUCT_FREE(asn_DEF_E2AP_PDU, pdu);
      exit(1);
    default:
      break;
  }

  int procedureCode = e2ap_asn1c_get_procedureCode(pdu);
  int index = (int)pdu->present;

  LOG_D("Unpacked E2AP-PDU: index = %d, procedureCode = %d", index, procedureCode);

  switch (procedureCode) {
    case ProcedureCode_id_E2setup:
      LOG_I("Received a message of E2 setup procedure");
      switch (index) {
        case E2AP_PDU_PR_initiatingMessage:
          e2ap_handle_E2SetupRequest(pdu, socket_fd);
          LOG_I("Received SETUP-REQUEST");
          break;

        case E2AP_PDU_PR_successfulOutcome:
          LOG_I("Received SETUP-RESPONSE-SUCCESS");
          break;

        case E2AP_PDU_PR_unsuccessfulOutcome:
          LOG_I("Received SETUP-RESPONSE-FAILURE");
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU", index);
          break;
      }
      break;

    case ProcedureCode_id_Reset:  // reset = 7
      LOG_I("Received a message of Reset procedure");
      switch (index) {
        case E2AP_PDU_PR_initiatingMessage:
          LOG_I("Received RESET-REQUEST");
          break;

        case E2AP_PDU_PR_successfulOutcome:
          break;

        case E2AP_PDU_PR_unsuccessfulOutcome:
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU", index);
          break;
      }
      break;

    case ProcedureCode_id_RICsubscription:  // RIC SUBSCRIPTION = 201
      LOG_I("Received a message of RIC subscription procedure");
      switch (index) {
        case E2AP_PDU_PR_initiatingMessage: {  // initiatingMessage
          long func_id = encoding::get_function_id_from_subscription(pdu);
          LOG_I("Received RIC subscription request for function with ID %d", func_id);

          try {
            SubscriptionCallback cb;
            cb = e2sim->get_subscription_callback(func_id);
            LOG_I("Calling callback subscription function to handle subscription request to function with ID %d", func_id);
            cb(pdu);
          } catch (const std::out_of_range& e) {
            LOG_E("No RAN Function with this ID exists\n");
          }

          break;
        }

        case E2AP_PDU_PR_successfulOutcome:
          LOG_I("Received RIC-SUBSCRIPTION-RESPONSE");
          break;

        case E2AP_PDU_PR_unsuccessfulOutcome:
          LOG_I("Received RIC-SUBSCRIPTION-FAILURE");
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU", index);
          break;
      }
      break;

    case ProcedureCode_id_RICindication:  // 205
      LOG_I("Received a message of RIC indication procedure");
      switch (index) {
        case E2AP_PDU_PR_initiatingMessage:  // initiatingMessage
          LOG_I("Received RIC-INDICATION-REQUEST");
          // e2ap_handle_RICSubscriptionRequest(pdu, socket_fd);
          break;
        case E2AP_PDU_PR_successfulOutcome:
          LOG_I("Received RIC-INDICATION-RESPONSE");
          break;

        case E2AP_PDU_PR_unsuccessfulOutcome:
          LOG_I("Received RIC-INDICATION-FAILURE");
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICindication);
          break;
      }
      break;

    case ProcedureCode_id_RICserviceQuery:
      LOG_I("Received a message of RIC service query procedure");
      switch (index) {
        case E2AP_PDU_PR_initiatingMessage:
          LOG_I("Received RIC-Service-Query")
          e2ap_handle_E2SeviceRequest(pdu, socket_fd, e2sim);
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICserviceQuery);
          break;
      }
      break;

    case ProcedureCode_id_E2nodeConfigurationUpdate:
      LOG_I("Received a message of E2 node configuration update procedure");
      switch (index) {
        case E2AP_PDU_PR_successfulOutcome:
          LOG_I("Received E2nodeConfigurationUpdate")
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_E2nodeConfigurationUpdate);
          break;
      }
      break;

    case ProcedureCode_id_RICserviceUpdate:
      LOG_I("Received a message of RIC service update procedure");
      switch (index) {
        case E2AP_PDU_PR_successfulOutcome:
          LOG_I("Received RIC-SERVICE-UPDATE-SUCCESS")
          break;

        case E2AP_PDU_PR_unsuccessfulOutcome:
          LOG_I("Received RIC-SERVICE-UPDATE-FAILURE")
          break;

        default:
          LOG_E("Invalid message index=%d in E2AP-PDU %d", index, (int)ProcedureCode_id_RICserviceUpdate);
          break;
      }
      break;

    default:
      LOG_E("No available handler for procedureCode=%d", procedureCode);
      break;
  }
  ASN_STRUCT_FREE(asn_DEF_E2AP_PDU, pdu);
}

void e2ap_handle_E2SeviceRequest(E2AP_PDU_t* pdu, int &socket_fd, E2Sim *e2sim) {
  auto buffer_size = MAX_SCTP_BUFFER;
  unsigned char buffer[MAX_SCTP_BUFFER];
  auto* res_pdu = (E2AP_PDU_t*)calloc(1,sizeof(E2AP_PDU));

  // prepare ran function defination
  std::vector<encoding::ran_func_info> all_funcs;

  //Loop through RAN function definitions that are registered

  for (std::pair<long, OCTET_STRING_t*> elem : e2sim->getRegistered_ran_functions()) {
    LOG_D("looping through ran func\n");
    encoding::ran_func_info next_func{};

    next_func.ranFunctionId = elem.first;
    next_func.ranFunctionDesc = elem.second;
    next_func.ranFunctionRev = (long)3;
    all_funcs.push_back(next_func);
  }

  printf("about to call service update encode\n");

  encoding::generate_e2apv1_service_update(res_pdu, all_funcs);

  LOG_D("Created E2-SERVICE-UPDATE");

  e2ap_asn1c_print_pdu(res_pdu);

  sctp_buffer_t data;

  char error_buf[300] = {
    0,
  };
  size_t errlen = 0;

  asn_check_constraints(&asn_DEF_E2AP_PDU, res_pdu, error_buf, &errlen);
  printf("error length %d\n", errlen);
  printf("error buf %s\n", error_buf);

  auto er = asn_encode_to_buffer(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2AP_PDU, res_pdu, buffer, buffer_size);
//
  if (er.encoded == -1) {
    LOG_E("Failed to serialize message. Detail: %s.\n", asn_DEF_E2AP_PDU.name);
    exit(1);
  } else if (er.encoded > buffer_size) {
    LOG_E("Buffer of size %zu is too small for %s, need %zu\n", buffer_size, asn_DEF_E2AP_PDU.name, er.encoded);
    exit(1);
  }
//


  data.len = er.encoded;
  memcpy(data.buffer, buffer, er.encoded);

  //send response data over sctp
  if(sctp_send_data(socket_fd, data) > 0) {
    LOG_I("Sent E2-SERVICE-UPDATE");
  } else {
    LOG_E("Unable to send E2-SERVICE-UPDATE to peer");
  }
}

void e2ap_send_e2nodeConfigUpdate(int& socket_fd) {
  auto buffer_size = MAX_SCTP_BUFFER;
  unsigned char buffer[MAX_SCTP_BUFFER];
  E2AP_PDU_t* pdu = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));

  LOG_I("Generating E2 node configure update");

  encoding::generate_e2apv2_config_update(pdu);

  e2ap_asn1c_print_pdu(pdu);

  sctp_buffer_t data;

  char error_buf[300] = {
      0,
  };
  size_t errlen = 0;

  asn_check_constraints(&asn_DEF_E2AP_PDU, pdu, error_buf, &errlen);

  auto er = asn_encode_to_buffer(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2AP_PDU, pdu, buffer, buffer_size);

  if (er.encoded == -1) {
    LOG_E("Failed to serialize message. Detail: %s.\n", asn_DEF_E2AP_PDU.name);
    exit(1);
  } else if (er.encoded > buffer_size) {
    LOG_E("Buffer of size %zu is too small for %s, need %zu\n", buffer_size, asn_DEF_E2AP_PDU.name, er.encoded);
    exit(1);
  }

  data.len = er.encoded;

  memcpy(data.buffer, buffer, er.encoded);

  // send response data over sctp
  if (sctp_send_data(socket_fd, data) > 0) {
    LOG_I("Sent E2nodeConfigUpdate");
  } else {
    LOG_E("[SCTP] Unable to send E2nodeConfigUpdate to peer");
  }
}

void e2ap_handle_E2SetupRequest(E2AP_PDU_t* pdu, int &socket_fd) {
  auto* res_pdu = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));
  encoding::generate_e2apv1_setup_response(res_pdu);


  LOG_D("Created E2-SETUP-RESPONSE");

  e2ap_asn1c_print_pdu(res_pdu);

  auto buffer_size = MAX_SCTP_BUFFER;
  unsigned char buffer[MAX_SCTP_BUFFER];

  sctp_buffer_t data;
  auto er = asn_encode_to_buffer(nullptr, ATS_BASIC_XER, &asn_DEF_E2AP_PDU, res_pdu, buffer, buffer_size);


    //
  if (er.encoded == -1) {
    LOG_E("Failed to serialize message. Detail: %s.\n", asn_DEF_E2AP_PDU.name);
    exit(1);
  } else if (er.encoded > buffer_size) {
    LOG_E("Buffer of size %zu is too small for %s, need %zu\n", buffer_size, asn_DEF_E2AP_PDU.name, er.encoded);
    exit(1);
  }

    //

  data.len = er.encoded;

  memcpy(data.buffer, buffer, er.encoded);

  //send response data over sctp
  if(sctp_send_data(socket_fd, data) > 0) {
    LOG_I("Sent E2-SETUP-RESPONSE");
  } else {
    LOG_E("[SCTP] Unable to send E2-SETUP-RESPONSE to peer");
  }

  sleep(5);

  // Sending Subscription Request

  E2AP_PDU_t* pdu_sub = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));

  encoding::generate_e2apv1_subscription_request(pdu_sub);

  xer_fprint(stderr, &asn_DEF_E2AP_PDU, pdu_sub); 

  auto buffer_size2 = MAX_SCTP_BUFFER;
  unsigned char buffer2[MAX_SCTP_BUFFER];

  sctp_buffer_t data2;

  auto er2 = asn_encode_to_buffer(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2AP_PDU, pdu_sub, buffer2, buffer_size2);

  if (er2.encoded == -1) {
    LOG_E("Failed to serialize message. Detail: %s.\n", asn_DEF_E2AP_PDU.name);
    exit(1);
  } else if (er2.encoded > buffer_size2) {
    LOG_E("Buffer of size %zu is too small for %s, need %zu\n", buffer_size2, asn_DEF_E2AP_PDU.name, er2.encoded);
    exit(1);
  }


  data2.len = (int) er2.encoded;
  memcpy(data2.buffer, buffer2, er2.encoded);

  if(sctp_send_data(socket_fd, data2) > 0) {
    LOG_I("Sent E2-SUBSCRIPTION-REQUEST");
  } else {
    LOG_E("[SCTP] Unable to send E2-SUBSCRIPTION-REQUEST to peer");
  }
}

// To be deleted?
void e2ap_handle_RICSubscriptionDeleteRequest(E2AP_PDU_t* pdu, int &socket_fd, E2Sim *e2sim) {

    RICsubscriptionDeleteRequest_t orig_req = pdu->choice.initiatingMessage->value.choice.RICsubscriptionDeleteRequest;
// RICsubscriptionDeleteResponse_IEs

  int count = orig_req.protocolIEs.list.count;
  int size = orig_req.protocolIEs.list.size;

  RICsubscriptionDeleteRequest_IEs_t **ies = (RICsubscriptionDeleteRequest_IEs_t**)orig_req.protocolIEs.list.array;

  RICsubscriptionDeleteRequest_IEs__value_PR pres;
  
  uint16_t reqRequestorId {};
  uint16_t reqInstanceId {};
  uint16_t ranFuncionId {};


  for (int i = 0; i < count; i++) 
  {
    RICsubscriptionDeleteRequest_IEs_t *next_ie = ies[i];
    pres = next_ie->value.present; // value of the current IE
      
    switch(pres) 
    {
      // IE containing the RIC Request ID
      case RICsubscriptionDeleteRequest_IEs__value_PR_RICrequestID:
        {	
          RICrequestID_t reqId = next_ie->value.choice.RICrequestID;
          reqRequestorId = reqId.ricRequestorID;
          reqInstanceId = reqId.ricInstanceID;
          break;
        }
      // IE containing the RAN Function ID
      case RICsubscriptionDeleteRequest_IEs__value_PR_RANfunctionID:
        {
          ranFuncionId = next_ie->value.choice.RANfunctionID;
          break;
        }
        default:
        {
        //   NS_LOG_DEBUG ("in case default");	
          continue;
        }
    }
  }

    auto* res_pdu = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));
    encoding::generate_e2apv1_subscription_delete_acknowledge(res_pdu, reqRequestorId, reqInstanceId, ranFuncionId);

    LOG_D("[E2AP] Created RIC-SUBSCRIPTION-DELETE-ACKNOWLEDGE***");

    e2ap_asn1c_print_pdu(res_pdu);

    {
        uint8_t       *buf;
        sctp_buffer_t data;

        data.len = e2ap_asn1c_encode_pdu(res_pdu, &buf);
        memcpy(data.buffer, buf, (data.len < MAX_SCTP_BUFFER)? data.len : MAX_SCTP_BUFFER);

        printf("\n ****** e2ap_handle_RICSubscriptionDeleteRequest ************ \n");

        //send response data over sctp
        if (sctp_send_data(socket_fd, data) > 0) {
            LOG_I("[SCTP] Sent RIC-SUBSCRIPTION-DELETE-ACKNOWLEDGE");
        } else {
            LOG_E("[SCTP] Unable to send RIC-SUBSCRIPTION-DELETE-ACKNOWLEDGE to peer");
        }
    }
}

// TODO: Mostafa ????? jlee???
void e2ap_handle_RICControlRequest(E2AP_PDU_t *pdu, int &socket_fd, E2Sim *e2sim) {
    // long func_id = 3;

    RICcontrolRequest_t orig_req = pdu->choice.initiatingMessage->value.choice.RICcontrolRequest;

  int count = orig_req.protocolIEs.list.count;
  int size = orig_req.protocolIEs.list.size;

  RICcontrolRequest_IEs_t **ies = (RICcontrolRequest_IEs_t**)orig_req.protocolIEs.list.array;

  RICcontrolRequest_IEs__value_PR pres;
  
  uint16_t reqRequestorId {};
  uint16_t reqInstanceId {};
  uint16_t ranFuncionId {};

  
  // iterate over the IEs
  for (int i = 0; i < count; i++) 
  {
    RICcontrolRequest_IEs_t *next_ie = ies[i];
    pres = next_ie->value.present; // value of the current IE
      
    switch(pres) 
    {
      // IE containing the RIC Request ID
      case RICcontrolRequest_IEs__value_PR_RICrequestID:
        {
        //   NS_LOG_DEBUG ("Processing RIC Request ID field");	
          RICrequestID_t reqId = next_ie->value.choice.RICrequestID;
          reqRequestorId = reqId.ricRequestorID;
          reqInstanceId = reqId.ricInstanceID;
        //   NS_LOG_DEBUG ( "RIC Requestor ID " << reqRequestorId);
        //   NS_LOG_DEBUG ( "RIC Instance ID " << reqInstanceId);
          break;
        }
      // IE containing the RAN Function ID
      case RICcontrolRequest_IEs__value_PR_RANfunctionID:
        {
        //   NS_LOG_DEBUG ("Processing RAN Function ID field");	
              ranFuncionId = next_ie->value.choice.RANfunctionID;

              SmCallback cb;

            LOG_I("****** e2ap_handle_RICControlRequest ******");

            bool func_exists = true;
            try {
                // cb = e2sim->get_sm_callback(func_id);
                cb = e2sim->get_sm_callback(ranFuncionId);

            } catch (const std::out_of_range &e) {
                func_exists = false;
            }

            if (func_exists) {
                LOG_D("Calling callback function");
                cb(pdu);
                LOG_D("*** DONE Calling callback function ****");
            } else {
                LOG_E("Error: No RAN Function with this ID exists");
            }

        //   NS_LOG_DEBUG ("RAN Function ID " << ranFuncionId);
          break;
        }
        default:
        {
        //   NS_LOG_DEBUG ("in case default");	
          continue;
        }
    }
  }


    auto* res_pdu = (E2AP_PDU_t*)calloc(1, sizeof(E2AP_PDU));
    encoding::generate_e2apv1_ric_control_acknowledge(res_pdu, reqRequestorId, reqInstanceId, ranFuncionId);

    LOG_D("[E2AP] Created E2-RIC-CONTROL-ACKNOWLEDGE");

    e2ap_asn1c_print_pdu(res_pdu);

    if(false) {
        // Disable due Bug - not right encode
        auto buffer_size = MAX_SCTP_BUFFER;
        unsigned char buffer[MAX_SCTP_BUFFER];

        sctp_buffer_t data;
        auto er = asn_encode_to_buffer(nullptr, ATS_BASIC_XER, &asn_DEF_E2AP_PDU, res_pdu, buffer, buffer_size);

        LOG_D("er encoded is %zd\n", er.encoded);
        data.len = (int) er.encoded;

        memcpy(data.buffer, buffer, er.encoded);

        // m_e2sim->encode_and_send_sctp_data(e2ap_pdu);
        //send response data over sctp
        if (sctp_send_data(socket_fd, data) > 0) {
            LOG_I("[SCTP] Sent E2-SERVICE-UPDATE");
        } else {
            LOG_E("[SCTP] Unable to send E2-SERVICE-UPDATE to peer");
        }
    } else {
        uint8_t       *buf;
        sctp_buffer_t data;

        data.len = e2ap_asn1c_encode_pdu(res_pdu, &buf);
        memcpy(data.buffer, buf, (data.len < MAX_SCTP_BUFFER)? data.len : MAX_SCTP_BUFFER);

        printf("\n ****** e2ap_handle_RICControlRequest End Print buffer ************ \n");

        // m_e2sim->encode_and_send_sctp_data(e2ap_pdu);
        //send response data over sctp
        if (sctp_send_data(socket_fd, data) > 0) {
            LOG_I("[SCTP] Sent E2-SERVICE-UPDATE");
        } else {
            LOG_E("[SCTP] Unable to send E2-SERVICE-UPDATE to peer");
        }
    }

    // // m_e2sim->encode_and_send_sctp_data(e2ap_pdu);
    // //send response data over sctp
    // if (sctp_send_data(socket_fd, data) > 0) {
    //     LOG_I("[SCTP] Sent E2-SERVICE-UPDATE");
    // } else {
    //     LOG_E("[SCTP] Unable to send E2-SERVICE-UPDATE to peer");
    // }

}



