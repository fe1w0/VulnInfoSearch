static int
call_backend(char *uri,                 // I - URI of final destination
	     int  argc,                 // I - Number of command line
	                                //     arguments
	     char **argv,		// I - Command-line arguments
	     char *filename)            // I - File name of input data
{
  const char	*cups_serverbin;	// Location of programs
  char		scheme[1024],           // Scheme from URI
                *ptr,			// Pointer into scheme
		cmdline[65536];		// Backend command line
  int           retval;

  //
  // Build the backend command line...
  //

  strncpy(scheme, uri, sizeof(scheme) - 1);
  if (strlen(uri) > 1023)
    scheme[1023] = '\0';
  if ((ptr = strchr(scheme, ':')) != NULL)
    *ptr = '\0';

  if ((cups_serverbin = getenv("CUPS_SERVERBIN")) == NULL)
    cups_serverbin = CUPS_SERVERBIN;

  if (!strncasecmp(uri, "file:", 5) || uri[0] == '/')
  {
    fprintf(stderr,
	    "ERROR: beh: Direct output into a file not supported.\n");
    exit (CUPS_BACKEND_FAILED);
  }
  else
    snprintf(cmdline, sizeof(cmdline),
	     "%s/backend/%s '%s' '%s' '%s' '%s' '%s' %s",
	     cups_serverbin, scheme, argv[1], argv[2], argv[3],
	     // Apply number of copies only if beh was called with a
	     // file name and not with the print data in stdin, as
	     // backends should handle copies only if they are called
	     // with a file name
	     (argc == 6 ? "1" : argv[4]),
	     argv[5], filename);

  //
  // Overwrite the device URI and run the actual backend...
  //

  setenv("DEVICE_URI", uri, 1);

  fprintf(stderr,
	  "DEBUG: beh: Executing backend command line \"%s\"...\n",
	  cmdline);
  fprintf(stderr,
	  "DEBUG: beh: Using device URI: %s\n",
	  uri);

  retval = system(cmdline) >> 8;

  if (retval == -1)
    fprintf(stderr, "ERROR: Unable to execute backend command line: %s\n",
	    strerror(errno));

  return (retval);
}