/*
 * Copyright 2013 MongoDB Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#if !defined (MONGOC_INSIDE) && !defined (MONGOC_COMPILATION)
# error "Only <bson.h> can be included directly."
#endif


#ifndef MONGOC_THREAD_PRIVATE_H
#define MONGOC_THREAD_PRIVATE_H


#include <bson.h>

#include "mongoc-config.h"


/*
 * The following tries to abstract the native threading implementation for
 * the current system using macros.
 */


#if !defined(_WIN32)
#  include <pthread.h>
#  define MONGOC_MUTEX_INITIALIZER       PTHREAD_MUTEX_INITIALIZER
#  define mongoc_cond_t                  pthread_cond_t
#  define mongoc_cond_init(_n)           pthread_cond_init((_n), NULL)
#  define mongoc_cond_wait               pthread_cond_wait
#  define mongoc_cond_signal             pthread_cond_signal
#  define mongoc_cond_destroy            pthread_cond_destroy
#  define mongoc_mutex_t                 pthread_mutex_t
#  define mongoc_mutex_init(_n)          pthread_mutex_init((_n), NULL)
#  define mongoc_mutex_lock              pthread_mutex_lock
#  define mongoc_mutex_unlock            pthread_mutex_unlock
#  define mongoc_mutex_destroy           pthread_mutex_destroy
#  define mongoc_thread_t                pthread_t
#  define mongoc_thread_create(_t,_f,_d) pthread_create((_t), NULL, (_f), (_d))
#  define mongoc_thread_join(_n)         pthread_join((_n), NULL)
#  define mongoc_once_t                  pthread_once_t
#  define mongoc_once                    pthread_once
#  define MONGOC_ONCE_FUN(n)             void n(void)
#  define MONGOC_ONCE_RETURN             return

#  ifdef _PTHREAD_ONCE_INIT_NEEDS_BRACES
#    define MONGOC_ONCE_INIT       { PTHREAD_ONCE_INIT }
#  else
#    define MONGOC_ONCE_INIT       PTHREAD_ONCE_INIT
#  endif
#else
#  define mongoc_thread_t          HANDLE
static BSON_INLINE int mongoc_thread_create(mongoc_thread_t *thread, void *(*cb)(void *), void *arg)
{
   *thread = CreateThread(NULL, 0, (void *)cb, arg, 0, NULL);
   return 0;
}
#  define mongoc_thread_join(_n)            WaitForSingleObject((_n), INFINITE)
#  define mongoc_mutex_t                    CRITICAL_SECTION
#  define mongoc_mutex_init                 InitializeCriticalSection
#  define mongoc_mutex_lock                 EnterCriticalSection
#  define mongoc_mutex_unlock               LeaveCriticalSection
#  define mongoc_mutex_destroy              DeleteCriticalSection
#  define mongoc_cond_t                     CONDITION_VARIABLE
#  define mongoc_cond_init                  InitializeConditionVariable
#  define mongoc_cond_wait(_c, _m)          SleepConditionVariableCS((_c), (_m), INFINITE)
#  define mongoc_cond_signal                WakeConditionVariable
static BSON_INLINE int mongoc_cond_destroy(mongoc_cond_t * _ignored) { return 0; }
#  define mongoc_once_t                     INIT_ONCE
#  define MONGOC_ONCE_INIT                  INIT_ONCE_STATIC_INIT
#  define mongoc_once(o, c)                 InitOnceExecuteOnce(o, c, NULL, NULL)
#  define MONGOC_ONCE_FUN(n)                BOOL CALLBACK n(PINIT_ONCE _ignored_a, PVOID _ignored_b, PVOID *_ignored_c)
#  define MONGOC_ONCE_RETURN                return true
#endif


#endif /* MONGOC_THREAD_PRIVATE_H */
