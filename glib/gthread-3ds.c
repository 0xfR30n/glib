#include "config.h"

#include "glib.h"
#include "gslice.h"
#include "gthread.h"
#include "gthreadprivate.h"

#include <3ds.h>

typedef struct
{
  GRealThread base_thread;

  GThreadFunc proxy;
  Thread thread;

  gboolean joined;

} GThread3DS;

static void
_3ds_thread_proxy_func (void *arg)
{
  GThread3DS *thread = (GThread3DS *) arg;
  thread->proxy (thread);
  g_system_thread_exit ();
  g_assert_not_reached ();
}

void
g_system_thread_wait (GRealThread * thread)
{
  GThread3DS *t = (GThread3DS *) thread;

  if (!t->joined) {
    threadJoin (t->thread, U64_MAX);
    t->joined = TRUE;
  }
}

GRealThread *
g_system_thread_new (GThreadFunc proxy,
    gulong stack_size,
    const GThreadSchedulerSettings * scheduler_settings,
    const char *name, GThreadFunc func, gpointer data, GError ** error)
{
  GThread3DS *thread;
  GRealThread *base_thread;
  s32 prio = 0;

  thread = g_slice_new0 (GThread3DS);
  thread->proxy = proxy;
  thread->joined = FALSE;

  base_thread = (GRealThread *) thread;
  base_thread->ref_count = 2;
  base_thread->ours = TRUE;
  base_thread->thread.joinable = TRUE;
  base_thread->thread.func = func;
  base_thread->thread.data = data;
  base_thread->name = g_strdup (name);

  svcGetThreadPriority (&prio, CUR_THREAD_HANDLE);

  thread->thread =
      threadCreate (_3ds_thread_proxy_func, thread, 4 * 1024, prio - 1, -2,
      false);

  return thread;
}

void
g_system_thread_free (GRealThread * thread)
{
  GThread3DS *t = (GThread3DS *) thread;

  if (!t->joined)
    threadDetach (t->thread);

  g_slice_free (GThread3DS, t);
}

void
g_system_thread_exit (void)
{
  threadExit (0);
}

void
g_system_thread_set_name (const gchar * name)
{
}

gboolean
g_system_thread_get_scheduler_settings (GThreadSchedulerSettings *
    scheduler_settings)
{
}

static LightLock *
g_mutex_impl_new ()
{
  LightLock *lock;

  lock = malloc (sizeof (LightLock));
  LightLock_Init (lock);

  return lock;
}

static void
g_mutex_impl_free (LightLock * lock)
{
  free (lock);
}

static inline LightLock *
g_mutex_get_impl (GMutex * mutex)
{
  LightLock *impl = mutex->p;

  if (G_UNLIKELY (impl == NULL)) {
    impl = g_mutex_impl_new ();
    if (!g_atomic_pointer_compare_and_exchange (&mutex->p, NULL, impl))
      g_mutex_impl_free (impl);
    impl = mutex->p;
  }

  return impl;
}

void
g_mutex_init (GMutex * mutex)
{
  mutex->p = g_mutex_impl_new ();
}

void
g_mutex_clear (GMutex * mutex)
{
  g_mutex_impl_free (mutex->p);
}

void
g_mutex_lock (GMutex * mutex)
{
  LightLock_Lock (g_mutex_get_impl (mutex));
}

gboolean
g_mutex_trylock (GMutex * mutex)
{
  if (LightLock_TryLock (g_mutex_get_impl (mutex)) == 0)
    return TRUE;

  return FALSE;
}

void
g_mutex_unlock (GMutex * mutex)
{
  LightLock_Unlock (g_mutex_get_impl (mutex));
}

void
g_rw_lock_init (GRWLock * rw_lock)
{
}

void
g_rw_lock_clear (GRWLock * rw_lock)
{
}

void
g_rw_lock_writer_lock (GRWLock * rw_lock)
{
}

gboolean
g_rw_lock_writer_trylock (GRWLock * rw_lock)
{
  return FALSE;
}

void
g_rw_lock_writer_unlock (GRWLock * rw_lock)
{
}

void
g_rw_lock_reader_lock (GRWLock * rw_lock)
{
}

gboolean
g_rw_lock_reader_trylock (GRWLock * rw_lock)
{
  return FALSE;
}

void
g_rw_lock_reader_unlock (GRWLock * rw_lock)
{
}

static RecursiveLock *
g_rec_mutex_impl_new ()
{
  RecursiveLock *lock;

  lock = malloc (sizeof (RecursiveLock));
  RecursiveLock_Init (lock);

  return lock;
}

static void
g_rec_mutex_impl_free (RecursiveLock * lock)
{
  free (lock);
}

static inline RecursiveLock *
g_rec_mutex_get_impl (GRecMutex * rec_mutex)
{
  RecursiveLock *impl = rec_mutex->p;

  if (G_UNLIKELY (impl == NULL)) {
    impl = g_rec_mutex_impl_new ();
    if (!g_atomic_pointer_compare_and_exchange (&rec_mutex->p, NULL, impl))
      g_rec_mutex_impl_free (impl);
    impl = rec_mutex->p;
  }

  return impl;
}

void
g_rec_mutex_init (GRecMutex * rec_mutex)
{
  rec_mutex->p = g_rec_mutex_impl_new ();
}

void
g_rec_mutex_clear (GRecMutex * rec_mutex)
{
  g_mutex_impl_free (rec_mutex->p);
}

void
g_rec_mutex_lock (GRecMutex * rec_mutex)
{
  RecursiveLock_Lock (g_rec_mutex_get_impl (rec_mutex));
}

gboolean
g_rec_mutex_trylock (GRecMutex * rec_mutex)
{
  if (RecursiveLock_TryLock (g_rec_mutex_get_impl (rec_mutex)) == 0)
    return TRUE;

  return FALSE;
}

void
g_rec_mutex_unlock (GRecMutex * rec_mutex)
{
  RecursiveLock_Unlock (g_rec_mutex_get_impl (rec_mutex));
}

static CondVar *
g_cond_impl_new ()
{
  CondVar *cond;

  cond = malloc (sizeof (CondVar));
  CondVar_Init (cond);

  return cond;
}

static void
g_cond_impl_free (CondVar * cond)
{
  free (cond);
}

static CondVar *
g_cond_get_impl (GCond * cond)
{
  CondVar *impl = cond->p;

  if (G_UNLIKELY (impl == NULL)) {
    impl = g_mutex_impl_new ();
    if (!g_atomic_pointer_compare_and_exchange (&cond->p, NULL, impl))
      g_cond_impl_free (impl);
    impl = cond->p;
  }

  return impl;
}

void
g_cond_init (GCond * cond)
{
  cond->p = g_cond_impl_new ();
}

void
g_cond_clear (GCond * cond)
{
  g_cond_impl_free (cond->p);
}

void
g_cond_wait (GCond * cond, GMutex * mutex)
{
  CondVar_Wait (g_cond_get_impl (cond), g_mutex_get_impl (mutex));
}

void
g_cond_signal (GCond * cond)
{
  CondVar_Signal (g_cond_get_impl (cond));
}

void
g_cond_broadcast (GCond * cond)
{
  CondVar_Broadcast (g_cond_get_impl (cond));
}

gboolean
g_cond_wait_until (GCond * cond, GMutex * mutex, gint64 end_time)
{
  int ret;

  ret =
      CondVar_WaitTimeout (g_cond_get_impl (cond), g_mutex_get_impl (mutex),
      end_time);

  if (ret == 0)
    return TRUE;

  return FALSE;
}


#include <stdio.h>

typedef struct
{
  guint32 idx;
  gpointer data;
} GPrivate3DS;

static GPrivate3DS *
g_private_3ds_get (GPrivate * key)
{
  GPrivate3DS *priv = key->p;

  if (G_UNLIKELY (priv == NULL)) {
    u32 i;
    u32 *tlsbuf = (u32 *) getThreadStaticBuffers ();

    for (i = 0; i < 32; i++) {
      if (tlsbuf[i] == NULL)
        break;
    }

    if (i == 32) {
      // no more space !
      printf ("NO MORE SPACE?!\n");
      return NULL;
      // g_abort ();
    }

    priv = malloc (sizeof (GPrivate3DS));
    priv->idx = i;
    priv->data = NULL;

    tlsbuf[i] = (u32 *) priv;
    key->p = (gpointer) priv;
  }

  return priv;
}

static void
g_private_3ds_free (GPrivate3DS * priv)
{
  if (G_UNLIKELY (priv == NULL))
    return;

  u32 *tlsbuf = (u32 *) getThreadLocalStorage ();
  g_assert (priv->idx >= START_IDX && priv->idx < END_IDX);
  tlsbuf[priv->idx] = NULL;
  free (priv);
}

gpointer
g_private_get (GPrivate * key)
{
  (void) g_private_3ds_get;
  (void) g_private_3ds_free;

  GPrivate3DS *priv = g_private_3ds_get (key);
  return priv->data;
  // return NULL;
}

void
g_private_set (GPrivate * key, gpointer value)
{
  GPrivate3DS *priv = g_private_3ds_get (key);
  priv->data = value;
}

void
g_private_replace (GPrivate * key, gpointer value)
{
  GPrivate3DS *priv = g_private_3ds_get (key);
  gpointer old;

  old = priv->data;
  priv->data = value;

  if (old && key->notify)
    key->notify (old);
}

void
g_thread_yield (void)
{
  svcSleepThread (1);
}
