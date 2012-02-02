#include "cs8modbusitem.h"
#include <QDebug>
//
cs8ModbusItem::cs8ModbusItem( )
{
	m_type = Bit;
	m_size = 1;
	m_name = "";
}
//

cs8ModbusItem::cs8ModbusItem ( const QDomElement & node )
{
	m_name=node.attribute ( "Name" );

	QString syntax=node.attribute ( "Syntaxe" );
	if ( syntax.mid ( 1,1 ) =="Q" )
		m_accessType=RW;
	else
		m_accessType=R;

	if ( syntax.mid ( 2,1 ) =="D" )
		m_type=DWord;
	else if ( syntax.mid ( 2,1 ) =="W" )
		m_type=Word;
	else if ( syntax.mid ( 2,1 ) =="F" )
		m_type=Float;
	else
		m_type=Bit;

	if ( syntax.contains ( ":" ) )
		m_size=syntax.remove ( 0,syntax.indexOf ( ':' ) +1 ).toUInt();
	else
		m_size=1;
}


/*!
    \fn cs8ModbusItem::variableSize()
 */
int cs8ModbusItem::variableSize()
{
	switch ( m_type )
	{
		case Bit:
			return 1;
		case Word:
			return 1;
		case DWord:
			return 2;
		case Float:
			return 2;
	}
	return 0;
}


/*!
    \fn cs8ModbusItem::typeChar(int role)
 */
QChar cs8ModbusItem::typeChar ( int role )
{
	if ( role==Qt::EditRole )
	{
		switch ( m_type )
		{
			case Bit:
				return '0';
			case Word:
				return '1';
			case DWord:
				return '2';
			case Float:
				return '3';
		}
		return QChar();
	}
	else
	{
		switch ( m_type )
		{
			case Bit:
				if ( role==Qt::UserRole )
					return QChar();
				else
					return 'B';
			case Word:
				return 'W';
			case DWord:
				return 'D';
			case Float:
				return 'F';
		}
		return QChar();
	}
}

void cs8ModbusItem::setType ( const QString typeChar )
{
	if ( typeChar=="F" )
		m_type=Float;
	else if ( typeChar=="D" )
		m_type=DWord;
	else if ( typeChar=="W" )
		m_type=Word;
	else
		m_type=Bit;
}
