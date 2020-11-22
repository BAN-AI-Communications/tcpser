#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define VER_MAJOR         1
#define VER_MINOR         1
#define VER_PATCH         4
#define VER_FIX           _vicekludge

#ifdef VER_FIX
  #define VERSION           TOSTRING(VER_MAJOR) "." TOSTRING(VER_MINOR) "." TOSTRING(VER_PATCH)TOSTRING(VER_FIX)
#else
  #define VERSION           TOSTRING(VER_MAJOR) "." TOSTRING(VER_MINOR) "." TOSTRING(VER_PATCH)
#endif

