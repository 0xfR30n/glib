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

} GThread3DS;

typedef struct
{
  gpointer key;
  gpointer value;
} GTls3DSPair;

#define TLS_OFFSET 0x20
#define MAX_PAIRS  32

typedef struct
{
  GTls3DSPair data[MAX_PAIRS];
  gsize length;
} GTls3DS;

static GTls3DS *
g_3ds_create_tls ()
{
  GTls3DS *tls = (GTls3DS *) malloc (sizeof (GTls3DS));
  memset (tls, 0, sizeof (GTls3DS));
  *(u32 *) ((u8 *) getThreadLocalStorage () + TLS_OFFSET) = tls;
}

static GTls3DS *
g_3ds_get_tls ()
{
  GTls3DS *tls = (GTls3DS *) ((u8 *) getThreadLocalStorage () + TLS_OFFSET);

  if (!tls)
    tls = g_3ds_create_tls ();

  return tls;
}

static void
g_3ds_delete_tls ()
{
  free (g_3ds_get_tls ());
}

static GTls3DSPair *
g_3ds_tls_get_pair (gpointer key)
{
  GTls3DS *tls = g_3ds_get_tls ();
  GTls3DSPair *pair = NULL;

  for (gsize i = 0; i < tls->length; i++) {
    if (tls->data[i].key == key) {
      pair = &tls->data[i];
      break;
    }
  }

  if (!pair) {
    if (tls->length < MAX_PAIRS) {
      pair = &tls->data[tls->length];
      pair->key = key;
      pair->value = NULL;
      tls->length += 1;
    } else {
      // NO space for more keys+values
      g_assert_not_reached ();
    }
  }
  // printf ("get_pair %p %p \n", GTls3DS3DS, pair);

  return pair;
}

static gpointer
g_3ds_tls_get (gpointer key)
{
  GTls3DSPair *pair = g_3ds_tls_get_pair (key);
  return pair->value;
}

static void
g_3ds_tls_set (gpointer key, gpointer value)
{
  GTls3DSPair *pair = g_3ds_tls_get_pair (key);
  pair->value = value;
}

static void
g_3ds_thread_proxy_func (void *arg)
{
  GThread3DS *thread = (GThread3DS *) arg;

  g_3ds_create_tls ();
  thread->proxy (thread);
  g_3ds_delete_tls ();

  g_system_thread_exit ();
}

void
g_system_thread_wait (GRealThread * thread)
{
  GThread3DS *t = (GThread3DS *) thread;
  threadJoin (t->thread, U64_MAX);
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

  base_thread = (GRealThread *) thread;
  base_thread->ref_count = 2;
  base_thread->ours = TRUE;
  base_thread->thread.joinable = TRUE;
  base_thread->thread.func = func;
  base_thread->thread.data = data;
  base_thread->name = g_strdup (name);

  svcGetThreadPriority (&prio, CUR_THREAD_HANDLE);

  thread->thread =
      threadCreate (g_3ds_thread_proxy_func, thread, 4 * 1024, prio - 1, -2,
      false);

  return thread;
}

void
g_system_thread_free (GRealThread * thread)
{
  GThread3DS *t = (GThread3DS *) thread;
  threadDetach (t->thread);
  g_slice_free (GThread3DS, t);
}

void
g_system_thread_exit (void)
{
  threadExit (0);
}

void
g_thread_yield (void)
{
  svcSleepThread (1);
}

gpointer
g_private_get (GPrivate * key)
{
  return g_3ds_tls_get (key);
}

void
g_private_set (GPrivate * key, gpointer value)
{
  g_3ds_tls_set (key, value);
}

void
g_private_replace (GPrivate * key, gpointer value)
{
  gpointer old = g_3ds_tls_get (key);

  g_3ds_tls_set (key, value);

  if (old && key->notify)
    key->notify (old);
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
