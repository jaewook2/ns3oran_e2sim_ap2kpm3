/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-RC-IEs"
 * 	found in "e2sm_rc_v1_03_standard.asn"
 * 	`asn1c -fcompound-names -fno-include-deps -findirect-choice -pdu=auto -no-gen-example`
 */

#include "RANParameter-Testing-Item-Choice-ElementFalse.h"

#include "RANParameter-Value.h"
asn_TYPE_member_t asn_MBR_RANParameter_Testing_Item_Choice_ElementFalse_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct RANParameter_Testing_Item_Choice_ElementFalse, ranParameter_TestCondition),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_RANParameter_TestingCondition,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"ranParameter-TestCondition"
		},
	{ ATF_POINTER, 2, offsetof(struct RANParameter_Testing_Item_Choice_ElementFalse, ranParameter_Value),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_RANParameter_Value,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"ranParameter-Value"
		},
	{ ATF_POINTER, 1, offsetof(struct RANParameter_Testing_Item_Choice_ElementFalse, logicalOR),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_LogicalOR,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"logicalOR"
		},
};
static const int asn_MAP_RANParameter_Testing_Item_Choice_ElementFalse_oms_1[] = { 1, 2 };
static const ber_tlv_tag_t asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_RANParameter_Testing_Item_Choice_ElementFalse_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* ranParameter-TestCondition */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* ranParameter-Value */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* logicalOR */
};
asn_SEQUENCE_specifics_t asn_SPC_RANParameter_Testing_Item_Choice_ElementFalse_specs_1 = {
	sizeof(struct RANParameter_Testing_Item_Choice_ElementFalse),
	offsetof(struct RANParameter_Testing_Item_Choice_ElementFalse, _asn_ctx),
	asn_MAP_RANParameter_Testing_Item_Choice_ElementFalse_tag2el_1,
	3,	/* Count of tags in the map */
	asn_MAP_RANParameter_Testing_Item_Choice_ElementFalse_oms_1,	/* Optional members */
	2, 0,	/* Root/Additions */
	3,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse = {
	"RANParameter-Testing-Item-Choice-ElementFalse",
	"RANParameter-Testing-Item-Choice-ElementFalse",
	&asn_OP_SEQUENCE,
	asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1,
	sizeof(asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1)
		/sizeof(asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1[0]), /* 1 */
	asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1,	/* Same as above */
	sizeof(asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1)
		/sizeof(asn_DEF_RANParameter_Testing_Item_Choice_ElementFalse_tags_1[0]), /* 1 */
	{
#if !defined(ASN_DISABLE_OER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
#if !defined(ASN_DISABLE_JER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_JER_SUPPORT) */
		SEQUENCE_constraint
	},
	asn_MBR_RANParameter_Testing_Item_Choice_ElementFalse_1,
	3,	/* Elements count */
	&asn_SPC_RANParameter_Testing_Item_Choice_ElementFalse_specs_1	/* Additional specs */
};

