#include "config.h"

#include "glib.h"

GSourceFuncs g_io_watch_funcs = {
  NULL,                         // prepare
  NULL,                         // check
  NULL,                         //dispatch
  NULL,                         // finalize
};

GIOChannel *
g_io_channel_new_file (const gchar * filename,
    const gchar * mode, GError ** error)
{
  return NULL;
}

GIOChannel *
g_io_channel_unix_new (gint fd)
{
  return NULL;
}
