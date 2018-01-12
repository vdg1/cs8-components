#ifndef CS8PARAMETER_H
#define CS8PARAMETER_H
//
#include "cs8variable.h"
#include <QString>
//
class cs8Parameter : public cs8Variable
{

public:
        cs8Parameter(QDomElement & element, const QString & description=QString());

protected:
	//bool m_byVal;
};
#endif
