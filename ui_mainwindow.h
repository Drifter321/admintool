/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAdd_Server;
    QAction *actionDark_Theme;
    QAction *actionSet_Log_Port;
    QAction *actionAbout;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTableWidget *browserTable;
    QSplitter *rulesSplitter;
    QTableWidget *rulesTable;
    QTableWidget *playerTable;
    QTabWidget *tabWidget;
    QWidget *rconTab;
    QGridLayout *gridLayout_2;
    QVBoxLayout *rconLayout;
    QHBoxLayout *rconTopLayout;
    QVBoxLayout *rconLeft;
    QLabel *rconLabel;
    QLineEdit *rconPassword;
    QCheckBox *rconSave;
    QPushButton *rconLogin;
    QPlainTextEdit *commandOutput;
    QLineEdit *commandText;
    QWidget *logTab;
    QGridLayout *gridLayout_3;
    QVBoxLayout *logLayout;
    QHBoxLayout *logtopLayout;
    QVBoxLayout *logLeftLayout;
    QPushButton *logGetLog;
    QPlainTextEdit *logOutput;
    QWidget *chatTab;
    QGridLayout *gridLayout;
    QPlainTextEdit *chatOutput;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuSettings;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(1024, 768);
        MainWindow->setMinimumSize(QSize(1024, 768));
        MainWindow->setToolButtonStyle(Qt::ToolButtonIconOnly);
        actionAdd_Server = new QAction(MainWindow);
        actionAdd_Server->setObjectName(QStringLiteral("actionAdd_Server"));
        actionAdd_Server->setCheckable(false);
        actionDark_Theme = new QAction(MainWindow);
        actionDark_Theme->setObjectName(QStringLiteral("actionDark_Theme"));
        actionDark_Theme->setCheckable(true);
        actionDark_Theme->setIconVisibleInMenu(true);
        actionSet_Log_Port = new QAction(MainWindow);
        actionSet_Log_Port->setObjectName(QStringLiteral("actionSet_Log_Port"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        splitter->setHandleWidth(5);
        browserTable = new QTableWidget(splitter);
        if (browserTable->columnCount() < 7)
            browserTable->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        browserTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        browserTable->setObjectName(QStringLiteral("browserTable"));
        browserTable->setEnabled(true);
        browserTable->setLayoutDirection(Qt::LeftToRight);
        browserTable->setFrameShape(QFrame::StyledPanel);
        browserTable->setFrameShadow(QFrame::Sunken);
        browserTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        browserTable->setProperty("showDropIndicator", QVariant(false));
        browserTable->setDragEnabled(false);
        browserTable->setDragDropOverwriteMode(false);
        browserTable->setDragDropMode(QAbstractItemView::NoDragDrop);
        browserTable->setDefaultDropAction(Qt::IgnoreAction);
        browserTable->setAlternatingRowColors(false);
        browserTable->setSelectionMode(QAbstractItemView::SingleSelection);
        browserTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        browserTable->setIconSize(QSize(0, 0));
        browserTable->setShowGrid(false);
        browserTable->setSortingEnabled(true);
        browserTable->setWordWrap(false);
        browserTable->setColumnCount(7);
        splitter->addWidget(browserTable);
        browserTable->horizontalHeader()->setHighlightSections(false);
        browserTable->horizontalHeader()->setStretchLastSection(false);
        browserTable->verticalHeader()->setVisible(false);
        browserTable->verticalHeader()->setDefaultSectionSize(20);
        browserTable->verticalHeader()->setHighlightSections(false);
        browserTable->verticalHeader()->setMinimumSectionSize(20);
        rulesSplitter = new QSplitter(splitter);
        rulesSplitter->setObjectName(QStringLiteral("rulesSplitter"));
        rulesSplitter->setBaseSize(QSize(0, 250));
        rulesSplitter->setOrientation(Qt::Horizontal);
        rulesSplitter->setHandleWidth(5);
        rulesTable = new QTableWidget(rulesSplitter);
        if (rulesTable->columnCount() < 2)
            rulesTable->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(0, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(1, __qtablewidgetitem8);
        rulesTable->setObjectName(QStringLiteral("rulesTable"));
        rulesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        rulesTable->setProperty("showDropIndicator", QVariant(false));
        rulesTable->setDragDropOverwriteMode(false);
        rulesTable->setSelectionMode(QAbstractItemView::SingleSelection);
        rulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        rulesTable->setShowGrid(false);
        rulesTable->setSortingEnabled(true);
        rulesTable->setWordWrap(false);
        rulesSplitter->addWidget(rulesTable);
        rulesTable->horizontalHeader()->setDefaultSectionSize(250);
        rulesTable->horizontalHeader()->setHighlightSections(false);
        rulesTable->verticalHeader()->setVisible(false);
        rulesTable->verticalHeader()->setDefaultSectionSize(20);
        rulesTable->verticalHeader()->setHighlightSections(false);
        rulesTable->verticalHeader()->setMinimumSectionSize(20);
        playerTable = new QTableWidget(rulesSplitter);
        if (playerTable->columnCount() < 4)
            playerTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        playerTable->setHorizontalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        playerTable->setHorizontalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        playerTable->setHorizontalHeaderItem(2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        playerTable->setHorizontalHeaderItem(3, __qtablewidgetitem12);
        playerTable->setObjectName(QStringLiteral("playerTable"));
        playerTable->setMinimumSize(QSize(100, 100));
        playerTable->setBaseSize(QSize(0, 0));
        playerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        playerTable->setProperty("showDropIndicator", QVariant(false));
        playerTable->setDragDropOverwriteMode(false);
        playerTable->setAlternatingRowColors(false);
        playerTable->setSelectionMode(QAbstractItemView::SingleSelection);
        playerTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        playerTable->setShowGrid(false);
        playerTable->setSortingEnabled(true);
        playerTable->setWordWrap(false);
        playerTable->setColumnCount(4);
        rulesSplitter->addWidget(playerTable);
        playerTable->horizontalHeader()->setHighlightSections(false);
        playerTable->horizontalHeader()->setStretchLastSection(false);
        playerTable->verticalHeader()->setVisible(false);
        playerTable->verticalHeader()->setDefaultSectionSize(20);
        playerTable->verticalHeader()->setHighlightSections(false);
        playerTable->verticalHeader()->setMinimumSectionSize(20);
        splitter->addWidget(rulesSplitter);
        tabWidget = new QTabWidget(splitter);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setTabPosition(QTabWidget::South);
        rconTab = new QWidget();
        rconTab->setObjectName(QStringLiteral("rconTab"));
        rconTab->setEnabled(true);
        gridLayout_2 = new QGridLayout(rconTab);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        rconLayout = new QVBoxLayout();
        rconLayout->setSpacing(6);
        rconLayout->setObjectName(QStringLiteral("rconLayout"));
        rconTopLayout = new QHBoxLayout();
        rconTopLayout->setSpacing(6);
        rconTopLayout->setObjectName(QStringLiteral("rconTopLayout"));
        rconLeft = new QVBoxLayout();
        rconLeft->setSpacing(6);
        rconLeft->setObjectName(QStringLiteral("rconLeft"));
        rconLabel = new QLabel(rconTab);
        rconLabel->setObjectName(QStringLiteral("rconLabel"));
        rconLabel->setEnabled(true);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(rconLabel->sizePolicy().hasHeightForWidth());
        rconLabel->setSizePolicy(sizePolicy);
        rconLabel->setBaseSize(QSize(300, 20));

        rconLeft->addWidget(rconLabel);

        rconPassword = new QLineEdit(rconTab);
        rconPassword->setObjectName(QStringLiteral("rconPassword"));
        rconPassword->setEnabled(true);
        sizePolicy.setHeightForWidth(rconPassword->sizePolicy().hasHeightForWidth());
        rconPassword->setSizePolicy(sizePolicy);
        rconPassword->setBaseSize(QSize(300, 20));
        rconPassword->setEchoMode(QLineEdit::Password);

        rconLeft->addWidget(rconPassword);

        rconSave = new QCheckBox(rconTab);
        rconSave->setObjectName(QStringLiteral("rconSave"));
        rconSave->setEnabled(true);
        sizePolicy.setHeightForWidth(rconSave->sizePolicy().hasHeightForWidth());
        rconSave->setSizePolicy(sizePolicy);

        rconLeft->addWidget(rconSave);

        rconLogin = new QPushButton(rconTab);
        rconLogin->setObjectName(QStringLiteral("rconLogin"));
        rconLogin->setEnabled(true);

        rconLeft->addWidget(rconLogin);


        rconTopLayout->addLayout(rconLeft);

        commandOutput = new QPlainTextEdit(rconTab);
        commandOutput->setObjectName(QStringLiteral("commandOutput"));
        commandOutput->setEnabled(true);
        commandOutput->setUndoRedoEnabled(false);
        commandOutput->setReadOnly(true);

        rconTopLayout->addWidget(commandOutput);


        rconLayout->addLayout(rconTopLayout);


        gridLayout_2->addLayout(rconLayout, 0, 0, 1, 1);

        commandText = new QLineEdit(rconTab);
        commandText->setObjectName(QStringLiteral("commandText"));
        commandText->setEnabled(false);

        gridLayout_2->addWidget(commandText, 1, 0, 1, 1);

        tabWidget->addTab(rconTab, QString());
        logTab = new QWidget();
        logTab->setObjectName(QStringLiteral("logTab"));
        gridLayout_3 = new QGridLayout(logTab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        logLayout = new QVBoxLayout();
        logLayout->setSpacing(6);
        logLayout->setObjectName(QStringLiteral("logLayout"));
        logtopLayout = new QHBoxLayout();
        logtopLayout->setSpacing(6);
        logtopLayout->setObjectName(QStringLiteral("logtopLayout"));
        logLeftLayout = new QVBoxLayout();
        logLeftLayout->setSpacing(6);
        logLeftLayout->setObjectName(QStringLiteral("logLeftLayout"));
        logGetLog = new QPushButton(logTab);
        logGetLog->setObjectName(QStringLiteral("logGetLog"));
        logGetLog->setEnabled(true);

        logLeftLayout->addWidget(logGetLog);


        logtopLayout->addLayout(logLeftLayout);

        logOutput = new QPlainTextEdit(logTab);
        logOutput->setObjectName(QStringLiteral("logOutput"));
        logOutput->setEnabled(true);
        logOutput->setUndoRedoEnabled(false);
        logOutput->setReadOnly(true);

        logtopLayout->addWidget(logOutput);


        logLayout->addLayout(logtopLayout);


        gridLayout_3->addLayout(logLayout, 0, 0, 2, 2);

        tabWidget->addTab(logTab, QString());
        chatTab = new QWidget();
        chatTab->setObjectName(QStringLiteral("chatTab"));
        gridLayout = new QGridLayout(chatTab);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        chatOutput = new QPlainTextEdit(chatTab);
        chatOutput->setObjectName(QStringLiteral("chatOutput"));
        chatOutput->setUndoRedoEnabled(false);
        chatOutput->setReadOnly(true);

        gridLayout->addWidget(chatOutput, 0, 0, 1, 1);

        tabWidget->addTab(chatTab, QString());
        splitter->addWidget(tabWidget);

        verticalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setEnabled(true);
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(true);
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuFile->setEnabled(true);
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QStringLiteral("menuSettings"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuSettings->menuAction());
        menuFile->addAction(actionAdd_Server);
        menuSettings->addAction(actionDark_Theme);
        menuSettings->addAction(actionSet_Log_Port);
        menuSettings->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionAdd_Server->setText(QApplication::translate("MainWindow", "Add Server", 0));
        actionDark_Theme->setText(QApplication::translate("MainWindow", "Dark Theme", 0));
        actionSet_Log_Port->setText(QApplication::translate("MainWindow", "Set Log Port", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0));
        QTableWidgetItem *___qtablewidgetitem = browserTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("MainWindow", "#", 0));
        QTableWidgetItem *___qtablewidgetitem1 = browserTable->horizontalHeaderItem(4);
        ___qtablewidgetitem1->setText(QApplication::translate("MainWindow", "Server", 0));
        QTableWidgetItem *___qtablewidgetitem2 = browserTable->horizontalHeaderItem(5);
        ___qtablewidgetitem2->setText(QApplication::translate("MainWindow", "Map", 0));
        QTableWidgetItem *___qtablewidgetitem3 = browserTable->horizontalHeaderItem(6);
        ___qtablewidgetitem3->setText(QApplication::translate("MainWindow", "Players", 0));
        QTableWidgetItem *___qtablewidgetitem4 = rulesTable->horizontalHeaderItem(0);
        ___qtablewidgetitem4->setText(QApplication::translate("MainWindow", "Rule", 0));
        QTableWidgetItem *___qtablewidgetitem5 = rulesTable->horizontalHeaderItem(1);
        ___qtablewidgetitem5->setText(QApplication::translate("MainWindow", "Value", 0));
        QTableWidgetItem *___qtablewidgetitem6 = playerTable->horizontalHeaderItem(0);
        ___qtablewidgetitem6->setText(QApplication::translate("MainWindow", "#", 0));
        QTableWidgetItem *___qtablewidgetitem7 = playerTable->horizontalHeaderItem(1);
        ___qtablewidgetitem7->setText(QApplication::translate("MainWindow", "Player Name", 0));
        QTableWidgetItem *___qtablewidgetitem8 = playerTable->horizontalHeaderItem(2);
        ___qtablewidgetitem8->setText(QApplication::translate("MainWindow", "Frags", 0));
        QTableWidgetItem *___qtablewidgetitem9 = playerTable->horizontalHeaderItem(3);
        ___qtablewidgetitem9->setText(QApplication::translate("MainWindow", "Time", 0));
        rconLabel->setText(QApplication::translate("MainWindow", "RCon Passowrd", 0));
        rconSave->setText(QApplication::translate("MainWindow", "Save Password", 0));
        rconLogin->setText(QApplication::translate("MainWindow", "Login", 0));
        tabWidget->setTabText(tabWidget->indexOf(rconTab), QApplication::translate("MainWindow", "RCon", 0));
        logGetLog->setText(QApplication::translate("MainWindow", "Get Log", 0));
        tabWidget->setTabText(tabWidget->indexOf(logTab), QApplication::translate("MainWindow", "Log", 0));
        tabWidget->setTabText(tabWidget->indexOf(chatTab), QApplication::translate("MainWindow", "Chat", 0));
        menuFile->setTitle(QApplication::translate("MainWindow", "Servers", 0));
        menuSettings->setTitle(QApplication::translate("MainWindow", "Settings", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
