#define __FILENAME__ "cfg.c"
#include "cfg.h"

#include <config.h>


#include <string.h>
#include <stdio.h>

#define clz(x) __builtin_clz(x)
#define ctz(x) __builtin_ctz(x)

struct Config config;


size_t parse_num(char *c){
	size_t num = 0;
	while(*c){
		switch(*c){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				num = num * 10 + (*c++ - '0');
				break;
			case 'G':
			case 'g':
				num *= 1024;
			case 'M':
			case 'm':
				num *= 1024;
			case 'K':
			case 'k':
				num *= 1024;
				c++;
				goto look_for_null;
			default:
				return 0;
				break;
		}
	}
	look_for_null:
	if( *c == '\0' ) return num;
	return 0;

}


void cfg_init( int argc, char **argv ){

	// Set defaults and stuff
	config.chunk_size = 6;
	config.world_size[0] = 5;
	config.world_size[1] = 2;
	config.world_size[2] = 5;
	config.opengl_compat = 0;
	config.opengl_debug = 0;

	uint8_t custom_world = 0;

	config.heap = parse_num( "1G" );


	// THIS IS SO UGLY
	for (int i = 1; i < argc; ++i)
	{
		char *arg = argv[i];
		if(
			strcmp( arg, "--opengl_compat" ) == 0
		){

			config.opengl_compat = 1;

		} else if ( 
			strcmp( arg, "--opengl_debug" ) == 0
		){

			config.opengl_debug = 1;

		} else if (
			strcmp( arg, "--version" ) == 0
		){

			printf( "%s %s.%s\n", PROJECT_NAME,VERSION_MAJOR,VERSION_MINOR);
			exit(0);

		} else if (
			strcmp( arg, "--chunk_size" ) == 0
		){

			if( i+1 >= argc ){
				printf("Not enough arguments\n");
				exit(1);
			}

			if(custom_world){
				printf("Please, set chunk_size before world_size\n");
				exit(1);
			}

			config.chunk_size = ctz( parse_num(argv[++i]) );


		} else if (
			strcmp( arg, "--world_size" ) == 0
		){

			if( i+3 >= argc ){ 
				printf("Not enough arguments\n");
				exit(1);
			}

			uint32_t x = parse_num(argv[++i]) / (1<<config.chunk_size);
			uint32_t y = parse_num(argv[++i]) / (1<<config.chunk_size);
			uint32_t z = parse_num(argv[++i]) / (1<<config.chunk_size);

			config.world_size[0] = ctz(x);
			config.world_size[1] = ctz(y);
			config.world_size[2] = ctz(z);

			custom_world++;

		} else if (
			strcmp( arg, "--help" ) == 0
		){

			printf("--opengl_compat\n");
			printf("--version\n");
			printf("--help\n");
			exit(0);

		} else if (
			strcmp( arg, "--heap" ) == 0
		){
			printf("Setting heap size\n");

			i++;
			if( i >= argc ){ 
				printf("Usage: --heap <number followed by K, M or G>\nExample: --heap 512M\n");
				exit(1);
			}

			config.heap = parse_num( argv[i] );

			if( config.heap == 0 ) {
				printf("Bad heap size\n");
				exit(1);
			}

		} else {

			printf("Bad option %s\n", arg);
			printf("See --help\n");
			exit(1);

		}


	}


}


struct Config *cfg_get(){
	return &config;
}
