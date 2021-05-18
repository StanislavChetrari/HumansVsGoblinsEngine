#ifndef _ENGINE_API_H_
#define _ENGINE_API_H_

#if defined(_WIN32)
	#if defined(_H_VS_G_SHARED_LIB)
		#if defined(_H_VS_G_EXPORT_API)
			#define DECL_API __declspec(dllexport)
		#else 
			#define DECL_API __declspec(dllimport)
		#endif	
	#else
		#define DECL_API 
	#endif
#else 
	#define DECL_API 
#endif

#endif /* #ifndef _ENGINE_API_H_ */