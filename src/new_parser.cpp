/**************************************************************************
*   alsoft-conf, a configuration utility for OpenAL-soft.
*   Copyright (C) 2008-2009 Matias D'Ambrosio
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License along
*   with this program; if not, write to the Free Software Foundation, Inc.,
*   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**************************************************************************/


#include "new_parser.h"

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>

// Structures used throughout

struct _NPDict;
struct _NPSection;
struct _NPKey;

struct _NPDict{
	char *filename;
	struct _NPSection *sections;
};

struct _NPSection{
	struct _NPSection *next;

	char *name;
	struct _NPKey *keys;
};

struct _NPKey{
	struct _NPKey *next;

	char *name;
	char *value;
};

// Helper functions

static void _np_add_key( struct _NPKey **first_key, const char *key, const char *value)
{
	struct _NPKey *new_key = *first_key;
	if( new_key == NULL){
		new_key = (struct _NPKey *) calloc( 1, sizeof( struct _NPKey));
		*first_key = new_key;
	}
	else{
		while( new_key->next){
			new_key = new_key->next;
		}
		new_key->next = (struct _NPKey *) calloc( 1, sizeof( struct _NPKey));
		new_key = new_key->next;
	}
	new_key->name = strdup( key);
	new_key->value = strdup( value);
}


// Public functions

int np_load_dictionary( const char *filename, NPDict *dict)
{
	if( filename == NULL || dict == NULL){
		return -1;
	}

	FILE *fp;
	fp = fopen( filename, "rt");
	if( fp == NULL){
		return -1;
	}

	struct _NPDict *new_dict = (struct _NPDict *) calloc( 1, sizeof( _NPDict));
	new_dict->filename = strdup( filename);
	new_dict->sections = (struct _NPSection *) calloc( 1, sizeof( _NPSection));
	new_dict->sections->name = strdup( "general");

	ssize_t read_count = 0;
	size_t len = 0;
	char *line = NULL;
	int iter = 0;
	int tmp = 0;

	struct _NPSection *curr_section = new_dict->sections;
	struct _NPSection *iter_section = NULL;
	struct _NPKey *iter_key = NULL;

	while( (read_count = getline( &line, &len, fp)) != -1){
		iter = tmp = 0;
		// Remove leading spaces and tabs (makes it easier to find other stuff)
		while( isblank( line[ iter])){
			iter++;
		}

		// Ignore empty lines and comments
		if( line[ iter] == '\n' || line[ iter] == '#'){
			continue;
		}

		// New section found
		if( line[ iter] == '['){
			iter++;
			tmp = iter;
			while( isalpha( line[ tmp])){
				tmp++;
			}
			if( line[ tmp] != ']'){
				fprintf( stderr, "Error parsing filename %s\n", filename);
				continue;
			}

			// Check if it exists already
			char *section_name = strndup( (line + iter), (tmp - iter));
			iter_section = new_dict->sections;
			do{
				// if it exists, set as active and continue reading file
				if( strcmp( section_name, iter_section->name) == 0){
					curr_section = iter_section;
					free( section_name);
					section_name = NULL;
					break;
				}
			} while( (iter_section = iter_section->next));
			if( section_name == NULL){
				continue;
			}

			// Create new section, set name and set as active
			iter_section = curr_section;
			while( iter_section->next){
				iter_section = iter_section->next;
			}
			iter_section->next = (struct _NPSection *) calloc( 1, sizeof( _NPSection));
			curr_section = iter_section->next;
			curr_section->name = section_name;
			continue;
		}

		// This is not a new section, empty line or full line comment,
		// so attempt to find a key=value pair

		// If there is no equal sign, then there is no pair.

		if( strstr( line, "=") == NULL){
			continue;
		}

		// Find key name length
		tmp = 0;
		while( !isblank( line[ iter + tmp])
		&& line[ iter + tmp] != '='
		&& line[ iter + tmp] != '#'){
			tmp++;
		}
		if( line[ iter + tmp] == '#'){
			continue;
		}

		char *key = strndup( (line + iter), tmp);

		// Get past the spaces, equal sign and leading spaces
		iter += tmp;
		tmp = 0;
		while( (line[ iter + tmp] == '='
		|| isblank( line[ iter + tmp]))
		&& line[ iter + tmp] != '#'){
			tmp++;
		}
		if( line[ iter + tmp] == '#'){
			free( key);
			continue;
		}
		//tmp++;

/*
		// Ignore leading whitespace for the value
		while( isblank( line[ iter + tmp])){
			tmp++;
			if( line + iter + tmp == NULL){
				free( key); key = NULL;
				continue;
			}
		}
*/

		iter += tmp;
		tmp = 0;

		// Ignore trailing whitespace or comment at the end
		while( !isspace( line[ iter + tmp]) && line[ iter + tmp] != '#'){
			tmp++;
			if( line + iter + tmp == NULL){
				break;
			}
		}
		if( line + iter + tmp == NULL){
			free( key);
			continue;
		}

		char *value = strndup( (line + iter), tmp);

		// Now place the key=value pair in the dictionary structure
		iter_key = curr_section->keys;
		// Check if the key already exists
		while( iter_key != NULL){
			if( strcmp( iter_key->name, key) == 0){
				free( key);
				key = NULL;
				free( iter_key->value);
				iter_key->value = value;
				value = NULL;
				break;
			}
			iter_key = iter_key->next;
		}
		if( key == NULL){
			continue;
		}
		// It doesn't exist, create it.
		if( iter_key == NULL){
			iter_key = curr_section->keys;
			if( iter_key == NULL){
				iter_key = (struct _NPKey *) calloc( 1, sizeof( struct _NPKey *));
				curr_section->keys = iter_key;
			}
			else{
				while( iter_key->next){
					iter_key = iter_key->next;
				}
				iter_key->next = (struct _NPKey *) calloc( 1, sizeof( struct _NPKey));
				iter_key = iter_key->next;
			}
			iter_key->name = key;
			iter_key->value = value;
			continue;
		}
		free( key);
		free( value);
	}
	if( line){
		free( line);
	}

	fclose( fp);

	*dict = new_dict;
	return 0;
}


void np_destroy_dictionary( NPDict *dict)
{
	if( dict == NULL){
		return;
	}
	struct _NPDict *d = *((struct _NPDict **) dict);
	if( d == NULL){
		return;
	}

	free( d->filename);

	struct _NPSection *iter_section = d->sections;
	struct _NPKey *iter_key = NULL;

	do{
		free( iter_section->name);
		while( iter_section->keys){
			iter_key = iter_section->keys;
			free( iter_key->name);
			free( iter_key->value);

			iter_section->keys = iter_key->next;
			free( iter_key);
		}
		d->sections = iter_section->next;
		free( iter_section);
	} while( (iter_section = d->sections) != NULL);

}


int np_save_dictionary( NPDict ndict, const char *filename)
{
	struct _NPDict *dict = (struct _NPDict *) ndict;
	if( dict == NULL || filename == NULL){
		return -1;
	}

	FILE *fp = fopen( filename, "wt");
	if( fp == NULL){
		return -2;
	}

	struct _NPSection *iter_section = dict->sections;
	struct _NPKey *iter_key = NULL;

	do{
		if( iter_section->keys){
			fprintf( fp, "[%s]\n", iter_section->name);

			iter_key = iter_section->keys;
			do{
				if( iter_key->name && iter_key->value){
					fprintf( fp, "%s = %s\n", iter_key->name, iter_key->value);
				}
			} while( (iter_key = iter_key->next) != NULL);
		}
		fprintf( fp, "\n"); // This just looks better IMHO
	} while( (iter_section = iter_section->next) != NULL);

	fclose( fp);
	return 0;
}


int np_set_string( NPDict ndict, const char *section, const char *key, const char *value)
{
	struct _NPDict *dict = (struct _NPDict *) ndict;
	if( dict == NULL || key == NULL || value == NULL){
		return -1;
	}

	struct _NPSection *iter_section = dict->sections;
	struct _NPKey *iter_key = NULL;

	// First try to find the section.
	do{
		if( strcmp( section, iter_section->name) == 0){
			iter_key = iter_section->keys;
			while( iter_key){
				if( strcmp( key, iter_key->name) == 0){
					free( iter_key->value);
					iter_key->value = strdup( value);
					return 0;
				}
				iter_key = iter_key->next;
			}

			_np_add_key( &(iter_section->keys), key, value);
			return 0;
		}
	} while( (iter_section = iter_section->next) != NULL);

	// The section doesn't exist, create it
	iter_section = dict->sections;
	while( iter_section->next){
		iter_section = iter_section->next;
	}
	iter_section->next = (struct _NPSection *) calloc( 1, sizeof( struct _NPSection));
	iter_section = iter_section->next;
	iter_section->name = strdup( section);

	_np_add_key( &(iter_section->keys), key, value);
	return 0;
}

int np_set_long( NPDict dict, const char *section, const char *key, long value)
{
	if( dict == NULL || key == NULL){
		return -1;
	}

	int size = 2; // One for the final '\0' and one for the sign.
	long tmp = labs(value);
	while( tmp > 0){
		size += 3;
		tmp /= 1000;
	}

	char *str_value = (char *) calloc( size, sizeof( char));
	snprintf( str_value, size - 1, "%ld", value);

	int retval = np_set_string( dict, section, key, str_value);
	free( str_value);

	return retval;
}


char *np_get_string( NPDict ndict, const char *section, const char *key, const char *default_value)
{
	struct _NPDict *dict = (struct _NPDict *) ndict;

	if( dict == NULL || section == NULL || key == NULL){
		return NULL;
	}

	struct _NPSection *iter_section = dict->sections;
	struct _NPKey *iter_key = NULL;

	do{
		if( strcmp( section, iter_section->name) == 0){
			iter_key = iter_section->keys;
			while( iter_key){
				if( strcmp( key, iter_key->name) == 0){
					if( iter_key->value){
						return strdup( iter_key->value);
					}
					goto set_default;
				}
				iter_key = iter_key->next;
			}
		}
	} while( (iter_section = iter_section->next) != NULL);

	set_default:

	return default_value ? strdup( default_value) : NULL;
}

long np_get_long( NPDict dict, const char *section, const char *key, long default_value)
{
	if( dict == NULL || section == NULL || key == NULL){
		return default_value;
	}

	char *str_value = np_get_string( dict, section, key, NULL);
	if( str_value == NULL){
		return default_value;
	}

	char *endptr=NULL;

	long value = strtol( str_value, &endptr, 10);

	if( endptr == str_value){
		free( str_value);
		return default_value;
	}

	free( str_value);
	return value;
}

