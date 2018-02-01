#include <fcntl.h>
#include <frida-gum.h>

static int replacement_open (const char * path, int oflag, ...);

void
example_agent_main (const gchar * data, gboolean * stay_resident)
{
  GumInterceptor * interceptor;

  /* We don't want to our library to be unloaded after we return. */
  *stay_resident = TRUE;

  gum_init_embedded ();

  g_printerr ("example_agent_main()\n");

  interceptor = gum_interceptor_obtain ();

  /* Transactions are optional but improve performance with multiple hooks. */
  gum_interceptor_begin_transaction (interceptor);

  gum_interceptor_replace_function (interceptor,
      (gpointer) gum_module_find_export_by_name (NULL, "open"), replacement_open, NULL);
  /*
   * ^
   * |
   * This is using replace_function(), but there's also attach_listener() which
   * can be used to hook functions without any knowledge of argument types,
   * calling convention, etc. It can even be used to put a probe in the middle
   * of a function.
   */

  gum_interceptor_end_transaction (interceptor);
}

static int
replacement_open (const char * path, int oflag, ...)
{
  g_printerr ("open(\"%s\", 0x%x)\n", path, oflag);

  return open (path, oflag);
}
