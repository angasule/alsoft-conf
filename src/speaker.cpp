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


#include "speaker.h"

#include <math.h>
#include <QStyleOption>
#include <QGraphicsScene>

#define SPEAKER_CENTRE 40
#define SPREAD_RADIUS  40

SpeakerItem::SpeakerItem( double _x, double _y, double _radius)
{
	setPos( _x, _y);
	radius = _radius > 0 ? _radius : 20;
	setFlag( QGraphicsItem::ItemIsMovable);
}


SpeakerItem::SpeakerItem( double _angle, double _radius)
{
	radius = _radius > 0 ? _radius : 20;
	setFlag( QGraphicsItem::ItemIsMovable);
	setAngle( _angle);
	emit valueChanged( getAngle());
}


QRectF SpeakerItem::boundingRect() const
{
	return QRectF( x() - radius, y() - radius, radius*2, radius*2);
}


void SpeakerItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setRenderHint( QPainter::Antialiasing);

	double _x = x();
	double _y = y();

	painter->setBrush( Qt::black);

	painter->setPen( Qt::black);
	painter->drawEllipse( _x - radius/2, _y - radius/2, radius, radius);

	painter->setPen( Qt::yellow);
	painter->drawEllipse( _x - radius*0.9/2, _y - radius*0.9/2, radius*0.9, radius*0.9);

	painter->setBrush( Qt::yellow);
	painter->drawEllipse( _x - radius/5/2, _y - radius/5/2, radius/5, radius/5);
}

void SpeakerItem::mouseMoveEvent( QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent( event);
	double _x = x() - SPEAKER_CENTRE;
	double _y = y() - SPEAKER_CENTRE;
	double len = sqrt( pow( _x, 2) + pow( _y, 2));
	setPos( (_x / len) * SPREAD_RADIUS + SPEAKER_CENTRE, (_y / len) * SPREAD_RADIUS + SPEAKER_CENTRE);
	emit valueChanged( getAngle());
}

int SpeakerItem::getAngle( void)
{
	double _x = x() - SPEAKER_CENTRE;
	double _y = y() - SPEAKER_CENTRE;
	double len = sqrt( pow( _x, 2) + pow( _y, 2));
	_x /= len;
	_y /= len;

	if( _x == 0){
		if( _y > 0){
			return 180;
		} else{
			return 0;
		}
	}

	_y *= -1;

	double angle = atan( _y / _x) * (180 / M_PI);

	if( _x < 0){
		angle += 180;
	} else{
		if( _y < 0){
			angle += 360;
		}
	}

	if( _x < 0){
		angle -= 90;
		if( angle != 180){ // FIXME This if shouldn't be necessary
			angle *= -1;
		}
	}
	else{
		if( _y >= 0){
			angle = 90 - angle;
		}
		else{
			angle -= 270;
			angle = 180 - angle;
		}
	}

	return (int) round( angle);
}


void SpeakerItem::setAngle( double _angle)
{
	double _x, _y;

	if( _angle < 0){
		_angle *= -1;
		_angle += 90;
	} else{
		_angle = 180 - _angle;
		if( _angle >= 90){
			_angle -= 90;
		} else{
			_angle += 270;
		}
	}

	_angle *= M_PI / 180;

	_y = sin( _angle);
	_y *= -1;

	_x = cos( _angle);

	_x *= SPREAD_RADIUS;
	_y *= SPREAD_RADIUS;
	_x += SPEAKER_CENTRE;
	_y += SPEAKER_CENTRE;

	setPos( _x, _y);
	emit valueChanged( getAngle());
}
