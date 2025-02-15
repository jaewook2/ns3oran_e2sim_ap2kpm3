/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-KPM-IEs"
 * 	found in "e2sm-kpm-v03.00.asn"
 * 	`asn1c -pdu=auto -fincludes-quoted -fcompound-names -findirect-choice -fno-include-deps -no-gen-example -no-gen-OER -D /tmp/workspace/oransim-gerrit/e2sim/asn1c/`
 */

#include "E2SM-KPM-ActionDefinition-Format1.h"

#include "CGI.h"
#include "DistMeasurementBinRangeList.h"
asn_TYPE_member_t asn_MBR_E2SM_KPM_ActionDefinition_Format1_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct E2SM_KPM_ActionDefinition_Format1, measInfoList),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_MeasurementInfoList,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"measInfoList"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct E2SM_KPM_ActionDefinition_Format1, granulPeriod),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GranularityPeriod,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"granulPeriod"
		},
	{ ATF_POINTER, 2, offsetof(struct E2SM_KPM_ActionDefinition_Format1, cellGlobalID),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CGI,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"cellGlobalID"
		},
	{ ATF_POINTER, 1, offsetof(struct E2SM_KPM_ActionDefinition_Format1, distMeasBinRangeInfo),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DistMeasurementBinRangeList,
		0,
		{
#if !defined(ASN_DISABLE_OER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
			0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
			0
		},
		0, 0, /* No default value */
		"distMeasBinRangeInfo"
		},
};
static const int asn_MAP_E2SM_KPM_ActionDefinition_Format1_oms_1[] = { 2, 3 };
static const ber_tlv_tag_t asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_E2SM_KPM_ActionDefinition_Format1_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* measInfoList */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* granulPeriod */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* cellGlobalID */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* distMeasBinRangeInfo */
};
asn_SEQUENCE_specifics_t asn_SPC_E2SM_KPM_ActionDefinition_Format1_specs_1 = {
	sizeof(struct E2SM_KPM_ActionDefinition_Format1),
	offsetof(struct E2SM_KPM_ActionDefinition_Format1, _asn_ctx),
	asn_MAP_E2SM_KPM_ActionDefinition_Format1_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_E2SM_KPM_ActionDefinition_Format1_oms_1,	/* Optional members */
	1, 1,	/* Root/Additions */
	3,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_E2SM_KPM_ActionDefinition_Format1 = {
	"E2SM-KPM-ActionDefinition-Format1",
	"E2SM-KPM-ActionDefinition-Format1",
	&asn_OP_SEQUENCE,
	asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1,
	sizeof(asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1)
		/sizeof(asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1[0]), /* 1 */
	asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1,	/* Same as above */
	sizeof(asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1)
		/sizeof(asn_DEF_E2SM_KPM_ActionDefinition_Format1_tags_1[0]), /* 1 */
	{
#if !defined(ASN_DISABLE_OER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_OER_SUPPORT) */
#if !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT)
		0,
#endif  /* !defined(ASN_DISABLE_UPER_SUPPORT) || !defined(ASN_DISABLE_APER_SUPPORT) */
		SEQUENCE_constraint
	},
	asn_MBR_E2SM_KPM_ActionDefinition_Format1_1,
	4,	/* Elements count */
	&asn_SPC_E2SM_KPM_ActionDefinition_Format1_specs_1	/* Additional specs */
};

