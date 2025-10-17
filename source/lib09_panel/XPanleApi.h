#ifdef  LIBPANEL09_XPANEL
#define lib09_XPANEL_API __declspec(dllexport)
#else
#define lib09_XPANEL_API __declspec(dllimport)
#endif 