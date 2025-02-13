/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "E2SM-KPM-IEs"
 * 	found in "/home/rshacham/e2sm-kpm-v01.02.asn"
 * 	`asn1c -fcompound-names`
 */

#ifndef	_RIC_ReportStyle_List_H_
#define	_RIC_ReportStyle_List_H_


#include <asn_application.h>

/* Including external dependencies */
#include "RIC-Style-Type.h"
#include "RIC-Style-Name.h"
#include "RIC-Format-Type.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RIC-ReportStyle-List */
typedef struct RIC_ReportStyle_List {
	RIC_Style_Type_t	 ric_ReportStyle_Type;
	RIC_Style_Name_t	 ric_ReportStyle_Name;
	RIC_Format_Type_t	 ric_IndicationHeaderFormat_Type;
	RIC_Format_Type_t	 ric_IndicationMessageFormat_Type;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RIC_ReportStyle_List_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RIC_ReportStyle_List;
extern asn_SEQUENCE_specifics_t asn_SPC_RIC_ReportStyle_List_specs_1;
extern asn_TYPE_member_t asn_MBR_RIC_ReportStyle_List_1[4];

#ifdef __cplusplus
}
#endif

#endif	/* _RIC_ReportStyle_List_H_ */
#include <asn_internal.h>
