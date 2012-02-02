#ifndef CS8MODBUSITEM_H
#define CS8MODBUSITEM_H
//
#include <QString>
#include <QDomElement>
#include <QVariant>
//
class cs8ModbusItem {
public:
	void setOffset(unsigned int value) {
		m_offset = value;
	}
	unsigned int offset() {
		return m_offset;
	}
	void setType(const QString typeChar);
	typedef enum
	{
		Bit, Word, DWord, Float
	} VariableType;
	typedef enum {R, RW} AccessType;

	void setSize(uint value) {
		m_size = value;
	}
	uint size() {
		return m_size;
	}
	void setName(QString value) {
		m_name = value;
	}
	QString name() {
		return m_name;
	}
	cs8ModbusItem(const QDomElement & node);
	cs8ModbusItem();

	void setAccessType(AccessType theValue) {
		m_accessType = theValue;
	}

	AccessType accessType() const {
		return m_accessType;
	}
	int variableSize();
	QChar typeChar(int role=Qt::UserRole);

	void setType(const VariableType& theValue) {
		m_type = theValue;
	}

	VariableType type() const {
		return m_type;
	}

private:
	unsigned int m_offset;
	unsigned int m_size;
	QString m_name;
	VariableType m_type;
	AccessType m_accessType;
};
#endif
