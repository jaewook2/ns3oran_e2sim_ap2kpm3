/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2sm-kpm-rc"
 * 	found in "e2sm-kpm-rc.asn"
 * 	`asn1c -no-gen-BER -no-gen-UPER -no-gen-OER -no-gen-JER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps`
 */

#include "L3-RRC-Measurements.h"

#include "ServingCellMeasurements.h"
#include "MeasResultNeighCells.h"
asn_TYPE_member_t asn_MBR_L3_RRC_Measurements_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct L3_RRC_Measurements, rrcEvent),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RRCEvent,
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
		"rrcEvent"
		},
	{ ATF_POINTER, 2, offsetof(struct L3_RRC_Measurements, servingCellMeasurements),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ServingCellMeasurements,
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
		"servingCellMeasurements"
		},
	{ ATF_POINTER, 1, offsetof(struct L3_RRC_Measurements, measResultNeighCells),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_MeasResultNeighCells,
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
		"measResultNeighCells"
		},
};
static const int asn_MAP_L3_RRC_Measurements_oms_1[] = { 1, 2 };
static const ber_tlv_tag_t asn_DEF_L3_RRC_Measurements_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_L3_RRC_Measurements_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* rrcEvent */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* servingCellMeasurements */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* measResultNeighCells */
};
asn_SEQUENCE_specifics_t asn_SPC_L3_RRC_Measurements_specs_1 = {
	sizeof(struct L3_RRC_Measurements),
	offsetof(struct L3_RRC_Measurements, _asn_ctx),
	asn_MAP_L3_RRC_Measurements_tag2el_1,
	3,	/* Count of tags in the map */
	asn_MAP_L3_RRC_Measurements_oms_1,	/* Optional members */
	2, 0,	/* Root/Additions */
	3,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_L3_RRC_Measurements = {
	"L3-RRC-Measurements",
	"L3-RRC-Measurements",
	&asn_OP_SEQUENCE,
	asn_DEF_L3_RRC_Measurements_tags_1,
	sizeof(asn_DEF_L3_RRC_Measurements_tags_1)
		/sizeof(asn_DEF_L3_RRC_Measurements_tags_1[0]), /* 1 */
	asn_DEF_L3_RRC_Measurements_tags_1,	/* Same as above */
	sizeof(asn_DEF_L3_RRC_Measurements_tags_1)
		/sizeof(asn_DEF_L3_RRC_Measurements_tags_1[0]), /* 1 */
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
	asn_MBR_L3_RRC_Measurements_1,
	3,	/* Elements count */
	&asn_SPC_L3_RRC_Measurements_specs_1	/* Additional specs */
};

