#include "config.h"

#include "gappinfo.h"
#include "gioerror.h"
#include "gfile.h"


GAppInfo *
g_app_info_create_from_commandline (const char *commandline,
    const char *application_name, GAppInfoCreateFlags flags, GError ** error)
{
  return NULL;
}

GList *
g_app_info_get_all (void)
{
  return NULL;
}

GList *
g_app_info_get_all_for_type (const char *content_type)
{
  return NULL;
}

GList *
g_app_info_get_recommended_for_type (const gchar * content_type)
{
  return NULL;
}

GList *
g_app_info_get_fallback_for_type (const gchar * content_type)
{
  return NULL;
}

void
g_app_info_reset_type_associations (const char *content_type)
{
}

GAppInfo *
g_app_info_get_default_for_type (const char *content_type,
    gboolean must_support_uris)
{
  return NULL;
}

GAppInfo *
g_app_info_get_default_for_uri_scheme (const char *uri_scheme)
{
  return NULL;
}
