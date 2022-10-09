#ifndef _TYPE_CONSTANTS_H
#define _TYPE_CONSTANTS_H

/// Data Types
enum {
	B_ANY_TYPE					 = 'ANYT',
	B_BOOL_TYPE					 = 'BOOL',
	B_CHAR_TYPE					 = 'CHAR',
	B_COLOR_8_BIT_TYPE			 = 'CLRB',
	B_DOUBLE_TYPE				 = 'DBLE',
	B_FLOAT_TYPE				 = 'FLOT',
	B_GRAYSCALE_8_BIT_TYPE		 = 'GRYB',
	B_INT64_TYPE				 = 'LLNG',
	B_INT32_TYPE				 = 'LONG',
	B_INT16_TYPE				 = 'SHRT',
	B_INT8_TYPE					 = 'BYTE',
	B_MESSAGE_TYPE				 = 'MSGG',
	B_MESSENGER_TYPE			 = 'MSNG',
	B_MIME_TYPE					 = 'MIME',
	B_MONOCHROME_1_BIT_TYPE		 = 'MNOB',
	B_OBJECT_TYPE				 = 'OPTR',
	B_OFF_T_TYPE				 = 'OFFT',
	B_PATTERN_TYPE				 = 'PATN',
	B_POINTER_TYPE				 = 'PNTR',
	B_POINT_TYPE				 = 'BPNT',
	B_RAW_TYPE					 = 'RAWT',
	B_RECT_TYPE					 = 'RECT',
	B_REF_TYPE					 = 'RREF',
	B_RGB_32_BIT_TYPE			 = 'RGBB',
	B_RGB_COLOR_TYPE			 = 'RGBC',
	B_SIZE_T_TYPE				 = 'SIZT',
	B_SSIZE_T_TYPE				 = 'SSZT',
	B_STRING_TYPE				 = 'CSTR',
	B_TIME_TYPE					 = 'TIME',
	B_UINT64_TYPE				 = 'ULLG',
	B_UINT32_TYPE				 = 'ULNG',
	B_UINT16_TYPE				 = 'USHT',
	B_UINT8_TYPE				 = 'UBYT',
	B_MEDIA_PARAMETER_TYPE		 = 'BMCT',
	B_MEDIA_PARAMETER_WEB_TYPE	 = 'BMCW',
	B_MEDIA_PARAMETER_GROUP_TYPE = 'BMCG',
	B_ATOM_TYPE					 = 'ATOM',
	B_ATOMREF_TYPE				 = 'ATMR',

	/* deprecated, do not use */
	B_ASCII_TYPE = 'TEXT' /* use B_STRING_TYPE instead */
};

#endif /* _TYPE_CONSTANTS_H */
