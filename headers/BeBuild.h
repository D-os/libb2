/*
 * Copyright 2007-2012, Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _BE_BUILD_H
#define _BE_BUILD_H


#define B_BEOS_VERSION_4				0x0400
#define B_BEOS_VERSION_4_5				0x0450
#define B_BEOS_VERSION_5				0x0500

#define B_BEOS_VERSION					B_BEOS_VERSION_5
#define B_BEOS_VERSION_MAUI				B_BEOS_VERSION_5

/* Haiku ABI */
#define B_HAIKU_ABI_MAJOR				0xffff0000
#define B_HAIKU_ABI_GCC_2				0x00020000
#define B_HAIKU_ABI_GCC_4				0x00040000

#define B_HAIKU_ABI_GCC_2_ANCIENT		0x00020000
#define B_HAIKU_ABI_GCC_2_BEOS			0x00020001
#define B_HAIKU_ABI_GCC_2_HAIKU			0x00020002

#define B_HAIKU_ABI_NAME				__HAIKU_ARCH_ABI

#if __GNUC__ == 2
#	define B_HAIKU_ABI					B_HAIKU_ABI_GCC_2_HAIKU
#elif __GNUC__ == 4 || __GNUC__ == 5 || __GNUC__ == 6
#	define B_HAIKU_ABI					B_HAIKU_ABI_GCC_4
#else
#	error Unsupported gcc version!
#endif


#define _UNUSED(argument) argument
#define _PACKED __attribute__((packed))
#define _PRINTFLIKE(_format_, _args_) \
	__attribute__((format(__printf__, _format_, _args_)))
#define _EXPORT
#define _IMPORT

#define B_DEFINE_SYMBOL_VERSION(function, versionedSymbol)	\
	__asm__(".symver " function "," versionedSymbol)


#ifdef __cplusplus
#	define B_DEFINE_WEAK_ALIAS(name, alias_name)	\
		extern "C" __typeof(name) alias_name __attribute__((weak, alias(#name)))
#else
#	define B_DEFINE_WEAK_ALIAS(name, alias_name)	\
		__typeof(name) alias_name __attribute__((weak, alias(#name)))
#endif


#endif	/* _BE_BUILD_H */
