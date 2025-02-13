/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-RC-IEs"
 * 	found in "e2sm_rc_v1_03_standard.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -no-gen-example`
 */

#ifndef	_RANParameter_Definition_Choice_LIST_Item_H_
#define	_RANParameter_Definition_Choice_LIST_Item_H_


#include <asn_application.h>

/* Including external dependencies */
#include "RANParameter-ID.h"
#include "RANParameter-Name.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct RANParameter_Definition;

/* RANParameter-Definition-Choice-LIST-Item */
typedef struct RANParameter_Definition_Choice_LIST_Item {
	RANParameter_ID_t	 ranParameter_ID;
	RANParameter_Name_t	 ranParameter_name;
	struct RANParameter_Definition	*ranParameter_Definition;	/* OPTIONAL */
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RANParameter_Definition_Choice_LIST_Item_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RANParameter_Definition_Choice_LIST_Item;
extern asn_SEQUENCE_specifics_t asn_SPC_RANParameter_Definition_Choice_LIST_Item_specs_1;
extern asn_TYPE_member_t asn_MBR_RANParameter_Definition_Choice_LIST_Item_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _RANParameter_Definition_Choice_LIST_Item_H_ */
#include <asn_internal.h>
