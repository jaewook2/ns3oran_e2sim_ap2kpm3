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
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>


#include "e2sim.hpp"
#include "e2sim_defs.h"
#include "e2sim_sctp.hpp"
#include "e2ap_message_handler.hpp"
#include "encode_e2apv1.hpp"
#include "signal_handler.hpp"
#include "RANfunctionOID.h" // add by jlee


using namespace std;

std::unordered_map<long , OCTET_STRING_t*> E2Sim::getRegistered_ran_functions() {
  return ran_functions_registered;
}

void E2Sim::register_subscription_callback(long func_id, SubscriptionCallback cb) {
  LOG_I("%%%%about to register callback for subscription for func_id %ld", func_id);
  subscription_callbacks[func_id] = cb;
}

SubscriptionCallback E2Sim::get_subscription_callback(long func_id) {
  LOG_I("%%%%we are getting the subscription callback for func id %ld", func_id);
  SubscriptionCallback cb;

  try {
    cb = subscription_callbacks.at(func_id);
  } catch(const std::out_of_range& e) {
    throw std::out_of_range("Function ID is not registered");
  }
  return cb;
}
// to be deleted? ns3?
void E2Sim::register_sm_callback(long func_id, SmCallback cb) {
    LOG_I("%%%%about to register callback for e2sm for func_id %ld", func_id);
    sm_callbacks[func_id] = cb;
}
// to be deleted? ns3?
void E2Sim::register_callback(long cb_id, CallbackFunction cb) {
    LOG_I("%%%%about to register callback function for e2sm for cb_id %ld", cb_id);
    callbackfunctions[cb_id] = cb;
}
// to be deleted? ns3?
CallbackFunction E2Sim::get_callback(long cb_id) {
    LOG_I("%%%%we are getting the e2sm callback function for cb_id %ld\n", cb_id);
    CallbackFunction cb;
    try {
        cb = callbackfunctions.at(cb_id);
    } catch (const std::out_of_range &e) {
        LOG_E("Function ID is not registered");
        throw std::out_of_range("Function ID is not registered");
    }
    return cb;
}
// to be deleted? ns3?

SmCallback E2Sim::get_sm_callback(long func_id) {
    LOG_I("%%%%we are getting the e2sm callback for func id %ld\n", func_id);
    SmCallback cb;
    try {
        cb = sm_callbacks.at(func_id);
    } catch (const std::out_of_range &e) {
        LOG_E("Function ID is not registered");
        throw std::out_of_range("Function ID is not registered");
    }
    return cb;
}

void E2Sim::register_e2sm(long func_id, OCTET_STRING_t *ostr) {

  //Error conditions:
  //If we already have an entry for func_id

  LOG_I("%%%%about to register e2sm func desc for %ld\n", func_id);
  ran_functions_registered[func_id] = ostr;

}


void E2Sim::encode_and_send_sctp_data(E2AP_PDU_t* pdu)
{
  uint8_t       *buf;
  sctp_buffer_t data;

  data.len = e2ap_asn1c_encode_pdu(pdu, &buf);
  memcpy(data.buffer, buf, min(data.len, MAX_SCTP_BUFFER));
  if (buf) free(buf);
  LOG_I("Send data to client_fd %d", client_fd)
  sctp_send_data(client_fd, data);
}


void E2Sim::wait_for_sctp_data()
{
  sctp_buffer_t recv_buf;
  if(sctp_receive_data(client_fd, recv_buf) > 0)
  {
    LOG_I("[SCTP] Received new data of size %d", recv_buf.len);
    e2ap_handle_sctp_data(client_fd, recv_buf, false, this);
  }
}


void E2Sim::generate_e2apv1_subscription_response_success(E2AP_PDU *e2ap_pdu, long reqActionIdsAccepted[], long reqActionIdsRejected[], int accept_size, int reject_size, long reqRequestorId, long reqInstanceId) {
  encoding::generate_e2apv1_subscription_response_success(e2ap_pdu, reqActionIdsAccepted, reqActionIdsRejected, accept_size, reject_size, reqRequestorId, reqInstanceId);
}

void E2Sim::generate_e2apv1_indication_request_parameterized(E2AP_PDU *e2ap_pdu, long requestorId, long instanceId, long ranFunctionId, long actionId, long seqNum, uint8_t *ind_header_buf, int header_length, uint8_t *ind_message_buf, int message_length) {
  encoding::generate_e2apv1_indication_request_parameterized(e2ap_pdu, requestorId, instanceId, ranFunctionId, actionId, seqNum, ind_header_buf, header_length, ind_message_buf, message_length);
}

int E2Sim::run_loop(int argc, char* argv[]){
  options_t ops = read_input_options(argc, argv);
  LOG_D("After reading input options");
  return run_loop(ops.server_ip,ops.server_port,ops.client_port,ops.gnb_id,ops.plmn_id);
}

int E2Sim::run_loop(std::string server_ip, uint16_t server_port, uint16_t local_port, std::string gnb_id, std::string plmn_id) {
    LOG_I("Start E2 Agent (E2 Simulator)");
    LOG_D("Current Log level is %d", LOG_LEVEL);

    client_fd = sctp_start_client(server_ip.c_str(), server_port, local_port);
    //auto *pdu_setup = (E2AP_PDU_t *) calloc(1, sizeof(E2AP_PDU));
    E2AP_PDU_t* pdu_setup = (E2AP_PDU_t*)calloc(1,sizeof(E2AP_PDU));

    LOG_I("SCTP client has been started");
    LOG_D("client_fd value is %d\n", client_fd);

    std::vector<encoding::ran_func_info> all_funcs;
    //0210 adding 
    RANfunctionOID_t *ranFunctionOIDe = (RANfunctionOID_t*)calloc(1,sizeof(RANfunctionOID_t));
    uint8_t *buf = (uint8_t*)"OID123";
    ranFunctionOIDe->buf = (uint8_t*)calloc(1,strlen((char*)buf)+1);
    memcpy(ranFunctionOIDe->buf, buf, strlen((char*)buf)+1);
    ranFunctionOIDe->size = strlen((char*)buf);

    // Loop through RAN function definitions that are registered
    //0210 adding log
    LOG_I("Constructing a list of RAN functions based on registered information");
    for (std::pair<long, OCTET_STRING_t*> elem : ran_functions_registered) {
        // 0210 adding
        char* ran_desc = (char*) calloc(1, elem.second->size+1);
        ran_desc = (char*)elem.second->buf;
        ran_desc[elem.second->size] = '\0';
        // 0210 **
        LOG_I("Adding RAN function ID %ld, description: %s to the list", elem.first, ran_desc);

        //encoding::ran_func_info next_func{};
       encoding::ran_func_info next_func;

        next_func.ranFunctionId = elem.first;
        next_func.ranFunctionDesc = elem.second;
        next_func.ranFunctionRev = (long)2;
        //0210 adding
        next_func.ranFunctionOId = ranFunctionOIDe;

        all_funcs.push_back(next_func);
    }

    //LOG_D("About to call setup request encode");
    LOG_I("Generate E2AP v1 setup request for all registered RAN functions");
    generate_e2apv1_setup_request_parameterized(pdu_setup, all_funcs);

    //generate_e2apv1_setup_request_parameterized(pdu_setup, all_funcs, (uint8_t *) gnb_id.c_str(), (uint8_t *) plmn_id.c_str());

    //LOG_D("After generating e2setup req");
    //LOG_D("*After XER Encoding");
    /*
    sctp_buffer_t data;
    memset(data_buf.buffer, 0, MAX_SCTP_BUFFER);
    data_buf.len = 0;

    char *error_buf = (char*)calloc(300, sizeof(char));
    size_t errlen;

    auto err_ret = asn_check_constraints(&asn_DEF_E2AP_PDU, pdu_setup, error_buf, &errlen);
    if (err_ret == -1) {
        LOG_E("E2 Setup request constraints check failed, reason:\n%s", error_buf);
    }
    //auto er = asn_encode_to_buffer(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2AP_PDU, pdu_setup, data_buf.buffer, MAX_SCTP_BUFFER);

    */
    // 0210 add
    xer_fprint(stderr, &asn_DEF_E2AP_PDU, pdu_setup);

    auto buffer_size = MAX_SCTP_BUFFER;
    unsigned char buffer[MAX_SCTP_BUFFER];

    sctp_buffer_t data;

    char error_buf[300] = {0, };
    size_t errlen = 0;
    asn_check_constraints(&asn_DEF_E2AP_PDU, pdu_setup, error_buf, &errlen);
    LOG_I("Error length %d, error buf %s", errlen, error_buf);

    auto er = asn_encode_to_buffer(nullptr, ATS_ALIGNED_BASIC_PER, &asn_DEF_E2AP_PDU, pdu_setup, buffer, buffer_size);
    data.len = er.encoded;
    LOG_I("Error encoded %d", er.encoded);
    memcpy(data.buffer, buffer, er.encoded);
    // 0210 **
    
    if(sctp_send_data(client_fd, data) > 0) {
        LOG_I("Sent E2-SETUP-REQUEST as E2AP message");
    } else {
        LOG_E("[SCTP] Unable to send E2-SETUP-REQUEST to peer");
    }

    //LOG_D("[SCTP] Waiting for SCTP data");
    // 0210 add

    buffer_size = MAX_SCTP_BUFFER;
    memset(buffer, '\0', sizeof(buffer));

    sctp_buffer_t recv_buf;

    LOG_I("Waiting for SCTP data");

    try {
        SignalHandler signalHandler;
        //constantly looking for data on SCTP interface
        while (SignalHandler::isRunning())
        {
            if (sctp_receive_data(client_fd, recv_buf) <= 0)
                break;

            LOG_D("[SCTP] Received new data of size %d", recv_buf.len);

            /* if true means delete reuqest*/
            if(e2ap_handle_sctp_data(client_fd, recv_buf, this)) {

              LOG_D("[SCTP] Application will be terminated");

              if(sctp_close_clinet_connection(client_fd)) {
                LOG_D("[SCTP] Connection Terminated");
              }
              // TODO: Mostafa, need to terminate detached thread, 
              // and indication message thread.
              break;
            }

        }
    } catch (SignalException &e) {
        LOG_E("SIGINT raised, possible cause: %s", strsignal(SIGINT));
    }

    return EXIT_SUCCESS;
}
