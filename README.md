# ios-inject-custom

Example showing how to use Frida for standalone injection of a custom
payload. The payload is a .dylib that uses Gum, Frida's low-level
instrumentation library, to hook `open()` and print the arguments on
`stderr` every time it's called. The payload could be any shared library
as long as it exports a function with the name that you specify when
calling `inject_library_file_sync()`. In our example we named it
`example_agent_main`. This function will also be passed a string of
data, which you can use for application-specific purposes.

Note that only the build system is iOS-specific, so this example is
easily portable to all other OSes supported by Frida.

# Prerequisites

- Xcode
- Jailbroken iOS device

# Running

```sh
$ make
```

This will build the injector, the payload, and an example program you
can inject the payload into to easily observe the results.

Next copy the `bin/` directory somewhere on your iOS device, and in one
terminal SSH to your device and launch the `victim` binary:

```sh
$ ./victim
Victim running with PID 1303
```

Then in another terminal change directory to where the `inject` binary
is and run it:

```sh
$ ./inject 1303
$
```

You should now see a message printed by the `victim` process every time
`open()` is called.
