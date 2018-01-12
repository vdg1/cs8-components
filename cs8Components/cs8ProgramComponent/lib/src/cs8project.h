/*
 * cs8project.h
 *
 *  Created on: 2008-nov-02
 *      Author: Volker.Drewer-Gutlan
 */

#ifndef CS8PROJECT_H_
#define CS8PROJECT_H_

#include <QAbstractItemModel>
#include "cs8application.h"

class cs8Project: public QObject /*QAbstractItemModel*/ {
Q_OBJECT
public:
	cs8Project(QObject * parent);
	virtual ~cs8Project();
	bool openDoxyFile(const QString & fileName);

protected:
	QList<cs8Application*> m_applications;
	/*
	QModelIndex
			index(int r, int c, const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& ind, int role) const;
	QModelIndex parent(const QModelIndex& index) const;
	int rowCount(const QModelIndex& ind) const;
	int columnCount(const QModelIndex& ind) const;
*/
};

#endif /* CS8PROJECT_H_ */
