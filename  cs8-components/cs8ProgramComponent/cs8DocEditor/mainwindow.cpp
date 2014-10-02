#include "dialogcopyrighteditor.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QDirIterator>

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow ) {
    QCoreApplication::setOrganizationName( "SAXE Swiss System" );
    QCoreApplication::setOrganizationDomain( "hsh.as" );
    QCoreApplication::setApplicationName( "Val3 Documentation Editor" );

    ui->setupUi( this );
    m_application = new cs8Application( this );
    connect( m_application, SIGNAL( modified( bool ) ), this, SLOT( slotModified( bool ) ) );
    ui->listViewProgams->setModel( m_application->programModel() );

    ui->tableViewPars->setMode( cs8ProgramDataView::ParameterData );
    ui->tableViewPars->setMasterView( ui->listViewProgams );

    ui->tableViewVars->setMode( cs8ProgramDataView::LocalData );
    ui->tableViewVars->setMasterView( ui->listViewProgams );

    ui->tableViewReferencedGlobalVaribales->setMode (cs8ProgramDataView::ReferencedGlobalData);
    ui->tableViewReferencedGlobalVaribales->setMasterView (ui->listViewProgams);

    ui->globalDataView->setMode(cs8ProgramDataView::GlobalData);
    ui->globalDataView->setMasterView (ui->listViewProgams);

    ui->widgetDocumentation->setMasterView( ui->listViewProgams );

    connect(ui->widgetDocumentation,SIGNAL(modified(bool)),this,SLOT(slotModified(bool)));

    connect( ui->listViewProgams->selectionModel(), SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ),
             this, SLOT( slotSelectionChanged( QItemSelection, QItemSelection ) ) );

    readSettings();
    createRecentFilesItems();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent( QCloseEvent *event ) {
    if ( maybeSave() ) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::changeEvent( QEvent *e ) {
    QMainWindow::changeEvent( e );
    switch ( e->type() ) {
    case QEvent::LanguageChange:
        ui->retranslateUi( this );
        break;
    default:
        break;
    }
}

void MainWindow::readSettings() {
    QSettings settings;
    QPoint pos = settings.value( "pos", QPoint( 200, 200 ) ).toPoint();
    QSize size = settings.value( "size", QSize( 400, 400 ) ).toSize();
    restoreGeometry( settings.value( "mainWindowGeometry" ).toByteArray() );
    restoreState( settings.value( "mainWindowState" ).toByteArray() );
    //resize(size);
    //move(pos);
}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.setValue( "pos", pos() );
    settings.setValue( "size", size() );
    settings.setValue( "mainWindowGeometry", saveGeometry() );
    settings.setValue( "mainWindowState", saveState() );
}

void MainWindow::createRecentFilesItems() {
    for ( int i = 0; i < MaxRecentFiles; ++i ) {
        recentFileActs[i] = new QAction( this );
        recentFileActs[i]->setVisible( false );
        connect( recentFileActs[i], SIGNAL( triggered() ),
                 this, SLOT( openRecentFile() ) );
    }
    for ( int i = 0; i < MaxRecentFiles; ++i )
        ui->menuRecent->addAction( recentFileActs[i] );
    updateRecentFileActions ();
}


void MainWindow::on_action_Open_triggered() {
    QString applicationName = QFileDialog::getOpenFileName( this, tr( "Open a Val3 project" ), m_application->name(), "(*.pjx)" );
    if ( !applicationName.isEmpty() ) {
        openApplication( applicationName );
    }
}

void MainWindow::on_action_Save_triggered() {
    m_application->save();
}

void MainWindow::slotSelectionChanged( const QItemSelection &selected, const QItemSelection &deselected ) {
    int index = selected.indexes().at( 0 ).row();
    ui->plainTextEditCode->setPlainText( m_application->programModel()->programList().at( index )->toDocumentedCode() );
    ui->labelDeclaration->setText( m_application->programModel()->programList().at( index )->definition() );
}

void MainWindow::openRecentFile() {
    if ( maybeSave() ) {
        QAction *action = qobject_cast<QAction *>( sender() );
        if ( action )
            openApplication( action->data().toString() );
    }
}

void MainWindow::slotModified( bool modified_ ) {
    setWindowModified( modified_ );
}

void MainWindow::on_tableViewPars_doubleClicked( QModelIndex index ) {
    cs8Variable *var = qobject_cast<cs8VariableModel *>( ui->tableViewPars->model() )->variable( index );
    ui->detailEditor->setVariable( var );
    ui->stackedWidget->setCurrentIndex( 1 );
}

void MainWindow::on_detailEditor_done() {
    ui->stackedWidget->setCurrentIndex( 0 );
}

bool MainWindow::maybeSave() {
    if ( m_application->isModified() ) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning( this, tr( "Application" ),
                                    tr( "The document has been modified.\n"
                                        "Do you want to save your changes?" ),
                                    QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel );
        if ( ret == QMessageBox::Save )
            return m_application->save();
        else if ( ret == QMessageBox::Cancel )
            return false;
    }
    return true;
}

void MainWindow::openApplication( const QString &applicationName ) {
    setCurrentFile( applicationName );
    m_application->open( applicationName );
    ui->actionAdd_tags_for_undocumented_symbols->setChecked (m_application->withUndocumentedSymbols ());
}

void MainWindow::updateRecentFileActions() {
    QSettings settings;
    QStringList files = settings.value( "recentFileList" ).toStringList();

    int numRecentFiles = qMin( files.size(), ( int )MaxRecentFiles );

    for ( int i = 0; i < numRecentFiles; ++i ) {
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( files[i] );
        recentFileActs[i]->setText( text );
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }
    for ( int j = numRecentFiles; j < MaxRecentFiles; ++j )
        recentFileActs[j]->setVisible( false );


}

void MainWindow::setCurrentFile( const QString &fileName ) {
    setWindowTitle ( fileName+ " [*]" );
    QSettings settings;
    QStringList files = settings.value( "recentFileList" ).toStringList();
    files.removeAll( fileName );
    files.prepend( fileName );
    while ( files.size() > MaxRecentFiles )
        files.removeLast();

    settings.setValue( "recentFileList", files );

    foreach( QWidget * widget, QApplication::topLevelWidgets() ) {
        MainWindow *mainWin = qobject_cast<MainWindow *>( widget );
        if ( mainWin )
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::on_actionEdit_Copy_Right_Message_triggered() {
    QString msg=m_application->copyRightMessage();
    DialogCopyRightEditor dlg;
    dlg.setCopyRightText (msg);
    if (dlg.exec ()==QDialog::Accepted)
        m_application->setCopyrightMessage (dlg.copyRightText ());

}

void MainWindow::on_actionAdd_tags_for_undocumented_symbols_triggered(bool checked) {
    m_application->setWithUndocumentedSymbols (checked);
}

QStringList scanDirIter(QDir dir) {
    QStringList projects;
    QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        iterator.next();
        //qDebug() << iterator.fileInfo ().absoluteFilePath ();
        if (!iterator.fileInfo().isDir()) {
            QString filename = iterator.fileName();
            if (filename.endsWith(".pjx")) {
                qDebug("Found %s matching pattern.", qPrintable(filename));
                projects<< iterator.fileInfo ().absoluteFilePath ();
            }
        }

    }
    return projects;
}

void MainWindow::on_actionApply_project_data_to_all_sub_projects_triggered() {
    // get list of pro files
    QString projectPath=m_application->projectPath ();
    QStringList projects=scanDirIter (QDir(projectPath));

    foreach(QString projectPath,projects) {
        cs8Application app;
        if (app.open (projectPath)) {
            app.setCopyrightMessage (m_application->copyRightMessage ());
            app.setWithUndocumentedSymbols (m_application->withUndocumentedSymbols ());
            app.save ();
        }
    }
}

