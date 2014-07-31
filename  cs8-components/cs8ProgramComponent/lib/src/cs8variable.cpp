#include "cs8variable.h"
#include <QStringList>
//
cs8Variable::cs8Variable( QDomElement &element, const QString &description )
    : QObject()
{
    m_element = element;
    m_description = description;
}

cs8Variable::cs8Variable()
    : QObject()
{
    m_docFragment = m_doc.createDocumentFragment();
    m_element = m_doc.createElement( "Data" );
    m_docFragment.appendChild( m_element );
}

QString cs8Variable::toString( bool withTypeDefinition )
{
    if ( withTypeDefinition )
        return QString( "%1 %2%3" )
               .arg( type() )
               .arg( use() == "reference" ? "& " : "" )
               .arg( name() );
    else
        return QString( "%1" )
               .arg( name() );
}


QString cs8Variable::documentation( bool withPrefix )
{
    QString out;
    QString prefix = withPrefix ? "///" : "";
    if ( m_description.isEmpty() )
        return QString();

    QStringList list = m_description.split( "\n" );

    //qDebug() << "documentation(): " << m_name << ":" << m_description;
    bool inCodeSection = false;
    bool firstLine = true;
    foreach( QString str, list )
    {
        if ( str.contains( "<code>" ) )
        {
            inCodeSection = true;
            out += prefix + "<br>\n";
        }
        if ( str.contains( "</code>" ) )
        {
            inCodeSection = false;
            out += prefix + "<br>\n";
        }
        out += /*(firstLine ? "" : prefix)*/ prefix + str + ( inCodeSection ? "<br>" : "" ) + "\n";
        firstLine = false;
    }
    //qDebug() << "processed: " << out;
    if ( !isGlobal() )
        return prefix + "\\param " + name() + " " + out + "\n";
    else
        return out;
}

void cs8Variable::setPublic( bool m_public )
{
    emit modified();
    m_element.setAttribute( "access", m_public ? "public" : "private" );

}

QString cs8Variable::definition()
{

    return ( QString( "%1 %2" ).arg( type() ).arg( name() ) ) + ( allSizes() != QString() ? QString(
                "[%1]" ).arg( allSizes() ) : "" );
}

void cs8Variable::setGlobal( bool global )
{
    emit modified();
    m_global = global;
    if ( global )
    {
        m_element.setTagName( "Data" );
        m_element.setAttribute( "xsi:type", "array" );
        if ( !m_element.hasAttribute( "size" ) )
            m_element.setAttribute( "size", "1" );
        m_element.removeAttribute( "use" );
    }
}

QString cs8Variable::allSizes(){
    return m_element.attribute("size").replace(" ",", ");
}

void cs8Variable::setAllSizes(const QString &sizes)
{
    emit modified ();
    m_element.setAttribute ("size",sizes);
}

QVariant cs8Variable::varValue( QString index )
{
    QDomElement e;
    for ( int i = 0; i < m_element.childNodes().count(); i++ )
    {
        if ( m_element.childNodes().at( i ).toElement().attribute( "key" ) == index )
        {
            return m_element.childNodes().at( i ).toElement().attribute( "value" );
        }
    }

    // return empty variable
    if ( type() == "num" )
        return QVariant( 0 );
    else if ( type() == "string" )
        return QVariant( "" );
    else
        return QVariant();
}

void cs8Variable::setUse( QString value )
{
    emit modified();
    m_element.setAttribute( "use", value );
}

QString cs8Variable::use() const
{
    return m_element.attribute( "use", "reference" );
}

void cs8Variable::setDescription( QString value )
{
    emit modified();
    m_description = value;
}

QString cs8Variable::description() const
{
    return m_description;
}

QString cs8Variable::dimension() const
{
    return m_element.attribute( "size", "" );
}

void cs8Variable::setDimension( const QString &dim )
{
    if ( m_element.hasAttribute( "size" ) )
        m_element.setAttribute( "size", dim );
}

void cs8Variable::setType( QString value )
{
    emit modified();
    m_element.setAttribute( "type", value );
}

QString cs8Variable::type() const
{
    return m_element.attribute( "type" );

}

void cs8Variable::setName( QString value )
{
    emit modified();
    m_element.setAttribute( "name", value );
}

//
