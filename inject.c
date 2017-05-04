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

  if (argc != 2)
    goto bad_usage;

  pid = atoi (argv[1]);
  if (pid <= 0)
    goto bad_usage;

  frida_init ();

  injector = frida_injector_new ();

  error = NULL;
  id = frida_injector_inject_library_file_sync (injector, pid, "./agent.dylib", "example_agent_main", "example data", &error);
  if (error != NULL)
  {
    fprintf (stderr, "%s\n", error->message);
    g_error_free (error);

    result = 1;
  }

  frida_injector_close_sync (injector);
  g_object_unref (injector);

  frida_deinit ();

  return result;

bad_usage:
  {
    fprintf (stderr, "Usage: %s <pid>\n", argv[0]);
    return 1;
  }
}
