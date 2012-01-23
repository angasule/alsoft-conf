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


#include "main_window.h"

#include <QtGui>
#include <stdio.h>
#include <stdlib.h>

#include <QGraphicsEllipseItem>

#define MONO            0
#define HEADPHONES      1
#define STEREO          2
#define QUAD            3
#define CHN51           4
#define CHN61           5
#define CHN71           6

static int layout_parse( const char *which, const char *entry)
{
	const char *needle = strcasestr( entry, which);
	if( needle == NULL){
		return 0;
	}

	int value = 0;
	int len = strlen( needle);
	int iter = 0;
	char *endptr = NULL;
	while( iter < len){
		if( needle[ iter] == '='){
			iter++;
			value = (int) strtol( needle + iter, &endptr, 10);
			if( endptr == needle + iter){
				return 0;
			}
			return value;
		}
		iter++;
	}

	return 0;
}


void mainWindow::saveSpeakerLayout( int layout)
{
	QString layout_string;
	if( layout == HEADPHONES){
		layout_string = "fl=" + QString::number( speaker_fl->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle());
		np_set_string( dict, "general", "layout_STEREO", layout_string.toAscii().data());
	}
	else if( layout == STEREO){
		layout_string = "fl=" + QString::number( speaker_fl->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle());
		np_set_string( dict, "general", "layout_STEREO", layout_string.toAscii().data());
	}
	else if( layout == QUAD){
		layout_string = "bl=" + QString::number( speaker_bl->getAngle()) + ", fl=" + QString::number( speaker_fl->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle()) + ", br=" + QString::number( speaker_br->getAngle());
		np_set_string( dict, "general", "layout_QUAD", layout_string.toAscii().data());
	}
	else if( layout == CHN51){
		layout_string = "bl=" + QString::number( speaker_bl->getAngle()) + ", fl=" + QString::number( speaker_fl->getAngle()) + ", fc=" + QString::number( speaker_fc->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle()) + ", br=" + QString::number( speaker_br->getAngle());
		np_set_string( dict, "general", "layout_51CHN", layout_string.toAscii().data());
	}
	else if( layout == CHN61){
		layout_string = "sl=" + QString::number( speaker_sl->getAngle()) + ", fl=" + QString::number( speaker_fl->getAngle()) + ", fc=" + QString::number( speaker_fc->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle()) + ", sr=" + QString::number( speaker_sr->getAngle())+ ", bc=" + QString::number( speaker_bc->getAngle());
		np_set_string( dict, "general", "layout_61CHN", layout_string.toAscii().data());
	}
	else if( layout == CHN71){
		layout_string = "bl=" + QString::number( speaker_bl->getAngle()) + ", sl=" + QString::number( speaker_sl->getAngle()) + ", fl=" + QString::number( speaker_fl->getAngle()) + ", fc=" + QString::number( speaker_fc->getAngle()) + ", fr=" + QString::number( speaker_fr->getAngle()) + ", sr=" + QString::number( speaker_sr->getAngle()) + ", br=" + QString::number( speaker_br->getAngle());
		np_set_string( dict, "general", "layout_71CHN", layout_string.toAscii().data());
	}
}


void mainWindow::removeAllSpeakers( void)
{
		ui.speaker_bl_spinBox->setEnabled( false); ui.speaker_bl_label->setEnabled( false); scene->removeItem( speaker_bl);
		ui.speaker_sl_spinBox->setEnabled( false); ui.speaker_sl_label->setEnabled( false); scene->removeItem( speaker_sl);
		ui.speaker_fl_spinBox->setEnabled( false); ui.speaker_fl_label->setEnabled( false); scene->removeItem( speaker_fl);
		ui.speaker_fc_spinBox->setEnabled( false); ui.speaker_fc_label->setEnabled( false); scene->removeItem( speaker_fc);
		ui.speaker_fr_spinBox->setEnabled( false); ui.speaker_fr_label->setEnabled( false); scene->removeItem( speaker_fr);
		ui.speaker_sr_spinBox->setEnabled( false); ui.speaker_sr_label->setEnabled( false); scene->removeItem( speaker_sr);
		ui.speaker_br_spinBox->setEnabled( false); ui.speaker_br_label->setEnabled( false); scene->removeItem( speaker_br);
		ui.speaker_bc_spinBox->setEnabled( false); ui.speaker_bc_label->setEnabled( false); scene->removeItem( speaker_bc);
}


mainWindow::mainWindow( QMainWindow *parent)
	: QMainWindow (parent), modified (false),
	onGlobal(false), dictionaryFilename (NULL)
	, dict(NULL)
	, scene( NULL)
{
	ui.setupUi( this);
	QValidator *validator = new QIntValidator( 0, 2147483647, this);
	ui.frequency_comboBox->setValidator( validator);

	QObject::connect( confirmDialog.ui.buttonBox, SIGNAL( accepted()), this, SLOT( wrapUpAndLeave()));
	QObject::connect( confirmDialog.ui.buttonBox, SIGNAL( rejected()), this, SLOT( continueEditing()));

	ui.backends_in_use_listWidget->setDragDropOverwriteMode( true);
	ui.disabled_backends_listWidget->setDragDropOverwriteMode( true);

	scene = new QGraphicsScene;

	speaker_bl = new SpeakerItem( -150, 20);
	QObject::connect( speaker_bl, SIGNAL( valueChanged( int)), ui.speaker_bl_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_bl);

	speaker_sl = new SpeakerItem( -110, 20);
	QObject::connect( speaker_sl, SIGNAL( valueChanged( int)), ui.speaker_sl_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_sl);

	speaker_fl = new SpeakerItem( -30, 20);
	QObject::connect( speaker_fl, SIGNAL( valueChanged( int)), ui.speaker_fl_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_fl);

	speaker_fc = new SpeakerItem( 0, 20);
	QObject::connect( speaker_fc, SIGNAL( valueChanged( int)), ui.speaker_fc_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_fc);

	speaker_fr = new SpeakerItem( 30, 20);
	QObject::connect( speaker_fr, SIGNAL( valueChanged( int)), ui.speaker_fr_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_fr);

	speaker_sr = new SpeakerItem( 110, 20);
	QObject::connect( speaker_sr, SIGNAL( valueChanged( int)), ui.speaker_sr_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_sr);

	speaker_br = new SpeakerItem( 150, 20);
	QObject::connect( speaker_br, SIGNAL( valueChanged( int)), ui.speaker_br_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_br);

	speaker_bc = new SpeakerItem( 180, 20);
	QObject::connect( speaker_bc, SIGNAL( valueChanged( int)), ui.speaker_bc_spinBox, SLOT( setValue( int)));
	scene->addItem( speaker_bc);

	QGraphicsItem *item = new SpeakerItem( 40, 40, 60);
	item->setFlag( QGraphicsItem::ItemIsMovable, false);
	scene->addItem( item);

	ui.graphicsView->setScene( scene);
	ui.graphicsView->show();
}

mainWindow::~mainWindow()
{
	np_destroy_dictionary( &dict);
}

void mainWindow::wrapUpAndLeave( void)
{
	exit( 0);
}

void mainWindow::continueEditing( void)
{
	this->setEnabled( true);
}

void mainWindow::hideGlobalOrLocalButton( void)
{
	ui.global_or_local_pushButton->hide();
}

void mainWindow::loadGlobalConfig(void)
{
	onGlobal = true;
	loadConfig("/etc/openal/alsoft.conf");
	//TODO Do some error checking
}

void mainWindow::loadLocalConfig(void)
{
	char *filename = NULL;
	size_t len=0;

#ifdef _WIN32
	filename = malloc(MAX_PATH);
	filename[MAX_PATH - 1] = '\0';
	if( SHGetSpecialFolderPathA(NULL, filename, CSIDL_APPDATA, FALSE) != FALSE){
		len = strlen( filename);	
		_snprintf( filename+len, MAX_PATH-len, "\\alsoft.ini");
		loadConfig(filename);
		//TODO Add error checking
	}
	free(filename);
#else
	if( getenv("HOME")){
		len = strlen(getenv("HOME")) + strlen("/.alsoftrc") + 1;
		filename = (char *) malloc( len);
		
		snprintf( filename, len, "%s/.alsoftrc", getenv("HOME"));
		loadConfig(filename);
		free( filename);
#endif
	}else{
		//TODO Do some error reporting.
	}
}

bool mainWindow::string2bool( QString duplicate)
{
	duplicate = duplicate.toLower();
	bool isNumber=false;
	if( duplicate == "true" || duplicate == "yes" || duplicate == "on"
		|| (duplicate.toInt(&isNumber) != 0 && isNumber)){
		return true;
	}else{
		return false;
	}
}

void mainWindow::loadConfig(const char *filename)
{
	np_destroy_dictionary( &dict);
	FILE *fp = fopen( filename, "rt");
	if( fp){
		fclose( fp);
	}else{
		fp = fopen( filename, "wt");
		if( fp){
			fclose( fp);
		}else{
			fprintf(stderr, "ERROR: Can't create %s\n", filename);
			//TODO Show some kind of error dialog.
		}
	}
	if( np_load_dictionary( filename, &dict) < 0){
		//TODO Error checking would be nice.
		;
	}

	if( dictionaryFilename){
		free( dictionaryFilename);
	}
	dictionaryFilename = strdup( filename);

	ui.frequency_comboBox->setEditText( np_get_string( dict, "general", "frequency", "44100"));
	ui.period_size_spinBox->setValue( np_get_long( dict, "general", "period_size", 1024));
	ui.periods_spinBox->setValue( np_get_long( dict, "general", "periods", 4));
	ui.sources_spinBox->setValue( np_get_long( dict, "general", "sources", 256));
	long cf = np_get_long( dict, "general", "cf_level", 0);
	if( cf == 0){
		ui.easy_crossfeed_checkBox->setEnabled( false);
		ui.crossfeed_comboBox->setCurrentIndex( 0);
	}else{
		ui.easy_crossfeed_checkBox->setEnabled( true);
		if( cf > 3){
			cf -= 3;
			ui.easy_crossfeed_checkBox->setChecked( true);
		}
		ui.crossfeed_comboBox->setCurrentIndex(cf);
	}

	
	if( string2bool( np_get_string( dict, "general", "stereodup", "no"))){
		ui.dupstereo_checkBox->setChecked( true);
	}else{
		ui.dupstereo_checkBox->setChecked( false);
	}

	// output format
	QString output_format ( np_get_string( dict, "general", "format", "AL_FORMAT_STEREO16"));

	output_format.remove( 0, (int) strlen( "AL_FORMAT_"));

	int len = output_format.length();
	if( output_format.data()[len-1] == '8'){
		output_format.chop(1);
		ui.sample_depth_comboBox->setCurrentIndex(0);
	}else{ // Ends in 16
		output_format.chop(2);
		ui.sample_depth_comboBox->setCurrentIndex(1);
	}

	ui.layout_custom_radioButton->setChecked( true);
	int speaker_index;
	if( output_format == "MONO"){
		speaker_index = MONO;
	}else if( output_format == "STEREO"){
		QString stereo_layout ( np_get_string( dict, "general", "layout_STEREO", "fl=-90, fr=90"));
		if( layout_parse( "fl", stereo_layout.toAscii().data()) == -90 && layout_parse( "fr", stereo_layout.toAscii().data()) == 90){
			speaker_index = HEADPHONES;
		}
		else{
			speaker_index = STEREO;
		}
	}else if( output_format == "QUAD"){
		speaker_index = QUAD;
	}else if( output_format == "51CHN"){
		speaker_index = CHN51;
	}else if( output_format == "61CHN"){
		speaker_index = CHN61;
	}else if( output_format == "71CHN"){
		speaker_index = CHN71;
	}else{
		speaker_index = STEREO;
	}

	ui.speakers_comboBox->setCurrentIndex( speaker_index);
	on_speakers_comboBox_currentIndexChanged( speaker_index);

	// driver list
	QString drivers( np_get_string( dict, "general", "drivers", "alsa,oss,solaris,dsound,winmm,wport,pulse,wave"));
	if( drivers == ""){
		drivers = "alsa,oss,solaris,dsound,winmm,wport,pulse,wave";
	}
	ui.backends_in_use_listWidget->clear();
	ui.disabled_backends_listWidget->clear();
	ui.disabled_backends_listWidget->addItem( getLongName( "alsa"));
	ui.disabled_backends_listWidget->addItem( getLongName( "dsound"));
	ui.disabled_backends_listWidget->addItem( getLongName( "oss"));
	ui.disabled_backends_listWidget->addItem( getLongName( "port"));
	ui.disabled_backends_listWidget->addItem( getLongName( "pulse"));
	ui.disabled_backends_listWidget->addItem( getLongName( "solaris"));
	ui.disabled_backends_listWidget->addItem( getLongName( "winmm"));
	ui.disabled_backends_listWidget->addItem( getLongName( "wave"));

	drivers = drivers.simplified();
	drivers.remove( QChar(' '));

	QStringList enabledDrivers( drivers.split( ","));
	QList<QListWidgetItem *> item_list;
	for( int q=0; q < enabledDrivers.size(); q++){

		item_list = ui.disabled_backends_listWidget->findItems( getLongName(enabledDrivers.at(q)), Qt::MatchExactly);
		if( item_list.isEmpty()){
			continue;
		}
		ui.disabled_backends_listWidget->takeItem( ui.disabled_backends_listWidget->row( item_list[0]));
		ui.backends_in_use_listWidget->addItem( item_list[0]);
	}

	// Excluded effects list

	QString effects( np_get_string( dict, "general", "excludefx", ""));
	ui.excluded_effects_listWidget->clear();
	ui.allowed_effects_listWidget->clear();
	ui.allowed_effects_listWidget->addItem( getLongName( "echo"));
	ui.allowed_effects_listWidget->addItem( getLongName( "reverb"));
	ui.allowed_effects_listWidget->addItem( getLongName( "eaxreverb"));

	effects = effects.simplified();
	effects.remove( QChar(' '));

	QStringList excludedEffects( effects.split( ","));
	QList<QListWidgetItem *> effect_list;
	for( int q=0; q < excludedEffects.size(); q++){

		effect_list = ui.allowed_effects_listWidget->findItems( getLongName(excludedEffects.at(q)), Qt::MatchExactly);
		if( effect_list.isEmpty()){
			continue;
		}
		ui.allowed_effects_listWidget->takeItem( ui.allowed_effects_listWidget->row( effect_list[0]));
		ui.excluded_effects_listWidget->addItem( effect_list[0]);
	}

	// Effects EFX
	ui.EFX_slots_spinBox->setValue( np_get_long( dict, "general", "slots", 4));
	ui.EFX_sends_spinBox->setValue( np_get_long( dict, "general", "sends", 2));

	// ALSA
	ui.ALSA_device_lineEdit->setText( np_get_string( dict, "alsa", "device", "default"));
	ui.ALSA_capture_lineEdit->setText( np_get_string( dict, "alsa", "capture", "default"));
	if( string2bool( np_get_string( dict, "alsa", "mmap", "yes"))){
		ui.ALSA_mmap_checkBox->setChecked( true);
	}else{
		ui.ALSA_mmap_checkBox->setChecked( false);
	}

	// OSS
	ui.OSS_device_lineEdit->setText( np_get_string( dict, "oss", "device", "/dev/dsp"));
	ui.OSS_capture_lineEdit->setText( np_get_string( dict, "oss", "capture", "/dev/dsp"));

	// PortAudio
	ui.PortAudio_device_index_spinBox->setValue( np_get_long( dict, "port", "device", -1));

	// PulseAudio

	// Solaris
	ui.Solaris_device_lineEdit->setText( np_get_string( dict, "solaris", "device", "/dev/audio"));

	// DirectSound

	// Windows MultiMedia

	// WAVE File Writer
	ui.WAVE_file_lineEdit->setText( np_get_string( dict, "wave", "file", ""));
	
	setModified( false);
}

void mainWindow::on_speakers_comboBox_currentIndexChanged( int index)
{
	if( index < 0){
		return;
	}

	setModified( true);
	saveSpeakerLayout( index);
	currentSpeakerIndex = index;

	if( index == 0){ // mono
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 2);

		removeAllSpeakers();

	} else if( index == 1){ // headphones
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 0);

		speaker_fl->setAngle( -90);
		speaker_fr->setAngle( 90);

		removeAllSpeakers();
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);

	} else if( index == 2){ // stereo
		ui.layout_presets_stackedWidget->setCurrentIndex( 0);
		ui.layout_options_stackedWidget->setCurrentIndex( 0);

		char *speakers = np_get_string( dict, "general", "layout_STEREO", "fl=-30, fr=30");
		speaker_fl->setAngle( layout_parse( "fl", speakers));
		speaker_fr->setAngle( layout_parse( "fr", speakers));
		free( speakers); speakers = NULL;

		removeAllSpeakers();
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);

	} else if( index == 3){ // quadraphonic
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 1);

		char *speakers = np_get_string( dict, "general", "layout_QUAD", "bl=-135, fl=-45, fr=45, br=135");
		speaker_bl->setAngle( layout_parse( "bl", speakers));
		speaker_fl->setAngle( layout_parse( "fl", speakers));
		speaker_fr->setAngle( layout_parse( "fr", speakers));
		speaker_br->setAngle( layout_parse( "br", speakers));
		free( speakers); speakers = NULL;

		removeAllSpeakers();
		ui.speaker_bl_spinBox->setEnabled( true); ui.speaker_bl_label->setEnabled( true); scene->addItem( speaker_bl);
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);
		ui.speaker_br_spinBox->setEnabled( true); ui.speaker_br_label->setEnabled( true); scene->addItem( speaker_br);

	} else if( index == 4){ // 5.1
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 1);

		char *speakers = np_get_string( dict, "general", "layout_51CHN", "bl=-110, fl=-30, fc=0, fr=30, br=110");
		speaker_bl->setAngle( layout_parse( "bl", speakers));
		speaker_fl->setAngle( layout_parse( "fl", speakers));
		speaker_fc->setAngle( layout_parse( "fc", speakers));
		speaker_fr->setAngle( layout_parse( "fr", speakers));
		speaker_br->setAngle( layout_parse( "br", speakers));
		free( speakers); speakers = NULL;

		removeAllSpeakers();
		ui.speaker_bl_spinBox->setEnabled( true); ui.speaker_bl_label->setEnabled( true); scene->addItem( speaker_bl);
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fc_spinBox->setEnabled( true); ui.speaker_fc_label->setEnabled( true); scene->addItem( speaker_fc);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);
		ui.speaker_br_spinBox->setEnabled( true); ui.speaker_br_label->setEnabled( true); scene->addItem( speaker_br);

	} else if( index == 5){ // 6.1
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 1);

		char *speakers = np_get_string( dict, "general", "layout_61CHN", "sl=-90, fl=-30, fc=0, fr=30, br=90, bc=180");
		speaker_bl->setAngle( layout_parse( "bl", speakers));
		speaker_fl->setAngle( layout_parse( "fl", speakers));
		speaker_fc->setAngle( layout_parse( "fc", speakers));
		speaker_fr->setAngle( layout_parse( "fr", speakers));
		speaker_br->setAngle( layout_parse( "br", speakers));
		speaker_bc->setAngle( layout_parse( "bc", speakers));
		free( speakers); speakers = NULL;

		removeAllSpeakers();
		ui.speaker_sl_spinBox->setEnabled( true); ui.speaker_sl_label->setEnabled( true); scene->addItem( speaker_sl);
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fc_spinBox->setEnabled( true); ui.speaker_fc_label->setEnabled( true); scene->addItem( speaker_fc);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);
		ui.speaker_sr_spinBox->setEnabled( true); ui.speaker_sr_label->setEnabled( true); scene->addItem( speaker_sr);
		ui.speaker_bc_spinBox->setEnabled( true); ui.speaker_bc_label->setEnabled( true); scene->addItem( speaker_bc);

	} else if( index == 6){ // 7.1
		ui.layout_presets_stackedWidget->setCurrentIndex( 1);
		ui.layout_options_stackedWidget->setCurrentIndex( 1);

		char *speakers = np_get_string( dict, "general", "layout_71CHN", "bl=-150, sl=-90, fl=-30, fc=0, fr=30, sr=90, br=150");
		speaker_bl->setAngle( layout_parse( "bl", speakers));
		speaker_sl->setAngle( layout_parse( "sl", speakers));
		speaker_fl->setAngle( layout_parse( "fl", speakers));
		speaker_fc->setAngle( layout_parse( "fc", speakers));
		speaker_fr->setAngle( layout_parse( "fr", speakers));
		speaker_sr->setAngle( layout_parse( "sr", speakers));
		speaker_br->setAngle( layout_parse( "br", speakers));
		free( speakers); speakers = NULL;

		removeAllSpeakers();
		ui.speaker_bl_spinBox->setEnabled( true); ui.speaker_bl_label->setEnabled( true); scene->addItem( speaker_bl);
		ui.speaker_sl_spinBox->setEnabled( true); ui.speaker_sl_label->setEnabled( true); scene->addItem( speaker_sl);
		ui.speaker_fl_spinBox->setEnabled( true); ui.speaker_fl_label->setEnabled( true); scene->addItem( speaker_fl);
		ui.speaker_fc_spinBox->setEnabled( true); ui.speaker_fc_label->setEnabled( true); scene->addItem( speaker_fc);
		ui.speaker_fr_spinBox->setEnabled( true); ui.speaker_fr_label->setEnabled( true); scene->addItem( speaker_fr);
		ui.speaker_sr_spinBox->setEnabled( true); ui.speaker_sr_label->setEnabled( true); scene->addItem( speaker_sr);
		ui.speaker_br_spinBox->setEnabled( true); ui.speaker_br_label->setEnabled( true); scene->addItem( speaker_br);
	}
}

void mainWindow::on_crossfeed_comboBox_currentIndexChanged( int index)
{
	if( index < 0){
		return;
	} else if( index == 0){
		ui.easy_crossfeed_checkBox->setEnabled( false);
	} else{
		ui.easy_crossfeed_checkBox->setEnabled( true);
	}
	setModified( true);
}


// Feed the three angles, and it returns a valid angle (if it was invalid, it'll move it to the nearest, minus one to the inside)
int mainWindow::validAngle( int cwise, int currentAngle, int countercwise)
{
	if( cwise > currentAngle && currentAngle > countercwise)
		return currentAngle;
	else if( cwise > currentAngle && cwise < countercwise && currentAngle < countercwise)
		return currentAngle;
	else if( currentAngle > countercwise && cwise < currentAngle && cwise < countercwise)
		return currentAngle;

	// By this point we know it's the wrong angle
	if( currentAngle < countercwise && currentAngle < cwise)
		return countercwise + 1;
	else if( currentAngle > countercwise && currentAngle > cwise)
		return cwise - 1;
	else if( currentAngle - cwise > currentAngle - countercwise)
		return cwise == -179 ? 180 : cwise - 1;
	else
		return countercwise == 180 ? -179 : countercwise + 1;
}


void mainWindow::speakerOrderCheck( void)
{
	// TODO Not properly implemented yet, so it's disabled

	return;

	if( currentSpeakerIndex == MONO
		|| currentSpeakerIndex == HEADPHONES
		|| currentSpeakerIndex == STEREO){
		return;
	}

	int bl = ui.speaker_bl_spinBox->value();
	int sl = ui.speaker_sl_spinBox->value();
	int fl = ui.speaker_fl_spinBox->value();
	int fc = ui.speaker_fc_spinBox->value();
	int fr = ui.speaker_fr_spinBox->value();
	int sr = ui.speaker_sr_spinBox->value();
	int br = ui.speaker_br_spinBox->value();
	int bc = ui.speaker_bc_spinBox->value();

	if( currentSpeakerIndex == QUAD){
		speaker_bl->setAngle( validAngle( fl, bl, br));
		speaker_fl->setAngle( validAngle( fr, fl, bl));
		speaker_fr->setAngle( validAngle( br, fr, fl));
	} else if( currentSpeakerIndex == CHN51){
		speaker_bl->setAngle( validAngle( fl, bl, br));
		speaker_fl->setAngle( validAngle( fc, fl, bl));
		speaker_fc->setAngle( validAngle( fr, fc, fl));
		speaker_fr->setAngle( validAngle( br, fr, fl));
	} else if( currentSpeakerIndex == CHN61){
		speaker_sl->setAngle( validAngle( fl, sl, bc));
		speaker_fl->setAngle( validAngle( fc, fl, sl));
		speaker_fc->setAngle( validAngle( fr, fc, fl));
		speaker_fr->setAngle( validAngle( sr, fr, fc));
		speaker_sr->setAngle( validAngle( bc, sr, fr));
	} else if( currentSpeakerIndex == CHN71){
		speaker_bl->setAngle( validAngle( sl, bl, bc));
		speaker_sl->setAngle( validAngle( fl, sl, bl));
		speaker_fl->setAngle( validAngle( fc, fl, sl));
		speaker_fc->setAngle( validAngle( fr, fc, fl));
		speaker_fr->setAngle( validAngle( sr, fr, fc));
		speaker_sr->setAngle( validAngle( bc, sr, fr));
	}
}


void mainWindow::on_speaker_bl_spinBox_valueChanged( int i)
{
	speaker_bl->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_sl_spinBox_valueChanged( int i)
{
	speaker_sl->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_fl_spinBox_valueChanged( int i)
{
	speaker_fl->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_fc_spinBox_valueChanged( int i)
{
	speaker_fc->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_fr_spinBox_valueChanged( int i)
{
	speaker_fr->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_sr_spinBox_valueChanged( int i)
{
	speaker_sr->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_br_spinBox_valueChanged( int i)
{
	speaker_br->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_speaker_bc_spinBox_valueChanged( int i)
{
	speaker_bc->setAngle( i);
	speakerOrderCheck();
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_layout_default_radioButton_clicked( bool checked)
{
	speaker_fl->setAngle( -40);
	speaker_fr->setAngle( 40);
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_layout_narrow_radioButton_clicked( bool checked)
{
	speaker_fl->setAngle( -20);
	speaker_fr->setAngle( 20);
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_layout_wide_radioButton_clicked( bool checked)
{
	speaker_fl->setAngle( -80);
	speaker_fr->setAngle( 80);
	saveSpeakerLayout( currentSpeakerIndex);
	setModified( true);
}


void mainWindow::on_global_or_local_pushButton_clicked( bool checked)
{
	if( onGlobal){
		onGlobal = false;
		ui.global_or_local_pushButton->setText(tr("View g&lobal configuration"));
		loadLocalConfig();
	}else{
		onGlobal = true;
		ui.global_or_local_pushButton->setText(tr("View &local configuration"));
		loadGlobalConfig();
	}
}

void mainWindow::on_close_or_cancel_pushButton_clicked( bool checked)
{
	if( modified){
		confirmDialog.show();
		// These two aren't needed, I think, I'm leaving them here
		// for future review on other platforms.
		// TODO Force the dialog to stay on top
		//confirmDialog.raise();
		//confirmDialog.setFocus();
		this->setEnabled( false);
	}else{
		QCoreApplication::instance()->quit();
	}
}

void mainWindow::on_apply_pushButton_clicked( bool checked)
{
	updateDictionary( dict);
	if( dictionaryFilename){
		np_save_dictionary( dict, dictionaryFilename);
		// TODO Error checking
/*
		FILE *fp = fopen(dictionaryFilename, "wt");
		if( fp == NULL){
			fprintf(stderr, "ERROR: Failed to open %s\n", dictionaryFilename);
		}
		iniparser_dump_ini(dict, fp);
		fclose( fp);
*/
	}else{
		fprintf( stderr, "ERROR: dictionary filename not set\n");
	}
	setModified( false);
}

void mainWindow::setModified(bool _modified)
{
	modified = _modified;
	ui.apply_pushButton->setEnabled(_modified);
	if( modified){
		ui.close_or_cancel_pushButton->setText("&Cancel");
	}else{
		ui.close_or_cancel_pushButton->setText("&Close");
	}
}

const QString mainWindow::getShortName( const QString long_name)
{
	if( long_name == "ALSA"){
		return QString("alsa");
	}else if( long_name == "Solaris"){
		return QString("solaris");
	}else if( long_name == "PortAudio"){
		return QString("port");
	}else if( long_name == "PulseAudio"){
		return QString("pulse");
	}else if( long_name == "OSS"){
		return QString("oss");
	}else if( long_name == "DirectSound"){
		return QString("dsound");
	}else if( long_name == tr("Windows Multimedia")){
		return QString("winmm");
	}else if( long_name == tr("WAVE File Writer")){
		return QString("wave");
	}else if( long_name == tr("Echo")){
		return QString("echo");
	}else if( long_name == tr("Reverb")){
		return QString("reverb");
	}else if( long_name == tr("EAX Reverb")){
		return QString("eaxreverb");
	}
	return QString("ERROR");
}

const QString mainWindow::getLongName( const QString short_name)
{
	if( short_name == "alsa"){
		return QString("ALSA");
	}else if( short_name == "solaris"){
		return QString("Solaris");
	}else if( short_name == "port"){
		return QString("PortAudio");
	}else if( short_name == "pulse"){
		return QString("PulseAudio");
	}else if( short_name == "oss"){
		return QString("OSS");
	}else if( short_name == "dsound"){
		return QString("DirectSound");
	}else if( short_name == "winmm"){
		return QString(tr("Windows Multimedia"));
	}else if( short_name == "wave"){
		return QString(tr("WAVE File Writer"));
	}else if( short_name == "echo"){
		return QString(tr("Echo"));
	}else if( short_name == "reverb"){
		return QString(tr("Reverb"));
	}else if( short_name == "eaxreverb"){
		return QString(tr("EAX Reverb"));
	}
	return QString("ERROR");
}

void mainWindow::updateDictionary(NPDict d)
{
	QString depth = ui.sample_depth_comboBox->currentText();
	if( depth == "8-bit"){
		depth = "8";
	} if( depth == "16-bit"){
		depth = "16";
	} else{
		depth = "32";
	}

	int speakers_index = ui.speakers_comboBox->currentIndex();
	QString speakers;
	if( speakers_index == MONO){
		speakers = "MONO";
	}else if( speakers_index == HEADPHONES){
		speakers = "STEREO";
	}else if( speakers_index == STEREO){
		speakers = "STEREO";
	}else if( speakers_index == QUAD){
		speakers = "QUAD";
	}else if( speakers_index == CHN51){
		speakers = "51CHN";
	}else if( speakers_index == CHN61){
		speakers = "61CHN";
	}else if( speakers_index == CHN71){
		speakers = "71CHN";
	}
	QString output_format = "AL_FORMAT_" + speakers + depth;
	np_set_string( d, "general", "format", output_format.toAscii().data());

	if( ui.crossfeed_comboBox->currentIndex() > 0){
		int cf;
		cf = ui.crossfeed_comboBox->currentIndex();
		cf += ui.easy_crossfeed_checkBox->isChecked() ? 3 : 0;
		np_set_string( d, "general", "cf_level", QString::number( cf).toAscii().data());
	}else{
		np_set_string( d, "general", "cf_level", "0");
	}

	np_set_string( d, "general", "sources", ui.sources_spinBox->text().toAscii().data());
	np_set_string( d, "general", "frequency", ui.frequency_comboBox->currentText().toAscii().data());
	np_set_string( d, "general", "period_size", ui.period_size_spinBox->text().toAscii().data());
	np_set_string( d, "general", "periods", ui.periods_spinBox->text().toAscii().data());
	np_set_string( d, "general", "stereodup", ui.dupstereo_checkBox->isChecked() ? "true" : "false");

	// List of excluded effects

	QString effects;
	int max_count, count=0;
	max_count = ui.excluded_effects_listWidget->count();
	for( count=0 ; count < max_count ; count++){
		effects += getShortName( ui.excluded_effects_listWidget->item(count)->text());
		if( count < max_count-1){
			effects += ",";
		}
	}
	np_set_string( d, "general", "excludefx", effects.toAscii().data());

	// Effects EFX
	np_set_string( d, "general", "slots", ui.EFX_slots_spinBox->text().toAscii().data());
	np_set_string( d, "general", "sends", ui.EFX_sends_spinBox->text().toAscii().data());

	// List of enabled backends

	QString drivers;
	count=0;
	max_count = ui.backends_in_use_listWidget->count();
	for( count=0 ; count < max_count ; count++){
		drivers += getShortName( ui.backends_in_use_listWidget->item(count)->text());
		if( count < max_count-1){
			drivers += ",";
		}
	}
	np_set_string( d, "general", "drivers", drivers.toAscii().data());

	// Now save the settings for each backend
	
	// ALSA
	np_set_string( d, "alsa", "device", ui.ALSA_device_lineEdit->text().toAscii().data());
	np_set_string( d, "alsa", "capture", ui.ALSA_capture_lineEdit->text().toAscii().data());
	np_set_string( d, "alsa", "mmap", ui.ALSA_mmap_checkBox->isChecked() ? "true" : "false");

	// DirectSound

	// OSS
	np_set_string( d, "oss", "device", ui.OSS_device_lineEdit->text().toAscii().data());
	np_set_string( d, "oss", "capture", ui.OSS_capture_lineEdit->text().toAscii().data());

	// PortAudio
	np_set_string( d, "port", "device", ui.PortAudio_device_index_spinBox->text().toAscii().data());

	// Solaris
	np_set_string( d, "solaris", "device", ui.Solaris_device_lineEdit->text().toAscii().data());

	// WAVE File Writer
	np_set_string( d, "wave", "file", ui.WAVE_file_lineEdit->text().toAscii().data());

}

void mainWindow::setModifiedTrue( void)
{
	setModified( true);
}

void mainWindow::on_allow_effect_pushButton_clicked( bool checked)
{
	QList<QListWidgetItem *> item_list;

	item_list = ui.excluded_effects_listWidget->selectedItems();
	if( item_list.isEmpty()){
		return;
	}
	on_excluded_effects_listWidget_itemDoubleClicked( item_list[0]);
}

void mainWindow::on_excluded_effects_listWidget_itemDoubleClicked( QListWidgetItem *item)
{
	ui.excluded_effects_listWidget->takeItem( ui.excluded_effects_listWidget->row( item));
	ui.allowed_effects_listWidget->addItem( item);
	setModified( true);
}

void mainWindow::on_exclude_effect_pushButton_clicked( bool checked)
{
	QList<QListWidgetItem *> item_list;

	item_list = ui.allowed_effects_listWidget->selectedItems();
	if( item_list.isEmpty()){
		return;
	}
	on_allowed_effects_listWidget_itemDoubleClicked( item_list[0]);
}

void mainWindow::on_allowed_effects_listWidget_itemDoubleClicked( QListWidgetItem *item)
{
	ui.allowed_effects_listWidget->takeItem( ui.allowed_effects_listWidget->row( item));
	ui.excluded_effects_listWidget->addItem( item);
	setModified( true);
}

void mainWindow::on_enable_backend_pushButton_clicked( bool checked)
{
	QList<QListWidgetItem *> item_list;

	item_list = ui.disabled_backends_listWidget->selectedItems();
	if( item_list.isEmpty()){
		return;
	}
	on_disabled_backends_listWidget_itemDoubleClicked( item_list[0]);
}

void mainWindow::on_disabled_backends_listWidget_itemDoubleClicked( QListWidgetItem *item)
{
	ui.disabled_backends_listWidget->takeItem( ui.disabled_backends_listWidget->row( item));
	ui.backends_in_use_listWidget->addItem( item);
	setModified( true);
}

void mainWindow::on_disable_backend_pushButton_clicked( bool checked)
{
	QList<QListWidgetItem *> item_list;

	item_list = ui.backends_in_use_listWidget->selectedItems();
	if( item_list.isEmpty()){
		return;
	}
	on_backends_in_use_listWidget_itemDoubleClicked( item_list[0]);
}

void mainWindow::on_backends_in_use_listWidget_itemDoubleClicked( QListWidgetItem *item)
{
	if( ui.backends_in_use_listWidget->count() <= 1){
		return;
	}
	ui.backends_in_use_listWidget->takeItem( ui.backends_in_use_listWidget->row( item));
	ui.disabled_backends_listWidget->addItem( item);
	setModified( true);
}

