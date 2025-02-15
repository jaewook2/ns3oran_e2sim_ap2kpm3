/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2AP-IEs"
 * 	found in "e2ap-ied-v03.01.asn"
 * 	`asn1c -pdu=auto -fincludes-quoted -fcompound-names -findirect-choice -fno-include-deps -no-gen-example -no-gen-OER -D /tmp/workspace/oransim-gerrit/e2sim/asn1c/`
 */

#ifndef	_E2AP_IEs_GlobalENB_ID_H_
#define	_E2AP_IEs_GlobalENB_ID_H_


#include "asn_application.h"

/* Including external dependencies */
#include "PLMN-Identity.h"
#include "E2AP-IEs_ENB-ID.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* E2AP-IEs_GlobalENB-ID */
typedef struct E2AP_IEs_GlobalENB_ID {
	PLMN_Identity_t	 pLMN_Identity;
	E2AP_IEs_ENB_ID_t	 eNB_ID;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2AP_IEs_GlobalENB_ID_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2AP_IEs_GlobalENB_ID;
extern asn_SEQUENCE_specifics_t asn_SPC_E2AP_IEs_GlobalENB_ID_specs_1;
extern asn_TYPE_member_t asn_MBR_E2AP_IEs_GlobalENB_ID_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _E2AP_IEs_GlobalENB_ID_H_ */
#include "asn_internal.h"
