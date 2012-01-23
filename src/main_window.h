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


#ifndef MAIN_WINDOW_H__
#define MAIN_WINDOW_H__

#include "ui_main.h"

#include "new_parser.h"
#include "confirm_close.h"
#include "speaker.h"

#include <QObject>

class mainWindow : public QMainWindow
{
	Q_OBJECT

	public:
	mainWindow( QMainWindow *parent=0);
	~mainWindow();

	void loadGlobalConfig( void);
	void loadLocalConfig( void);

	void hideGlobalOrLocalButton( void);

	private slots:
	void on_global_or_local_pushButton_clicked( bool checked=false);
	void on_close_or_cancel_pushButton_clicked( bool checked=false);
	void on_apply_pushButton_clicked( bool checked=false);

	void on_enable_backend_pushButton_clicked( bool checked=false);
	void on_disabled_backends_listWidget_itemDoubleClicked( QListWidgetItem *item);
	void on_disable_backend_pushButton_clicked( bool checked=false);
	void on_backends_in_use_listWidget_itemDoubleClicked( QListWidgetItem *item);

	void on_allow_effect_pushButton_clicked( bool checked=false);
	void on_exclude_effect_pushButton_clicked( bool checked=false);
	void on_excluded_effects_listWidget_itemDoubleClicked( QListWidgetItem *item);
	void on_allowed_effects_listWidget_itemDoubleClicked( QListWidgetItem *item);

	void on_speakers_comboBox_currentIndexChanged( int index);
	void on_crossfeed_comboBox_currentIndexChanged( int index);

	int validAngle( int cwise, int currentAngle, int countercwise);
	void speakerOrderCheck( void);

	void on_speaker_bl_spinBox_valueChanged( int i);
	void on_speaker_sl_spinBox_valueChanged( int i);
	void on_speaker_fl_spinBox_valueChanged( int i);
	void on_speaker_fc_spinBox_valueChanged( int i);
	void on_speaker_fr_spinBox_valueChanged( int i);
	void on_speaker_sr_spinBox_valueChanged( int i);
	void on_speaker_br_spinBox_valueChanged( int i);
	void on_speaker_bc_spinBox_valueChanged( int i);

	void on_layout_default_radioButton_clicked( bool checked);
	void on_layout_narrow_radioButton_clicked( bool checked);
	void on_layout_wide_radioButton_clicked( bool checked);

	void setModifiedTrue( void); // Used to receive all modification signals
	void wrapUpAndLeave( void);
	void continueEditing( void);

	private:
	Ui::MainWindow ui;
	bool modified; // true if the current config has been modified
	bool onGlobal; // true if viewing global config, false if local
	char *dictionaryFilename;
	NPDict dict;
	confirmClose confirmDialog;
	QGraphicsScene *scene;
	int currentSpeakerIndex;
	SpeakerItem *speaker_bl;
	SpeakerItem *speaker_sl;
	SpeakerItem *speaker_fl;
	SpeakerItem *speaker_fc;
	SpeakerItem *speaker_fr;
	SpeakerItem *speaker_sr;
	SpeakerItem *speaker_br;
	SpeakerItem *speaker_bc;
	
	void loadConfig( const char *filename);
	void setModified(bool _modified=true);
	void updateDictionary(NPDict d);

	void removeAllSpeakers( void);
	void saveSpeakerLayout( int layout);

	// Helper functions
	const QString getShortName( const QString long_name);
	const QString getLongName( const QString short_name);
	// Return true if "true", "yes", "on" or number != 0, ignores case
	bool string2bool( QString duplicate);
};

// End of include guard
#endif
