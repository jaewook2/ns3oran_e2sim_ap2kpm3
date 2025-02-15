/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-KPM-IEs"
 * 	found in "e2sm-kpm-v03.00.asn"
 * 	`asn1c -pdu=auto -fincludes-quoted -fcompound-names -findirect-choice -fno-include-deps -no-gen-example -no-gen-OER -D /tmp/workspace/oransim-gerrit/e2sim/asn1c/`
 */

#ifndef	_RIC_EventTriggerStyle_Item_H_
#define	_RIC_EventTriggerStyle_Item_H_


#include "asn_application.h"

/* Including external dependencies */
#include "RIC-Style-Type.h"
#include "RIC-Style-Name.h"
#include "RIC-Format-Type.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RIC-EventTriggerStyle-Item */
typedef struct RIC_EventTriggerStyle_Item {
	RIC_Style_Type_t	 ric_EventTriggerStyle_Type;
	RIC_Style_Name_t	 ric_EventTriggerStyle_Name;
	RIC_Format_Type_t	 ric_EventTriggerFormat_Type;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RIC_EventTriggerStyle_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RIC_EventTriggerStyle_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_RIC_EventTriggerStyle_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_RIC_EventTriggerStyle_Item_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _RIC_EventTriggerStyle_Item_H_ */
#include "asn_internal.h"
