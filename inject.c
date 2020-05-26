#include <frida-core.h>
#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char * argv[])
{
  int result = 0;
  FridaInjector * injector;
  int pid;
  GError * error;
  guint id;

  frida_init ();

  if (argc != 2)
    goto bad_usage;

  pid = atoi (argv[1]);
  if (pid <= 0)
    goto bad_usage;

  /*
   * Note that we use Frida's injector in inprocess mode, which is why
   * we need the `task_for_pid-allow` entitlement. If you're embedding
   * Frida as a plugin of an existing application this may not be an
   * option, so in that case use `frida_injector_new()` here instead,
   * which will spawn a helper process with the required entitlement.
   */
  injector = frida_injector_new_inprocess ();

  error = NULL;
  id = frida_injector_inject_library_file_sync (injector, pid, "./agent.dylib", "example_agent_main", "example data", NULL, &error);
  if (error != NULL)
  {
    fprintf (stderr, "%s\n", error->message);
    g_error_free (error);

    result = 1;
  }

  frida_injector_close_sync (injector, NULL, NULL);
  g_object_unref (injector);

  frida_deinit ();

  return result;

bad_usage:
  {
    g_printerr ("Usage: %s <pid>\n", argv[0]);
    frida_deinit ();
    return 1;
  }
}
