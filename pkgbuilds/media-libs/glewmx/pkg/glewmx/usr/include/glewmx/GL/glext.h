/* glewmx glext.h wrapper: suppress GL_SGIX_fragment_lighting typedefs
 * that GLEW 1.13 glew.h already defines, to avoid conflicting declaration
 * errors with modern xorgproto glext.h when building with -I.../glewmx */
#ifndef __GLEWMX_GLEXT_WRAPPER_H__
#define __GLEWMX_GLEXT_WRAPPER_H__

/* Pre-define the SGIX_fragment_lighting guard so system glext.h skips it */
#ifdef __glew_h__
#  ifndef GL_SGIX_fragment_lighting
#    define GL_SGIX_fragment_lighting 1
#  endif
#endif

/* Include the real system glext.h (search system paths, not our prefix) */
#include "/usr/include/GL/glext.h"

#endif /* __GLEWMX_GLEXT_WRAPPER_H__ */
