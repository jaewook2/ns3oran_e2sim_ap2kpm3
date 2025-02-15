/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-RC-IEs"
 * 	found in "e2sm_rc_v1_03_standard.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -no-gen-example`
 */

#ifndef	_E2SM_RC_EventTrigger_Format1_H_
#define	_E2SM_RC_EventTrigger_Format1_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct EventTrigger_UE_Info;
struct E2SM_RC_EventTrigger_Format1_Item;

/* E2SM-RC-EventTrigger-Format1 */
typedef struct E2SM_RC_EventTrigger_Format1 {
	struct E2SM_RC_EventTrigger_Format1__message_List {
		A_SEQUENCE_OF(struct E2SM_RC_EventTrigger_Format1_Item) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} message_List;
	struct EventTrigger_UE_Info	*globalAssociatedUEInfo;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} E2SM_RC_EventTrigger_Format1_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_E2SM_RC_EventTrigger_Format1;
extern asn_SEQUENCE_specifics_t asn_SPC_E2SM_RC_EventTrigger_Format1_specs_1;
extern asn_TYPE_member_t asn_MBR_E2SM_RC_EventTrigger_Format1_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _E2SM_RC_EventTrigger_Format1_H_ */
#include <asn_internal.h>
