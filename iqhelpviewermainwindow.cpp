#include "iqhelpviewermainwindow.h"
#include "ui_iqhelpviewermainwindow.h"
#include <QSettings>
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include "iqwebhelpview.h"
#include "iqpdfhelpview.h"

IqHelpViewerMainWindow::IqHelpViewerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IqHelpViewerMainWindow),
    _urlLineEdit(new QLineEdit(this)),
    _findStringLineEdit(new QLineEdit(this)),
    _findLabel(new QLabel(this)),
    _scaleLabel(new QLabel(this)),
    _scaleComboBox(new QComboBox(this)),
    _latLocaleButton(new QRadioButton(this)),
    _rusLocaleButton(new QRadioButton(this)),
    _currentView(NULL),
    _mainWidget(new QWidget()),
    _currentHistoryIndex(0),
    _backOrForward(false)
{   
    ui->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout();
    _mainWidget->setLayout(layout);
    setCentralWidget(_mainWidget);

    connect(ui->actionHome, SIGNAL(triggered()), this, SLOT(showHomePage()));
    ui->navigationToolBar->addAction(ui->actionHome);

    connect(ui->actionBack, SIGNAL(triggered()), this, SLOT(back()));
    ui->navigationToolBar->addAction(ui->actionBack);

    connect(ui->actionForward, SIGNAL(triggered()), this, SLOT(forward()));
    ui->navigationToolBar->addAction(ui->actionForward);

    ui->navigationToolBar->addAction(ui->actionRefresh);

    _urlLineEdit->setReadOnly(true);
    ui->navigationToolBar->addWidget(_urlLineEdit);

    _scaleLabel->setText(tr("Scale: "));
    ui->toolsToolBar->addWidget(_scaleLabel);

    QSettings settings;
    if (!settings.contains("scaleFactors"))
    {
        QStringList scaleFactors;
        scaleFactors << "200" << "150" << "125" << "100" << "75" << "50" << "25";
        settings.setValue("scaleFactors", scaleFactors);
    }
    _scaleComboBox->setEditable(true);
    _scaleComboBox->setMinimumWidth(50);
    QStringList scaleFactors = settings.value("scaleFactors").toStringList();
    _scaleComboBox->addItems(scaleFactors);
//    _scaleComboBox->setCurrentText("100");
    _scaleComboBox->setEditText("100");
//    connect(_scaleComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setZoomFactor(QString)));
    connect(_scaleComboBox, SIGNAL(editTextChanged(QString)), this, SLOT(setZoomFactor(QString)));
    ui->toolsToolBar->addWidget(_scaleComboBox);

    connect(ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    ui->toolsToolBar->addAction(ui->actionZoomOut);

    connect(ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    ui->toolsToolBar->addAction(ui->actionZoomIn);

    ui->toolsToolBar->addSeparator();
    _findLabel->setText(tr("Find: "));
    ui->toolsToolBar->addWidget(_findLabel);
    _findStringLineEdit->setMaximumWidth(150);

    connect(_findStringLineEdit, SIGNAL(textChanged(QString)), this, SLOT(rusificateFindString()));
    connect(_findStringLineEdit, SIGNAL(returnPressed()), this, SLOT(findText()));
    ui->toolsToolBar->addWidget(_findStringLineEdit);

    connect(ui->actionFindNext, SIGNAL(triggered()), this, SLOT(findText()));
    ui->toolsToolBar->addAction(ui->actionFindNext);
    
    _latLocaleButton->setText(tr("lat"));
    _latLocaleButton->setChecked(true);
    connect(_latLocaleButton, SIGNAL(clicked()), this, SLOT(rusificateFindString()));
    ui->toolsToolBar->addWidget(_latLocaleButton);

    _rusLocaleButton->setText(tr("rus"));
    connect(_rusLocaleButton, SIGNAL(clicked()), this, SLOT(rusificateFindString()));
    ui->toolsToolBar->addWidget(_rusLocaleButton);

    insertToolBarBreak(ui->toolsToolBar);

    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));
    ui->helpToolBar->addAction(ui->actionAboutQt);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    ui->helpToolBar->addAction(ui->actionAbout);
		
		ui->helpToolBar->hide();

    //WEB view
    IqWebHelpView *webView = new IqWebHelpView(this);
    _mainWidget->layout()->addWidget(webView->widget());
    _helpViewWidgets << webView;

    //PDF view
    IqPdfHelpView *pdfVier = new IqPdfHelpView(this);
    _mainWidget->layout()->addWidget(pdfVier->widget());
    _helpViewWidgets << pdfVier;

    checkBackForwardEnabled();

    //Content view
    connect(ui->contentTreeWidget, SIGNAL(linkClicked(QUrl)), this, SLOT(showPage(QUrl)));
    ui->contentTreeWidget->header()->hide();
    if (!settings.contains("contentUrl"))
    {
        settings.setValue("contentUrl", "http://localhost/content.xml");
    }
    ui->contentTreeWidget->loadContent(settings.value("contentUrl").toString());

    showHomePage();
}

IqHelpViewerMainWindow::~IqHelpViewerMainWindow()
{
    delete ui;
}

void IqHelpViewerMainWindow::rusificateFindString()
{
    if (_rusLocaleButton->isChecked())
    {
        _findStringLineEdit->setText(rus(_findStringLineEdit->text()));
    }
    else
    {
        _findStringLineEdit->setText(lat(_findStringLineEdit->text()));
    }
}

QString IqHelpViewerMainWindow::rus(const QString& lat) const
{
    QString latL = "1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./";
	QString latU = "!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?";
    QString rusL = tr("1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./");
    QString rusU = tr("!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?");
	
	QString result;
	for(int i = 0; i < lat.count(); i++)
	{
	    QChar ch = lat.at(i);
		int lowIndex = latL.indexOf(ch);
		if (lowIndex != -1)
		{
		    result += rusL.at(lowIndex);
		}
		else
		{
		    int upIndex = latU.indexOf(ch);
			if (upIndex != -1)
			{
			    result += rusU.at(upIndex);
			}
			else
			{
			    result += ch;
			}
		}
	}
	
    return result;
}

QString IqHelpViewerMainWindow::lat(const QString& rus) const
{
    QString latL = "1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./";
    QString latU = "!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?";
    QString rusL = tr("1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./");
    QString rusU = tr("!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?");

    QString result;
    for(int i = 0; i < rus.count(); i++)
    {
        QChar ch = rus.at(i);
        int lowIndex = rusL.indexOf(ch);
        if (lowIndex != -1)
        {
            result += latL.at(lowIndex);
        }
        else
        {
            int upIndex = rusU.indexOf(ch);
            if (upIndex != -1)
            {
                result += latU.at(upIndex);
            }
            else
            {
                result += ch;
            }
        }
    }

    return result;
}

void IqHelpViewerMainWindow::showPage(const QUrl &url)
{
    qDebug() << "showPage";
    IqHelpViewWidget *view = NULL;

    if (_currentView && _currentView->canShowPage(url))
    {
        view = _currentView;
    }
    else
    {
        foreach (IqHelpViewWidget *viewCandidate, _helpViewWidgets)
        {
            if (!viewCandidate)
                continue;
            if (!viewCandidate->canShowPage(url))
                continue;

            view = viewCandidate;
            break;
        }
    }

    if (view)
    {
        setCurrentView(view);

        _urlLineEdit->setText(url.toString());

        view->showPage(url);
    }
}

void IqHelpViewerMainWindow::addUrlToHistory(const QUrl &url)
{
    qDebug() << "addUrlToHistory";
    if (!_backOrForward)
    {
        for (int i = _currentHistoryIndex + 1; i < _history.count(); i++)
        {
            _history.removeLast();
        }
        if (_history.count() == 0 || _history.last() != url)
            _history << url;
        _currentHistoryIndex = _history.count() - 1;
    }
    _backOrForward = false;

    checkBackForwardEnabled();
}

void IqHelpViewerMainWindow::setCurrentView(IqHelpViewWidget *view)
{
    if (_currentView == view)
        return;

    if (_currentView)
    {
        disconnect(_currentView, 0, this, 0);
        disconnect(ui->actionRefresh, 0, _currentView, 0);
    }

    if (!view)
        return;

    QWidget *widget = view->widget();
    if (!widget)
        return;

    _currentView = view;
    connect(view, SIGNAL(linkClicked(QUrl)), this, SLOT(showPage(QUrl)));
    connect(view, SIGNAL(loadProgress(int)), this, SLOT(setLoadProgress(int)));
    connect(view, SIGNAL(loadStarted()), this, SLOT(onLoadStarted()));
    connect(view, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    connect(view, SIGNAL(linkHovered(QString,QString,QString)), this, SLOT(showLinkTitle(QString,QString,QString)));

    connect(ui->actionRefresh, SIGNAL(triggered()), view, SLOT(reload()));

    foreach (IqHelpViewWidget *view, _helpViewWidgets)
    {
        if (view && view->widget())
            view->widget()->hide();
    }

    widget->show();
}

void IqHelpViewerMainWindow::showHomePage()
{
    QSettings setting;
    if (!setting.contains("homePage"))
    {
        setting.setValue("homePage", "http://google.com");
    }
    QString homePage = setting.value("homePage").toString();

    showPage(homePage);
}

void IqHelpViewerMainWindow::back()
{
    if (_currentHistoryIndex == 0)
        return;

    _backOrForward = true;

    _currentHistoryIndex--;
    checkBackForwardEnabled();
    showPage(_history[_currentHistoryIndex]);
}

void IqHelpViewerMainWindow::forward()
{
    if (_currentHistoryIndex == _history.count() - 1)
        return;

    _backOrForward = true;

    _currentHistoryIndex++;
    checkBackForwardEnabled();
    showPage(_history[_currentHistoryIndex]);
}

void IqHelpViewerMainWindow::setLoadProgress(const int progress)
{
    if (!_currentView)
        return;

    ui->statusBar->showMessage(tr("Loading%0 %1%...")
                               .arg(_currentView->title().isEmpty()?"":" \"" + _currentView->title() + "\"")
                               .arg(progress));
}

void IqHelpViewerMainWindow::onLoadStarted()
{
    qDebug() << "onLoadStarted";

    if (!_currentView)
        return;

    ui->statusBar->showMessage(tr("Start loading%0...")
                               .arg(_currentView->title().isEmpty()?"":" \"" + _currentView->title() + "\""));
}

void IqHelpViewerMainWindow::onLoadFinished(bool ok)
{
    qDebug() << "onLoadFinished";
    if (!ok)
    {
        ui->statusBar->showMessage(tr("Loading fauled!"));
        return;
    }

    if (!_currentView)
        return;

    ui->statusBar->showMessage(tr("Lading%0 finish.")
                               .arg(_currentView->title().isEmpty()?"":" \"" + _currentView->title() + "\""));

    _urlLineEdit->setText(_currentView->url().toString());

    addUrlToHistory(_currentView->url());
}

void IqHelpViewerMainWindow::checkBackForwardEnabled()
{
    if (_currentHistoryIndex <= 0)
    {
        ui->actionBack->setEnabled(false);
    }
    else
    {
        ui->actionBack->setEnabled(true);
    }

    if (_currentHistoryIndex >= _history.count() - 1)
    {
        ui->actionForward->setEnabled(false);
    }
    else
    {
        ui->actionForward->setEnabled(true);
    }
}

void IqHelpViewerMainWindow::showLinkTitle(const QString &link, const QString &title, const QString &textContent)
{
    Q_UNUSED(title);
    if (link.isEmpty())
    {
        ui->statusBar->clearMessage();
        return;
    }
    ui->statusBar->showMessage(QString("%0 - %1")
                               .arg(textContent)
                               .arg(link));
}

void IqHelpViewerMainWindow::setZoomFactor(const QString &factor)
{
    if (!_currentView)
        return;

    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    if (factorInt < 1 || factorInt > 100000)
        return;

    _currentView->setZoomFactor((qreal)factorInt/100);
}

void IqHelpViewerMainWindow::zoomIn()
{
    if (!_currentView)
        return;

    QString factor = _scaleComboBox->currentText();
    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    factorInt = factorInt + factorInt*0.25;

    if (factorInt < 1 || factorInt > 100000)
        return;

//    _scaleComboBox->setCurrentText(QString::number(factorInt));
    _scaleComboBox->setEditText(QString::number(factorInt));
}

void IqHelpViewerMainWindow::zoomOut()
{
    if (!_currentView)
        return;

    QString factor = _scaleComboBox->currentText();
    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    factorInt = factorInt - factorInt*0.25;

    if (factorInt < 1 || factorInt > 100000)
        return;

//    _scaleComboBox->setCurrentText(QString::number(factorInt));
    _scaleComboBox->setEditText(QString::number(factorInt));
}

void IqHelpViewerMainWindow::findText()
{
    if (!_currentView)
        return;

    _currentView->findText(_findStringLineEdit->text());
}

void IqHelpViewerMainWindow::showAboutQt()
{
    QMessageBox::aboutQt(this);
}

void IqHelpViewerMainWindow::showAbout()
{
    QMessageBox::about(this, tr("About InfoDoc"),
                       tr("InfoDoc is cross-platform application to view help information for Sintez R2 ATC system.\n\n"
                          "Allow to display pages as HTML or PDF formats.\n\n"
                          "Author: Software group, Puckov P.A."));
}
