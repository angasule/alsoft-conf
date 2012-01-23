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


#ifndef SPEAKER_H__
#define SPEAKER_H__

#include <QGraphicsItem>
#include <QPainter>
#include <QObject>

class SpeakerItem : public QObject, public QGraphicsItem
{
	Q_OBJECT

	private:
	double radius;

	signals:
	void valueChanged( int value);

	public:
	SpeakerItem( double _x, double _y, double _radius);
	SpeakerItem( double _angle, double _radius);
	QRectF boundingRect() const;
	void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	void mouseMoveEvent( QGraphicsSceneMouseEvent *event);

	int getAngle( void);
	void setAngle( double _angle);
};

#endif
