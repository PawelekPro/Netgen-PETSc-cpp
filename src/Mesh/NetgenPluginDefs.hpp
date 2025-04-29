#ifndef NETGENPLUGIN_DEFS_HPP
#define NETGENPLUGIN_DEFS_HPP

#ifdef WIN32
#if defined NETGENPLUGIN_EXPORTS
#define NETGENPLUGIN_EXPORT __declspec(dllexport)
#else
#define NETGENPLUGIN_EXPORT __declspec(dllimport)
#endif

#if defined(NETGEN_V6)
#define NETGENPLUGIN_DLL_HEADER DLL_HEADER
#endif

#else

#define NETGENPLUGIN_EXPORT
#define NETGENPLUGIN_DLL_HEADER

#endif

#endif
