#include "config.h"

#include "gcontenttype.h"


gboolean
g_content_type_equals (const gchar * type1, const gchar * type2)
{
  return FALSE;
}

gboolean
g_content_type_is_a (const gchar * type, const gchar * supertype)
{
  return FALSE;
}

gboolean
g_content_type_is_mime_type (const gchar * type, const gchar * mime_type)
{
  return FALSE;
}

gboolean
g_content_type_is_unknown (const gchar * type)
{
  return FALSE;
}

gchar *
g_content_type_get_description (const gchar * type)
{
  return NULL;
}

gchar *
g_content_type_get_mime_type (const gchar * type)
{
  return NULL;
}

GIcon *
g_content_type_get_icon (const gchar * type)
{
  return NULL;
}

GIcon *
g_content_type_get_symbolic_icon (const gchar * type)
{
  return NULL;
}

gchar *
g_content_type_get_generic_icon_name (const gchar * type)
{
  return NULL;
}

gboolean
g_content_type_can_be_executable (const gchar * type)
{
  return FALSE;
}

gchar *
g_content_type_from_mime_type (const gchar * mime_type)
{
  return NULL;
}


gchar *
g_content_type_guess (const gchar * filename,
    const guchar * data, gsize data_size, gboolean * result_uncertain)
{
  return NULL;
}


gchar **
g_content_type_guess_for_tree (GFile * root)
{
  return NULL;
}


GList *
g_content_types_get_registered (void)
{
  return NULL;
}

void
g_content_type_set_mime_dirs (const gchar * const *dirs)
{
}
