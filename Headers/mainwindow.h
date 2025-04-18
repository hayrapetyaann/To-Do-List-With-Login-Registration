#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSqlDatabase>
#include <QStandardItemModel>
#include "StatusDelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class StatusDelegate;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void clickLoginButton();
    void clickRegisterButton();
    void clickForgotPasswordButton();
    void logout();
    void addTask();
    void editTask(int id, const QString& newTask);
    void removeSelectedTasks();
    void filterTasks(const QString& status = "");
    void clearTasks();
    void exitApp();
private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QStandardItemModel *model;
    QString currentUsername;
    QString hashPassword(const QString& password);
    bool registerUser(const QString& username, const QString& password);
    bool loginUser(const QString& username, const QString& password);
    void setupTodoListUI();
    void applyButtonStyle();
    void setupTaskModel();
    int findNextAvailableId();
};

#endif // MAINWINDOW_H
