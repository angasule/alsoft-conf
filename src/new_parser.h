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


#ifndef NEW_PARSER_H__
#define NEW_PARSER_H__

//typedef void *NPDict;
struct _NPDict;
typedef _NPDict * NPDict;

/**
  @brief    Load the dictionary from a file.
  @param    filename      Filename from which to load the dictionary.
  @param    dict          A pointer to a dictionary descriptor (it should be unused).
  @return   A negative value on error, 0 on success.
 */
int np_load_dictionary( const char *filename, NPDict *dict);

/**
  @brief    Free the resources used by a dictionary.
  @param    dict          Dictionary to be destroyed.
 */
void np_destroy_dictionary( NPDict *dict);

/**
  @brief    Save the dictionary to a file.
  @param    dict          Dictionary to be saved.
  @param    filename      Filename in which the dictionary will be saved.
  @return   A negative value on error, 0 on success.
 */
int np_save_dictionary( NPDict dict, const char *filename);



/**
  @brief    Set an entry in the dictionary in char* type.
  @param    dict          Dictionary in which to set the entry.
  @param    section       Section (also called block) in which the key is found.
  @param    key           Name of the key.
  @param    value         Value to set the key to.
  @return   A negative value on error, 0 on success.
 */
int np_set_string( NPDict ndict, const char *section, const char *key, const char *value);

/**
  @brief    Set an entry in the dictionary in long type.
  @param    dict          Dictionary in which to set the entry.
  @param    section       Section (also called block) in which the key is found.
  @param    key           Name of the key.
  @param    value         Value to set the key to.
  @return   A negative value on error, 0 on success.
 */
int np_set_long( NPDict dict, const char *section, const char *key, long value);



/**
  @brief    Get an entry from the dictionary in char* type.
  @param    dict          Dictionary from which to get the entry.
  @param    section       Section (also called block) in which the key is found.
  @param    key           Name of the key.
  @param    default_value Default value to be returned if the entry does not exist, it may be NULL.
  @return   NULL on error, if the key is not found then a dynamically allocated copy of default_value is returned (which might be NULL).
 */
char *np_get_string( NPDict ndict, const char *section, const char *key, const char *default_value);

/**
  @brief    Get an entry from the dictionary in long type.
  @param    dict          Dictionary from which to get the entry.
  @param    section       Section (also called block) in which the key is found.
  @param    key           Name of the key.
  @param    default_value Default value to be returned if the entry does not exist.
  @return   0 on error, any value (including 0) on success.
 */
long np_get_long( NPDict dict, const char *section, const char *key, long default_value);

#endif
