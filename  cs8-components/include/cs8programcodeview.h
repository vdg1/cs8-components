#ifndef CS8POGRAMCODEVIEW_H
#define CS8POGRAMCODEVIEW_H
//
#include <QTextEdit>
#include <QAbstractItemView>

//

class cs8Highlighter;
class cs8ProgramCodeView : public QTextEdit
{
private:
	cs8Highlighter* m_highlighter;
		Q_OBJECT
	private:
		QAbstractItemView* m_masterView;
	public:
		void setMasterView ( QAbstractItemView* value );
		cs8ProgramCodeView ( QWidget * parent=0 );
	protected slots:
		void slotSelectionChanged ( const QItemSelection & selected, const QItemSelection & deselected ) ;

};
#endif
