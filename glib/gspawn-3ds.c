#include "config.h"

#include "glib.h"
#include "gspawn.h"



GQuark
g_spawn_error_quark (void)
{
  return NULL;
}

GQuark
g_spawn_exit_error_quark (void)
{
  return NULL;
}


gboolean
g_spawn_async (const gchar * working_directory,
    gchar ** argv,
    gchar ** envp,
    GSpawnFlags flags,
    GSpawnChildSetupFunc child_setup,
    gpointer user_data, GPid * child_pid, GError ** error)
{
  return FALSE;
}


gboolean
g_spawn_async_with_pipes (const gchar * working_directory,
    gchar ** argv,
    gchar ** envp,
    GSpawnFlags flags,
    GSpawnChildSetupFunc child_setup,
    gpointer user_data,
    GPid * child_pid,
    gint * standard_input,
    gint * standard_output, gint * standard_error, GError ** error)
{
  return FALSE;
}


gboolean
g_spawn_async_with_pipes_and_fds (const gchar * working_directory,
    const gchar * const *argv,
    const gchar * const *envp,
    GSpawnFlags flags,
    GSpawnChildSetupFunc child_setup,
    gpointer user_data,
    gint stdin_fd,
    gint stdout_fd,
    gint stderr_fd,
    const gint * source_fds,
    const gint * target_fds,
    gsize n_fds,
    GPid * child_pid_out,
    gint * stdin_pipe_out,
    gint * stdout_pipe_out, gint * stderr_pipe_out, GError ** error)
{
  return FALSE;
}

gboolean
g_spawn_async_with_fds (const gchar * working_directory,
    gchar ** argv,
    gchar ** envp,
    GSpawnFlags flags,
    GSpawnChildSetupFunc child_setup,
    gpointer user_data,
    GPid * child_pid,
    gint stdin_fd, gint stdout_fd, gint stderr_fd, GError ** error)
{
  return FALSE;
}


gboolean
g_spawn_sync (const gchar * working_directory,
    gchar ** argv,
    gchar ** envp,
    GSpawnFlags flags,
    GSpawnChildSetupFunc child_setup,
    gpointer user_data,
    gchar ** standard_output,
    gchar ** standard_error, gint * exit_status, GError ** error)
{
  return FALSE;
}


gboolean
g_spawn_command_line_sync (const gchar * command_line,
    gchar ** standard_output,
    gchar ** standard_error, gint * exit_status, GError ** error)
{
  return FALSE;
}

gboolean
g_spawn_command_line_async (const gchar * command_line, GError ** error)
{
  return FALSE;
}

gboolean
g_spawn_check_exit_status (gint exit_status, GError ** error)
{
  return FALSE;
}


void
g_spawn_close_pid (GPid pid)
{
}
