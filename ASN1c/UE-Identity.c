/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2sm-kpm-rc"
 * 	found in "e2sm-kpm-rc.asn"
 * 	`asn1c -no-gen-BER -no-gen-UPER -no-gen-OER -no-gen-JER -fcompound-names -no-gen-example -findirect-choice -fno-include-deps`
 */

#include "UE-Identity.h"

/*
 * This type is implemented using OCTET_STRING,
 * so here we adjust the DEF accordingly.
 */
static const ber_tlv_tag_t asn_DEF_UE_Identity_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))
};
asn_TYPE_descriptor_t asn_DEF_UE_Identity = {
	"UE-Identity",
	"UE-Identity",
	&asn_OP_OCTET_STRING,
	asn_DEF_UE_Identity_tags_1,
	sizeof(asn_DEF_UE_Identity_tags_1)
		/sizeof(asn_DEF_UE_Identity_tags_1[0]), /* 1 */
	asn_DEF_UE_Identity_tags_1,	/* Same as above */
	sizeof(asn_DEF_UE_Identity_tags_1)
		/sizeof(asn_DEF_UE_Identity_tags_1[0]), /* 1 */
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
		OCTET_STRING_constraint
	},
	0, 0,	/* No members */
	&asn_SPC_OCTET_STRING_specs	/* Additional specs */
};

