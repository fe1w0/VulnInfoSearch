void get_cmdln_options(int argc, char *argv[]) {
	int o;
#if CONFIG_FILE && HAVE_GETPWUID
    static struct passwd *pwd_entry;
    char *str;
#endif
#ifdef LONG_OPTIONS
    int option_index = 0;
    static struct option long_options[] = {
        {"timeout", 1, 0, 't'},
#ifdef PROC_NET_DEV
        {"procfile",1,0,'f'},
#endif
#ifdef PROC_DISKSTATS
			{"diskstatsfile",1,0,1000},
			{"partitionsfile",1,0,1001},
#endif		  
#if NETSTAT && ALLOW_NETSTATPATH
        {"netstat",1,0,'n'},
#endif
#if IOSERVICE_IN
		  {"longdisknames",0,0,1002},
#endif
        {"input",1,0,'i'},
        {"dynamic",1,0,'d'},
        {"help", 0, 0, 'h'},
        {"version",0,0,'V'},
        {"allif",1,0,'a'},
        {"unit",1,0,'u'},
		  {"ansiout",0,0,'N'},
#if EXTENDED_STATS        
        {"type",1,0,'T'},
        {"avglength",1,0,'A'},
#endif        
        {"interfaces",1,0,'I'},
        {"sumhidden",1,0,'S'},
        {"output",1,0,'o'},
#ifdef CSV
        {"csvchar",1,0,'C'},
        {"csvfile",1,0,'F'},
#endif
        {"count",1,0,'c'},
        {"daemon",1,0,'D'},
#ifdef HTML
        {"htmlrefresh",1,0,'R'},
        {"htmlheader",1,0,'H'},
#endif
        {0,0,0,0}
    };
#endif
#ifdef CONFIG_FILE
    /* loop till first non option argument */
    opterr=0;
    while (1) {
#ifdef LONG_OPTIONS
        o=getopt_long (argc,argv,SHORT_OPTIONS,long_options, &option_index);
#else
        o=getopt (argc,argv,SHORT_OPTIONS);
#endif
        if (o==-1) break;
    }
    opterr=1;
    if (optind < argc) {
        read_config(argv[optind]);
    } else {
        read_config("/etc/bwm-ng.conf");
#ifdef HAVE_GETPWUID    
        pwd_entry=getpwuid(getuid());
        if (pwd_entry!=NULL) {
            str=(char*)malloc(strlen(pwd_entry->pw_dir)+14);
            snprintf(str,strlen(pwd_entry->pw_dir)+14,"%s/.bwm-ng.conf",pwd_entry->pw_dir);
            read_config(str);
            free(str);
        }
#endif    
    }
    /* reset getopt again  */
    optind=1;
#endif
    /* get command line arguments, kinda ugly, wanna rewrite it? */
    while (1) {
#ifdef LONG_OPTIONS
		o=getopt_long (argc,argv,SHORT_OPTIONS,long_options, &option_index);
#else
		o=getopt (argc,argv,SHORT_OPTIONS);
#endif		
        if (o==-1) break;
        switch (o) {
            case '?': printf("unknown option: %s\n",argv[optind-1]);
                      exit(EXIT_FAILURE);
                      break;
            /* ugly workaround to handle optional arguments for all platforms */                      
            case ':': if (!strcmp(argv[optind-1],"-a") || !strcasecmp(argv[optind-1],"--allif")) 
                            show_all_if=1;
                      else if (!strcmp(argv[optind-1],"-d") || !strcasecmp(argv[optind-1],"--dynamic"))
                            dynamic=1;
                      else if (!strcmp(argv[optind-1],"-D") || !strcasecmp(argv[optind-1],"--daemon"))
                            daemonize=1;
#ifdef HTML                      
                      else if (!strcmp(argv[optind-1],"-H") || !strcasecmp(argv[optind-1],"--htmlheader"))
                            html_header=1;
#endif                      
                      else if (!strcmp(argv[optind-1],"-S") || !strcasecmp(argv[optind-1],"--sumhidden"))
                            sumhidden=1;    
                          else {
                              printf("%s requires an argument!\n",argv[optind-1]);
                              exit(EXIT_FAILURE);
                          }
                      break;
#ifdef PROC_DISKSTATS
			case 1000:
				if (strlen(optarg)<PATH_MAX) 
					strcpy(PROC_DISKSTATS_FILE,optarg);
				break;
         case 1001:
            if (strlen(optarg)<PATH_MAX)
               strcpy(PROC_PARTITIONS_FILE,optarg);
				break;
#endif				
#if IOSERVICE_IN
			case 1002:
				long_darwin_disk_names=!long_darwin_disk_names;
				break;
#endif
			case 'D':
				if (optarg) daemonize=atoi(optarg);
				break;
#ifdef HTML
			case 'R':
				if ((optarg) && atol(optarg)>0) { html_refresh=atol(optarg); }
				break;
			case 'H':
				if (optarg) html_header=atoi(optarg);
				break;
#endif
			case 'c':
				if (optarg) output_count=atol(optarg);
				break;
#if CSV || HTML
            case 'F':
                if (optarg) { 
                    if (out_file) fclose(out_file);
                    out_file=fopen(optarg,"a"); 
                    if (!out_file) deinit(1, "failed to open outfile\n");
                    if (out_file_path) free(out_file_path);
                    out_file_path=(char *)strdup(optarg);
                }
                break;
#endif
#ifdef CSV
			case 'C':
				if (optarg) csv_char=optarg[0];
				break;
#endif
            case 'h':
                cmdln_printhelp();
                break;
#ifdef PROC_NET_DEV
			case 'f':
                if (optarg && (strlen(optarg)<PATH_MAX)) strcpy(PROC_FILE,optarg);
                break;
#endif			
			case 'i':
                if (optarg) {
                    input_method=str2in_method(optarg);
                }
				break;				
            case 'I':
                if (optarg) iface_list=(char *)strdup(optarg);
                break;
            case 'S':
                if (optarg) sumhidden=atoi(optarg);
                break;
            case 'o':
                if (optarg) {
                    output_method=str2out_method(optarg);
                }
                break;
				case 'N':
					 ansi_output=!ansi_output;
            case 'a':
                if (optarg) show_all_if=atoi(optarg);
                break;
            case 't':
                if ((optarg) && atol(optarg)>0) { delay=atol(optarg); }
                break;
#if EXTENDED_STATS                
            case 'T':
                output_type=str2output_type(optarg);
                break;
            case 'A':
                if (optarg) avg_length=atoi(optarg)*1000;
                break;
#endif                
            case 'd':
                if (optarg) dynamic=atoi(optarg);
                break;
            case 'u':
                output_unit=str2output_unit(optarg);
                break;
#if NETSTAT && ALLOW_NETSTATPATH
            case 'n':
                if (optarg && (strlen(optarg)<PATH_MAX)) strcpy(NETSTAT_FILE,optarg);
                break;
#endif                
            case 'V':
                print_version;
                exit(EXIT_SUCCESS);
                break;
        }
    }
    if (iface_list==NULL && show_all_if==1) show_all_if=2;
#if EXTENDED_STATS    
    /* default init of avg_length */
    if (avg_length==0) {
        if (delay<AVG_LENGTH/2) 
            avg_length=AVG_LENGTH; 
        else  
            avg_length=(delay*2)+1;
    } else /* avg_length was set via cmdline or config file, better check it */
        if (delay*2>=avg_length) deinit(1, "avglength needs to be a least twice the value of timeout\n");
#endif    
	 if ((output_unit==ERRORS_OUT && !net_input_method(input_method)) || 
			 (output_unit==PACKETS_OUT && input_method==LIBSTATDISK_IN)) 
		output_unit=BYTES_OUT;
    return;
}