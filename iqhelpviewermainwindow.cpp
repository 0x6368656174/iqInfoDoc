#include "iqhelpviewermainwindow.h"
#include "ui_iqhelpviewermainwindow.h"
#include <QSettings>
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#include "iqwebhelpview.h"

IqHelpViewerMainWindow::IqHelpViewerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IqHelpViewerMainWindow),
    m_urlLineEdit(new QLineEdit(this)),
    m_findStringLineEdit(new QLineEdit(this)),
    m_findLabel(new QLabel(this)),
    m_scaleLabel(new QLabel(this)),
    m_scaleComboBox(new QComboBox(this)),
    m_latLocaleButton(new QRadioButton(this)),
    m_rusLocaleButton(new QRadioButton(this)),
    m_currentView(NULL),
    m_mainWidget(new QWidget()),
    m_currentHistoryIndex(0),
    m_backOrForward(false)
{   
    ui->setupUi(this);

    QHBoxLayout *layout = new QHBoxLayout();
    m_mainWidget->setLayout(layout);
    setCentralWidget(m_mainWidget);

    connect(ui->actionHome, &QAction::triggered, this, &IqHelpViewerMainWindow::showHomePage);
    ui->navigationToolBar->addAction(ui->actionHome);

    connect(ui->actionBack, &QAction::triggered, this, &IqHelpViewerMainWindow::back);
    ui->navigationToolBar->addAction(ui->actionBack);

    connect(ui->actionForward, &QAction::triggered, this, &IqHelpViewerMainWindow::forward);
    ui->navigationToolBar->addAction(ui->actionForward);

    ui->navigationToolBar->addAction(ui->actionRefresh);

    m_urlLineEdit->setReadOnly(true);
    ui->navigationToolBar->addWidget(m_urlLineEdit);

    m_scaleLabel->setText(tr("Scale: "));
    ui->toolsToolBar->addWidget(m_scaleLabel);

    QSettings settings;
    if (!settings.contains("scaleFactors"))
    {
        QStringList scaleFactors;
        scaleFactors << "200" << "150" << "125" << "100" << "75" << "50" << "25";
        settings.setValue("scaleFactors", scaleFactors);
    }
    m_scaleComboBox->setEditable(true);
    m_scaleComboBox->setMinimumWidth(50);
    QStringList scaleFactors = settings.value("scaleFactors").toStringList();
    m_scaleComboBox->addItems(scaleFactors);
    m_scaleComboBox->setEditText("100");
    connect(m_scaleComboBox, &QComboBox::editTextChanged, this, &IqHelpViewerMainWindow::setZoomFactor);
    ui->toolsToolBar->addWidget(m_scaleComboBox);

    connect(ui->actionZoomIn, &QAction::triggered, this, &IqHelpViewerMainWindow::zoomIn);
    ui->toolsToolBar->addAction(ui->actionZoomOut);

    connect(ui->actionZoomOut, &QAction::triggered, this, &IqHelpViewerMainWindow::zoomOut);
    ui->toolsToolBar->addAction(ui->actionZoomIn);

    ui->toolsToolBar->addSeparator();
    m_findLabel->setText(tr("Find: "));
    ui->toolsToolBar->addWidget(m_findLabel);
    m_findStringLineEdit->setMaximumWidth(150);

    connect(m_findStringLineEdit, &QLineEdit::textChanged, this, &IqHelpViewerMainWindow::rusificateFindString);
    connect(m_findStringLineEdit, &QLineEdit::textChanged, this, &IqHelpViewerMainWindow::findText);
    connect(m_findStringLineEdit, &QLineEdit::returnPressed, this, &IqHelpViewerMainWindow::findText);
    ui->toolsToolBar->addWidget(m_findStringLineEdit);

    connect(ui->actionFindNext, &QAction::triggered, this, &IqHelpViewerMainWindow::findText);
    ui->toolsToolBar->addAction(ui->actionFindNext);
    
    m_latLocaleButton->setText(tr("lat"));
    m_latLocaleButton->setChecked(true);
    connect(m_latLocaleButton, &QAbstractButton::clicked, this, &IqHelpViewerMainWindow::rusificateFindString);
    ui->toolsToolBar->addWidget(m_latLocaleButton);

    m_rusLocaleButton->setText(tr("rus"));
    connect(m_rusLocaleButton, &QAbstractButton::clicked, this, &IqHelpViewerMainWindow::rusificateFindString);
    ui->toolsToolBar->addWidget(m_rusLocaleButton);

    insertToolBarBreak(ui->toolsToolBar);

    connect(ui->actionAboutQt, &QAction::triggered, this, &IqHelpViewerMainWindow::showAboutQt);
    ui->helpToolBar->addAction(ui->actionAboutQt);

    connect(ui->actionAbout, &QAction::triggered, this, &IqHelpViewerMainWindow::showAbout);
    ui->helpToolBar->addAction(ui->actionAbout);
		
		ui->helpToolBar->hide();

    //WEB view
    IqWebHelpView *webView = new IqWebHelpView(this);
    m_mainWidget->layout()->addWidget(webView->widget());
    m_helpViewWidgets << webView;

    checkBackForwardEnabled();

    //Content view
    connect(ui->contentTreeWidget, &IqContentWidget::linkClicked, this, &IqHelpViewerMainWindow::showPage);
    ui->contentTreeWidget->header()->hide();
    if (!settings.contains("contentUrl")) {
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
    if (m_rusLocaleButton->isChecked()) {
        m_findStringLineEdit->setText(rus(m_findStringLineEdit->text()));
    }
    else {
        m_findStringLineEdit->setText(lat(m_findStringLineEdit->text()));
    }
}

QString IqHelpViewerMainWindow::rus(const QString& lat) const
{
    QString latL = "1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./";
	QString latU = "!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?";
    QString rusL = tr("1234567890-=\\qwertyuiop[]asdfghjkl;'zxcvbnm,./");
    QString rusU = tr("!@#$%^&*()_+|QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>?");
	
	QString result;
    for(int i = 0; i < lat.count(); i++) {
	    QChar ch = lat.at(i);
		int lowIndex = latL.indexOf(ch);
        if (lowIndex != -1) {
		    result += rusL.at(lowIndex);
		}
        else {
		    int upIndex = latU.indexOf(ch);
            if (upIndex != -1) {
			    result += rusU.at(upIndex);
			}
            else {
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
    for(int i = 0; i < rus.count(); i++) {
        QChar ch = rus.at(i);
        int lowIndex = rusL.indexOf(ch);
        if (lowIndex != -1) {
            result += latL.at(lowIndex);
        }
        else {
            int upIndex = rusU.indexOf(ch);
            if (upIndex != -1) {
                result += latU.at(upIndex);
            }
            else {
                result += ch;
            }
        }
    }

    return result;
}

void IqHelpViewerMainWindow::showPage(const QUrl &url)
{
    qDebug() << tr("Show page \"%0\".")
                .arg(url.toString());
    IqHelpViewWidget *view = NULL;

    if (m_currentView && m_currentView->canShowPage(url)) {
        view = m_currentView;
    }
    else {
        foreach (IqHelpViewWidget *viewCandidate, m_helpViewWidgets) {
            if (!viewCandidate)
                continue;
            if (!viewCandidate->canShowPage(url))
                continue;

            view = viewCandidate;
            break;
        }
    }

    if (view) {
        setCurrentView(view);

        if (view->url() != url) {
            m_urlLineEdit->setText(url.toString());

            view->showPage(url);
        }
    }
}

void IqHelpViewerMainWindow::addUrlToHistory(const QUrl &url)
{
    qDebug() << tr("Add url \"%0\" to history.")
                .arg(url.toString());
    if (!m_backOrForward) {
        for (int i = m_currentHistoryIndex + 1; i < m_history.count(); i++) {
            m_history.removeLast();
        }
        if (m_history.count() == 0 || m_history.last() != url)
            m_history << url;
        m_currentHistoryIndex = m_history.count() - 1;
    }
    m_backOrForward = false;

    checkBackForwardEnabled();
}

void IqHelpViewerMainWindow::setCurrentView(IqHelpViewWidget *view)
{
    if (m_currentView == view)
        return;

    if (m_currentView) {
        disconnect(m_currentView, 0, this, 0);
        disconnect(ui->actionRefresh, 0, m_currentView, 0);
    }

    if (!view)
        return;

    QWidget *widget = view->widget();
    if (!widget)
        return;

    m_currentView = view;
    connect(view, &IqHelpViewWidget::linkClicked, this, &IqHelpViewerMainWindow::showPage);
    connect(view, &IqHelpViewWidget::loadProgress, this, &IqHelpViewerMainWindow::setLoadProgress);
    connect(view, &IqHelpViewWidget::loadStarted, this, &IqHelpViewerMainWindow::onLoadStarted);
    connect(view, &IqHelpViewWidget::loadFinished, this, &IqHelpViewerMainWindow::onLoadFinished);
    connect(view, &IqHelpViewWidget::linkHovered, this, &IqHelpViewerMainWindow::showLinkTitle);

    connect(ui->actionRefresh, &QAction::triggered, view, &IqHelpViewWidget::reload);

    foreach (IqHelpViewWidget *view, m_helpViewWidgets) {
        if (view && view->widget())
            view->widget()->hide();
    }

    widget->show();
}

void IqHelpViewerMainWindow::showHomePage()
{
    QSettings setting;
    if (!setting.contains("homePage")) {
        setting.setValue("homePage", "http://google.com");
    }
    QString homePage = setting.value("homePage").toString();

    showPage(homePage);
}

void IqHelpViewerMainWindow::back()
{
    if (m_currentHistoryIndex == 0)
        return;

    m_backOrForward = true;

    m_currentHistoryIndex--;
    checkBackForwardEnabled();
    showPage(m_history[m_currentHistoryIndex]);
}

void IqHelpViewerMainWindow::forward()
{
    if (m_currentHistoryIndex == m_history.count() - 1)
        return;

    m_backOrForward = true;

    m_currentHistoryIndex++;
    checkBackForwardEnabled();
    showPage(m_history[m_currentHistoryIndex]);
}

void IqHelpViewerMainWindow::setLoadProgress(const int progress)
{
    if (!m_currentView)
        return;

    ui->statusBar->showMessage(tr("Loading%0 %1%...")
                               .arg(m_currentView->title().isEmpty()?"":" \"" + m_currentView->title() + "\"")
                               .arg(progress));
}

void IqHelpViewerMainWindow::onLoadStarted()
{
    if (!m_currentView)
        return;

    ui->statusBar->showMessage(tr("Start loading%0...")
                               .arg(m_currentView->title().isEmpty()?"":" \"" + m_currentView->title() + "\""));
}

void IqHelpViewerMainWindow::onLoadFinished(bool ok)
{
    if (!ok) {
        ui->statusBar->showMessage(tr("Loading fauled!"));
        return;
    }

    if (!m_currentView)
        return;

    ui->statusBar->showMessage(tr("Lading%0 finish.")
                               .arg(m_currentView->title().isEmpty()?"":" \"" + m_currentView->title() + "\""));

    m_urlLineEdit->setText(m_currentView->url().toString());

    addUrlToHistory(m_currentView->url());
}

void IqHelpViewerMainWindow::checkBackForwardEnabled()
{
    if (m_currentHistoryIndex <= 0) {
        ui->actionBack->setEnabled(false);
    }
    else {
        ui->actionBack->setEnabled(true);
    }

    if (m_currentHistoryIndex >= m_history.count() - 1) {
        ui->actionForward->setEnabled(false);
    }
    else {
        ui->actionForward->setEnabled(true);
    }
}

void IqHelpViewerMainWindow::showLinkTitle(const QString &link, const QString &title, const QString &textContent)
{
    Q_UNUSED(title);
    if (link.isEmpty()) {
        ui->statusBar->clearMessage();
        return;
    }
    ui->statusBar->showMessage(QString("%0 - %1")
                               .arg(textContent)
                               .arg(link));
}

void IqHelpViewerMainWindow::setZoomFactor(const QString &factor)
{
    if (!m_currentView)
        return;

    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    if (factorInt < 1 || factorInt > 100000)
        return;

    m_currentView->setZoomFactor((qreal)factorInt/100);
}

void IqHelpViewerMainWindow::zoomIn()
{
    if (!m_currentView)
        return;

    QString factor = m_scaleComboBox->currentText();
    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    factorInt = factorInt + factorInt*0.25;

    if (factorInt < 1 || factorInt > 100000)
        return;

    if (factorInt == factor.toInt()) {
        m_scaleComboBox->setEditText(QString::number(factorInt * 2));
        return;
    }

    m_scaleComboBox->setEditText(QString::number(factorInt));
}

void IqHelpViewerMainWindow::zoomOut()
{
    if (!m_currentView)
        return;

    QString factor = m_scaleComboBox->currentText();
    bool ok = false;
    int factorInt = factor.toInt(&ok);
    if (!ok)
        return;

    factorInt = factorInt - factorInt*0.25;

    if (factorInt < 1 || factorInt > 100000)
        return;

    m_scaleComboBox->setEditText(QString::number(factorInt));
}

void IqHelpViewerMainWindow::findText()
{
    if (!m_currentView)
        return;

    m_currentView->findText(m_findStringLineEdit->text());
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
