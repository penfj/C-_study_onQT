#include "widget.h"
#include <QCoreApplication>
#include <QSize>
#include <QResizeEvent>
#include <QStackedLayout>
#include <QDir>
#include <QDebug>

Widget::Widget(QWidget *parent): QWidget(parent)
  , m_fileSelector(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setupGUI();   //先创建播放界面
    onSelectSong();   //然后让用户选择歌曲，实际开发要考虑保存上一次的播放列表
}

Widget::~Widget()
{

}


void Widget::onSelectSong()
{
    QString strPath = QDir::currentPath();
    QStringList nameFilters("*.mp3");
    nameFilters << "*.wma" << "*.ape" << "*.ogg"
                << "*.aac" << "*.wav" << "*.mid"
                << "*.amr" << "*.3gp" << "*.mp4";
    //下面创建OpenFileWidget帮助用户选择音乐文件
    m_fileSelector = new OpenFileWidget(strPath, nameFilters);
    m_layout->addWidget(m_fileSelector);
    m_layout->setCurrentWidget(m_fileSelector);

    m_fileSelector->setAttribute(Qt::WA_DeleteOnClose);
    connect(m_fileSelector, SIGNAL(selected(QString))
            , this, SLOT(onFileSelected(QString)));
    connect(m_fileSelector, SIGNAL(canceled())
            , this, SLOT(onFileSelectCanceled()));

    m_fileSelector->setStyleSheet("QListWidget{ background: #303030; border: 1px solid white}");
}

void Widget::onFileSelected(QString strFile)
{
    m_layout->removeWidget(m_fileSelector);
    QString strPath;
    m_fileSelector->getPath(strPath);
    QStringList files;
    m_fileSelector->getFileList(files);
    m_fileSelector = 0;

    m_player->playFile(strFile, strPath, files);  //调用musicWidget里面的playFile()接口播放所选音乐文件
}

void Widget::onFileSelectCanceled()
{
    m_layout->removeWidget(m_fileSelector);
    m_fileSelector = 0;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    const QSize &size = event->size();
    if(m_fileSelector) m_fileSelector->setFixedSize(size);
    update();
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    QEvent::Type t = event->type();
    switch(t)
    {
    case QEvent::KeyPress:
        if((static_cast<QKeyEvent *>(event))->key() == Qt::Key_Back)
        {
            event->accept();
            return true;
        }
        break;
    case QEvent::KeyRelease:
        if((static_cast<QKeyEvent *>(event))->key() == Qt::Key_Back)
        {
            event->accept();
            //如果进入了文件选择对话框，则返回，否则退出程序
            if(m_fileSelector)
            {
                m_fileSelector->onBackKey();
            }
            else
            {
                QCoreApplication::quit();
            }
            return true;
        }
        break;
    default:
        break;
    }

    return QWidget::eventFilter(obj, event);
}

void Widget::setupGUI()
{
    m_layout = new QStackedLayout(this);
    m_player = new MusicWidget();
    m_layout->addWidget(m_player);
    connect(m_player, SIGNAL(selectSong()), this, SLOT(onSelectSong()));
}
