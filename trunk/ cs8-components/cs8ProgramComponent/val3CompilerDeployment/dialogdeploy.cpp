#include "dialogdeploy.h"
#include "ui_dialogdeploy.h"

#include <QDir>
#include <QMessageBox>
#include <QDebug>

DialogDeploy::DialogDeploy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

DialogDeploy::~DialogDeploy()
{
    delete ui;
}
QString DialogDeploy::path() const
{
    return m_path;
}

void DialogDeploy::setPath(const QString &path)
{
    m_path = path;
}

void DialogDeploy::activatePreCompiler(bool activate)
{
    QDir packageDir;
    packageDir.setPath(QCoreApplication::applicationDirPath()+"/preCompilerPackage");
    qDebug() << "Search for package files in " << packageDir.absolutePath();
    QStringList fileList=packageDir.entryList(QStringList() << "*.dll" << "*.exe");

    QDir val3Dir;
    val3Dir.setPath(m_path);

    ui->textEdit->append(tr("Applying changes to %1").arg(m_path));
    if (!activate)
        {
            // check if original val3 compiler exists
            if (!QFile::exists(val3Dir.absolutePath()+"/"+VAL3CHECKORIG))
                {
                    QMessageBox::critical(this,
                                          tr("Error"),
                                          tr("The original Val3Check.exe (called Val3Check_Orig.exe) does not exist!"));
                    return;
                }
            foreach (QString fileName, fileList)
                {
                    QString absoluteFilePath=val3Dir.absolutePath()+"/"+fileName;
                    if (!QFile::remove(absoluteFilePath))
                        {
                            ui->textEdit->append(QString("Delete %1 ... failed").arg(fileName));
                            break;
                        }
                    else
                        {
                            ui->textEdit->append(QString("Delete %1 ... ok").arg(fileName));
                        }
                }
            if (!QFile::rename(val3Dir.absolutePath()+"/"+VAL3CHECKORIG,
                               val3Dir.absolutePath()+"/"+VAL3CHECK))
                {
                    ui->textEdit->append(QString("Rename %1 to %2 ... failed")
                                         .arg(VAL3CHECKORIG)
                                         .arg(VAL3CHECK));
                }
            else
                {
                    ui->textEdit->append(QString("Rename %1 to %2 ... ok")
                                         .arg(VAL3CHECKORIG)
                                         .arg(VAL3CHECK));
                }
        }
    else
        {
            // check if original val3 compiler exists
            if (QFile::exists(val3Dir.absolutePath()+"/"+VAL3CHECKORIG))
                {
                    QMessageBox::critical(this,
                                          tr("Error"),
                                          tr("The original VAL3Check.exe (called VAL3Check_Orig.exe) already exists!"));
                    return;
                }
            if (!QFile::rename(val3Dir.absolutePath()+"/"+VAL3CHECK,
                               val3Dir.absolutePath()+"/"+VAL3CHECKORIG))
                {
                    ui->textEdit->append(QString("Rename %1 to %2 ... failed")
                                         .arg(VAL3CHECK)
                                         .arg(VAL3CHECKORIG));
                    return;
                }
            else
                {
                    ui->textEdit->append(QString("Rename %1 to %2 ... ok")
                                         .arg(VAL3CHECK)
                                         .arg(VAL3CHECKORIG));
                }
            foreach (QString fileName, fileList)
                {
                    if (!QFile::copy(packageDir.absolutePath()+"/"+fileName,val3Dir.absolutePath()+"/"+fileName))
                        {
                            ui->textEdit->append(QString("Copy %1 ... failed").arg(fileName));
                            break;
                        }
                    else
                        {
                            ui->textEdit->append(QString("Copy %1 ... ok").arg(fileName));
                        }
                }

        }

}

QString VAL3CHECK = "VAL3Check.exe";
QString VAL3CHECKORIG = "VAL3Check_Orig.exe";

