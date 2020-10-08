#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* Flag set by ‘--verbose’. */
static int verbose_flag;

int main (int argc, char **argv)
{
    int c;
    static int is_local;

    while (1)
    {
        static struct option long_options[] =
        {
            {"local",  no_argument, &is_local, 1},
            {"addr",   required_argument, 0, 'i'},
            {"port",   required_argument, 0, 'p'},
            {"file",   required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "li:p:f:",
                        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
        break;

        switch (c)
        {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
            break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
            printf (" with arg %s", optarg);
            printf ("\n");
            break;
        case 'i':
            printf ("option -i with value `%s'\n", optarg);
            break;
        case 'p':
            printf ("option -p with value `%s'\n", optarg);
            break;
        case 'f':
            printf ("option -f with value `%s'\n", optarg);
            break;
        case '?':
            /* getopt_long already printed an error message. */
            break;

        default:
            abort ();
        }
    }
    
    if (is_local)
        puts ("verbose flag is set");
    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        
        putchar ('\n');
    }

    exit (0);
}