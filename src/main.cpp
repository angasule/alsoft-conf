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


#include <QApplication>

#include "main_window.h"

#ifndef _WIN32
#include <unistd.h>
#endif

int main( int argc, char** argv)
{
	QApplication app(argc, argv);
	mainWindow main_window;

#ifndef _WIN32
	if( getuid() == 0){
		main_window.loadGlobalConfig();
	}else{
		main_window.loadLocalConfig();
	}
#else
	// Windows doesn't have a global config
	main_window.loadLocalConfig();
#endif

	main_window.hideGlobalOrLocalButton();
	main_window.show();
	return app.exec();
}
