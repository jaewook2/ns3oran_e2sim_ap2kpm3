/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-KPM-IEs"
 * 	found in "E2SM-KPM-v02.00.03.asn"
 * 	`asn1c -pdu=auto -fno-include-deps -fcompound-names -findirect-choice -gen-PER -gen-OER -no-gen-example -D E2SM-KPM-v02.00.03`
 */

#include "CellGlobalID.h"

#include "NRCGI.h"
#include "EUTRACGI.h"
/*
static asn_oer_constraints_t asn_OER_type_CellGlobalID_constr_1 CC_NOTUSED = {
	{ 0, 0 },
	-1};
*/
asn_per_constraints_t asn_PER_type_CellGlobalID_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  1,  1,  0,  1 }	/* (0..1,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_CellGlobalID_1[] = {
	{ ATF_POINTER, 0, offsetof(struct CellGlobalID, choice.nr_CGI),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NRCGI,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"nr-CGI"
		},
	{ ATF_POINTER, 0, offsetof(struct CellGlobalID, choice.eUTRA_CGI),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_EUTRACGI,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"eUTRA-CGI"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_CellGlobalID_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* nr-CGI */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* eUTRA-CGI */
};
asn_CHOICE_specifics_t asn_SPC_CellGlobalID_specs_1 = {
	sizeof(struct CellGlobalID),
	offsetof(struct CellGlobalID, _asn_ctx),
	offsetof(struct CellGlobalID, present),
	sizeof(((struct CellGlobalID *)0)->present),
	asn_MAP_CellGlobalID_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0,
	2	/* Extensions start */
};
/*
asn_TYPE_descriptor_t asn_DEF_CellGlobalID = {
	"CellGlobalID",
	"CellGlobalID",
	&asn_OP_CHOICE,
	0,	
	0,	
	0,	
	0,	
	{ &asn_OER_type_CellGlobalID_constr_1, &asn_PER_type_CellGlobalID_constr_1, CHOICE_constraint },
	asn_MBR_CellGlobalID_1,
	2,	
	&asn_SPC_CellGlobalID_specs_1	
};
*/
