#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), currentUsername("") {
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to connect to database: " + db.lastError().text());
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password_ TEXT NOT NULL)")) {
        QMessageBox::critical(this, "Error", "Failed to create users table: " + query.lastError().text());
        return;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL, task TEXT NOT NULL, status TEXT NOT NULL, date TEXT NOT NULL, FOREIGN KEY(username) REFERENCES users(username))")) {
        QMessageBox::critical(this, "Error", "Failed to create tasks table: " + query.lastError().text());
        return;#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), currentUsername("") {
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to connect to database: " + db.lastError().text());
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password_ TEXT NOT NULL)")) {
        QMessageBox::critical(this, "Error", "Failed to create users table: " + query.lastError().text());
        return;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL, task TEXT NOT NULL, status TEXT NOT NULL, date TEXT NOT NULL, FOREIGN KEY(username) REFERENCES users(username))")) {
        QMessageBox::critical(this, "Error", "Failed to create tasks table: " + query.lastError().text());
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit->setPlaceholderText("Enter your username");
    ui->lineEdit_2->setPlaceholderText("Enter your password");
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    this->setStyleSheet("background-color: #2C3E50;");
    ui->lineEdit->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    ui->lineEdit_2->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    QString btnStyle = "QPushButton { border-radius: 20px; background-color: #2ECC71; color: white; font-weight: bold; } QPushButton:hover { background-color: #27AE60; }";
    ui->pushButton->setStyleSheet(btnStyle);
    ui->pushButton_2->setStyleSheet(btnStyle);
    ui->pushButton_3->setStyleSheet(
        "QPushButton { border: none; color: #95A5A6; text-decoration: underline; } QPushButton:hover { color: #ECF0F1; }"
    );

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::clickLoginButton);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::clickRegisterButton);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::clickForgotPasswordButton);

    setupTodoListUI();
}

void MainWindow::setupTodoListUI() {
    ui->tableView->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #eeeeee;
            padding: 4px;
            border: none;
            font-weight: bold;
        }
        QTableView {
            border: 1px solid #ccc;
            background-color: #FAFAFA;
            alternate-background-color: #F0F0F0;
            gridline-color: transparent;
            selection-background-color: #B3E5FC;
            selection-color: black;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
        }
        QTableView::item {
            padding: 6px;
        }
    )");

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::exitApp);
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=]() { filterTasks("To Do"); });
    connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() { filterTasks("In Progress"); });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() { filterTasks("Ready"); });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [=]() { filterTasks(); });
    connect(ui->pushButton_2_todo, &QPushButton::clicked, this, &MainWindow::removeSelectedTasks);
    connect(ui->pushButton_3_todo, &QPushButton::clicked, this, &MainWindow::clearTasks);
    connect(ui->pushButton_todo, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->pushButton_logout, &QPushButton::clicked, this, &MainWindow::logout);

    applyButtonStyle();
}

void MainWindow::applyButtonStyle() {
    QStringList styles = {
        "QPushButton { background-color: #81D4FA; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #4FC3F7; }",
        "QPushButton { background-color: #FFD54F; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #FFCA28; }",
        "QPushButton { background-color: #81C784; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #66BB6A; }",
        "QPushButton { background-color: #E0E0E0; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #BDBDBD; }",
        "QPushButton { background-color: #E57373; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #EF5350; }",
        "QPushButton { background-color: #BDBDBD; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #9E9E9E; }",
        "QPushButton { background-color: #EF5350; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #E53935; }",
        "QPushButton { background-color: #4A90E2; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #357ABD; }",
        "QPushButton { background-color: #FF7043; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #F4511E; }"
    };

    QList<QPushButton*> buttons = {
        ui->pushButton_5,
        ui->pushButton_6,
        ui->pushButton_7,
        ui->pushButton_9,
        ui->pushButton_2_todo,
        ui->pushButton_3_todo,
        ui->pushButton_4,
        ui->pushButton_todo,
        ui->pushButton_logout
    };

    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setStyleSheet(styles[i]);
    }
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
    if (model) {
        delete model;
    }
}

QString MainWindow::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool MainWindow::registerUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9]{4,}$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username must be at least 4 characters and contain only letters and numbers.");
        return false;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Error",
                             "Password must be at least 6 characters long and include:\n"
                             "- One uppercase letter\n"
                             "- One lowercase letter\n"
                             "- One number");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + query.lastError().text());
        return false;
    }
    if (query.next()) {
        QMessageBox::warning(this, "Error", "Username already exists");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password_) VALUES (:username, :password_)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password_", hashPassword(password));
    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Error", "Registration failed: " + insertQuery.lastError().text());
        return false;
    }

    QMessageBox::information(this, "Success", "Registration successful");
    return true;
}

bool MainWindow::loginUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QSqlQuery loginQuery(db);
    loginQuery.prepare("SELECT password_ FROM users WHERE username = :username");
    loginQuery.bindValue(":username", username);
    if (!loginQuery.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + loginQuery.lastError().text());
        return false;
    }

    if (loginQuery.next()) {
        QString storedPassword = loginQuery.value(0).toString();
        if (storedPassword == hashPassword(password)) {
            currentUsername = username;
            setupTaskModel();
            ui->stackedWidget->setCurrentIndex(1);
            QMessageBox::information(this, "Success", "Login successful");
            return true;
        }
    }

    QMessageBox::warning(this, "Error", "Invalid username or password");
    return false;
}

void MainWindow::clickForgotPasswordButton() {
    QString username = ui->lineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your username.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_ FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Error checking username: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        bool ok;
        QString newPassword = QInputDialog::getText(this, "Reset Password",
                                                   "Enter new password (at least 6 characters, 1 uppercase, 1 number):",
                                                   QLineEdit::Password, "", &ok);
        if (ok && !newPassword.isEmpty()) {
            QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
            if (!passwordRegex.match(newPassword).hasMatch()) {
                QMessageBox::warning(this, "Error",
                                     "Password must be at least 6 characters long and include:\n"
                                     "- One uppercase letter\n"
                                     "- One lowercase letter\n"
                                     "- One number");
                return;
            }

            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET password_ = :password WHERE username = :username");
            updateQuery.bindValue(":password", hashPassword(newPassword));
            updateQuery.bindValue(":username", username);
            if (!updateQuery.exec()) {
                QMessageBox::critical(this, "Error", "Failed to update password: " + updateQuery.lastError().text());
                return;
            }

            QMessageBox::information(this, "Success", "Your password has been reset successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Invalid password entered.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Username not found.");
    }
}

void MainWindow::clickRegisterButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    registerUser(username, password);
}

void MainWindow::clickLoginButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    loginUser(username, password);
}

void MainWindow::logout() {
    currentUsername = "";
    if (model) {
        delete model;
        model = nullptr;
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupTaskModel() {
    if (model) {
        delete model;
    }
    model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Task");
    model->setHeaderData(2, Qt::Horizontal, "Status");
    model->setHeaderData(3, Qt::Horizontal, "Date");

    QSqlQuery query(db);
    query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString task = query.value(1).toString();
            QString status = query.value(2).toString();
            QString date = query.value(3).toString();

            QStandardItem* idItem = new QStandardItem(QString::number(id));
            idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QStandardItem* taskItem = new QStandardItem(task);
            QStandardItem* statusItem = new QStandardItem(status);
            QStandardItem* dateItem = new QStandardItem(date);
            dateItem->setEditable(false);

            if (status.toLower() == "ready") {
                statusItem->setBackground(QColor("#A5D6A7"));
                taskItem->setForeground(QBrush(Qt::gray));
                QFont font = taskItem->font();
                font.setStrikeOut(true);
                taskItem->setFont(font);
            } else {
                statusItem->setBackground(QColor("#E0E0E0"));
            }

            model->appendRow({idItem, taskItem, statusItem, dateItem});
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(2, new StatusDelegate(this));

    connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem *item) {
        int row = item->row();
        int id = model->item(row, 0)->text().toInt();

        if (item->column() == 1) {
            QString newTask = item->text();
            editTask(id, newTask);
        }

        if (item->column() == 2) {
            QString newStatus = item->text();
            QSqlQuery query(db);
            query.prepare("UPDATE tasks SET status = ? WHERE id = ? AND username = ?");
            query.addBindValue(newStatus);
            query.addBindValue(id);
            query.addBindValue(currentUsername);
            if (!query.exec()) {
                qDebug() << "Failed to update status:" << query.lastError().text();
            }

            QString status = newStatus.toLower();
            if (status == "ready") {
                item->setBackground(QColor("#A5D6A7"));
                model->item(row, 1)->setForeground(QBrush(Qt::gray));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(true);
                model->item(row, 1)->setFont(font);
            } else {
                item->setBackground(QColor("#E0E0E0"));
                model->item(row, 1)->setForeground(QBrush(Qt::black));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(false);
                model->item(row, 1)->setFont(font);
            }
        }
    });

    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->setAlternatingRowColors(true);
}

int MainWindow::findNextAvailableId() {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM tasks WHERE username = ? ORDER BY id ASC");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to fetch IDs:" << query.lastError().text();
        return 1;
    }

    int expectedId = 1;
    while (query.next()) {
        int currentId = query.value(0).toInt();
        if (currentId != expectedId) {
            break;
        }
        expectedId++;
    }
    return expectedId;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit_todo->text().trimmed();
    if (taskText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Task cannot be empty.");
        return;
    }

    QSqlQuery query(db);
    QString status = "To Do";
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int newId = findNextAvailableId();

    query.prepare("INSERT INTO tasks (id, username, task, status, date) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(newId);
    query.addBindValue(currentUsername);
    query.addBindValue(taskText);
    query.addBindValue(status);
    query.addBindValue(dateStr);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Could not insert task.");
        return;
    }

    QStandardItem* idItem = new QStandardItem(QString::number(newId));
    idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QStandardItem* taskItem = new QStandardItem(taskText);
    taskItem->setEditable(true);
    QStandardItem* statusItem = new QStandardItem("To Do");
    statusItem->setEditable(true);
    statusItem->setBackground(QColor("#E0E0E0"));
    QStandardItem* dateItem = new QStandardItem(dateStr);
    dateItem->setEditable(false);

    model->appendRow({idItem, taskItem, statusItem, dateItem});
    ui->lineEdit_todo->clear();
}

void MainWindow::editTask(int id, const QString& newTask) {
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET task = ? WHERE id = ? AND username = ?");
    query.addBindValue(newTask);
    query.addBindValue(id);
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to update task name:" << query.lastError().text();
        QMessageBox::warning(this, "Update Error", "Failed to update the task name.");
    }
}

void MainWindow::removeSelectedTasks() {
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select a task to remove.");
        return;
    }

    std::sort(selected.begin(), selected.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : selected) {
        int id = model->item(index.row(), 0)->text().toInt();
        QSqlQuery query(db);
        query.prepare("DELETE FROM tasks WHERE id = ? AND username = ?");
        query.addBindValue(id);
        query.addBindValue(currentUsername);
        if (!query.exec()) {
            qDebug() << "Delete error:" << query.lastError().text();
        }
        model->removeRow(index.row());
    }
}

void MainWindow::filterTasks(const QString& status) {
    model->removeRows(0, model->rowCount());

    QSqlQuery query(db);
    if (status.isEmpty()) {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
        query.addBindValue(currentUsername);
    } else {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE status = ? AND username = ?");
        query.addBindValue(status);
        query.addBindValue(currentUsername);
    }

    if (!query.exec()) {
        qDebug() << "Failed to fetch tasks:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString task = query.value(1).toString();
        QString stat = query.value(2).toString();
        QString date = query.value(3).toString();

        QStandardItem* idItem = new QStandardItem(QString::number(id));
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QStandardItem* taskItem = new QStandardItem(task);
        QStandardItem* statusItem = new QStandardItem(stat);
        QStandardItem* dateItem = new QStandardItem(date);
        dateItem->setEditable(false);

        if (stat.toLower() == "ready") {
            statusItem->setBackground(QColor("#A5D6A7"));
            taskItem->setForeground(QBrush(Qt::gray));
            QFont font = taskItem->font();
            font.setStrikeOut(true);
            taskItem->setFont(font);
        } else {
            statusItem->setBackground(QColor("#E0E0E0"));
        }

        model->appendRow({idItem, taskItem, statusItem, dateItem});
    }
}

void MainWindow::clearTasks() {
    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Clear tasks error:" << query.lastError().text();
    }
    model->removeRows(0, model->rowCount());
}

void MainWindow::exitApp() {
    QApplication::quit();
}
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), currentUsername("") {
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to connect to database: " + db.lastError().text());
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password_ TEXT NOT NULL)")) {
        QMessageBox::critical(this, "Error", "Failed to create users table: " + query.lastError().text());
        return;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL, task TEXT NOT NULL, status TEXT NOT NULL, date TEXT NOT NULL, FOREIGN KEY(username) REFERENCES users(username))")) {
        QMessageBox::critical(this, "Error", "Failed to create tasks table: " + query.lastError().text());
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit->setPlaceholderText("Enter your username");
    ui->lineEdit_2->setPlaceholderText("Enter your password");
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    this->setStyleSheet("background-color: #2C3E50;");
    ui->lineEdit->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    ui->lineEdit_2->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    QString btnStyle = "QPushButton { border-radius: 20px; background-color: #2ECC71; color: white; font-weight: bold; } QPushButton:hover { background-color: #27AE60; }";
    ui->pushButton->setStyleSheet(btnStyle);
    ui->pushButton_2->setStyleSheet(btnStyle);
    ui->pushButton_3->setStyleSheet(
        "QPushButton { border: none; color: #95A5A6; text-decoration: underline; } QPushButton:hover { color: #ECF0F1; }"
    );

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::clickLoginButton);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::clickRegisterButton);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::clickForgotPasswordButton);

    setupTodoListUI();
}

void MainWindow::setupTodoListUI() {
    ui->tableView->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #eeeeee;
            padding: 4px;
            border: none;
            font-weight: bold;
        }
        QTableView {
            border: 1px solid #ccc;
            background-color: #FAFAFA;
            alternate-background-color: #F0F0F0;
            gridline-color: transparent;
            selection-background-color: #B3E5FC;
            selection-color: black;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
        }
        QTableView::item {
            padding: 6px;
        }
    )");

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::exitApp);
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=]() { filterTasks("To Do"); });
    connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() { filterTasks("In Progress"); });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() { filterTasks("Ready"); });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [=]() { filterTasks(); });
    connect(ui->pushButton_2_todo, &QPushButton::clicked, this, &MainWindow::removeSelectedTasks);
    connect(ui->pushButton_3_todo, &QPushButton::clicked, this, &MainWindow::clearTasks);
    connect(ui->pushButton_todo, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->pushButton_logout, &QPushButton::clicked, this, &MainWindow::logout);

    applyButtonStyle();
}

void MainWindow::applyButtonStyle() {
    QStringList styles = {
        "QPushButton { background-color: #81D4FA; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #4FC3F7; }",
        "QPushButton { background-color: #FFD54F; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #FFCA28; }",
        "QPushButton { background-color: #81C784; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #66BB6A; }",
        "QPushButton { background-color: #E0E0E0; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #BDBDBD; }",
        "QPushButton { background-color: #E57373; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #EF5350; }",
        "QPushButton { background-color: #BDBDBD; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #9E9E9E; }",
        "QPushButton { background-color: #EF5350; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #E53935; }",
        "QPushButton { background-color: #4A90E2; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #357ABD; }",
        "QPushButton { background-color: #FF7043; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #F4511E; }"
    };

    QList<QPushButton*> buttons = {
        ui->pushButton_5,
        ui->pushButton_6,
        ui->pushButton_7,
        ui->pushButton_9,
        ui->pushButton_2_todo,
        ui->pushButton_3_todo,
        ui->pushButton_4,
        ui->pushButton_todo,
        ui->pushButton_logout
    };

    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setStyleSheet(styles[i]);
    }
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
    if (model) {
        delete model;
    }
}

QString MainWindow::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool MainWindow::registerUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9]{4,}$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username must be at least 4 characters and contain only letters and numbers.");
        return false;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Error",
                             "Password must be at least 6 characters long and include:\n"
                             "- One uppercase letter\n"
                             "- One lowercase letter\n"
                             "- One number");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + query.lastError().text());
        return false;
    }
    if (query.next()) {
        QMessageBox::warning(this, "Error", "Username already exists");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password_) VALUES (:username, :password_)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password_", hashPassword(password));
    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Error", "Registration failed: " + insertQuery.lastError().text());
        return false;
    }

    QMessageBox::information(this, "Success", "Registration successful");
    return true;
}

bool MainWindow::loginUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QSqlQuery loginQuery(db);
    loginQuery.prepare("SELECT password_ FROM users WHERE username = :username");
    loginQuery.bindValue(":username", username);
    if (!loginQuery.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + loginQuery.lastError().text());
        return false;
    }

    if (loginQuery.next()) {
        QString storedPassword = loginQuery.value(0).toString();
        if (storedPassword == hashPassword(password)) {
            currentUsername = username;
            setupTaskModel();
            ui->stackedWidget->setCurrentIndex(1);
            QMessageBox::information(this, "Success", "Login successful");
            return true;
        }
    }

    QMessageBox::warning(this, "Error", "Invalid username or password");
    return false;
}

void MainWindow::clickForgotPasswordButton() {
    QString username = ui->lineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your username.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_ FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Error checking username: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        bool ok;
        QString newPassword = QInputDialog::getText(this, "Reset Password",
                                                   "Enter new password (at least 6 characters, 1 uppercase, 1 number):",
                                                   QLineEdit::Password, "", &ok);
        if (ok && !newPassword.isEmpty()) {
            QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
            if (!passwordRegex.match(newPassword).hasMatch()) {
                QMessageBox::warning(this, "Error",
                                     "Password must be at least 6 characters long and include:\n"
                                     "- One uppercase letter\n"
                                     "- One lowercase letter\n"
                                     "- One number");
                return;
            }

            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET password_ = :password WHERE username = :username");
            updateQuery.bindValue(":password", hashPassword(newPassword));
            updateQuery.bindValue(":username", username);
            if (!updateQuery.exec()) {
                QMessageBox::critical(this, "Error", "Failed to update password: " + updateQuery.lastError().text());
                return;
            }

            QMessageBox::information(this, "Success", "Your password has been reset successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Invalid password entered.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Username not found.");
    }
}

void MainWindow::clickRegisterButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    registerUser(username, password);
}

void MainWindow::clickLoginButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    loginUser(username, password);
}

void MainWindow::logout() {
    currentUsername = "";
    if (model) {
        delete model;
        model = nullptr;
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupTaskModel() {
    if (model) {
        delete model;
    }
    model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Task");
    model->setHeaderData(2, Qt::Horizontal, "Status");
    model->setHeaderData(3, Qt::Horizontal, "Date");

    QSqlQuery query(db);
    query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString task = query.value(1).toString();
            QString status = query.value(2).toString();
            QString date = query.value(3).toString();

            QStandardItem* idItem = new QStandardItem(QString::number(id));
            idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QStandardItem* taskItem = new QStandardItem(task);
            QStandardItem* statusItem = new QStandardItem(status);
            QStandardItem* dateItem = new QStandardItem(date);
            dateItem->setEditable(false);

            if (status.toLower() == "ready") {
                statusItem->setBackground(QColor("#A5D6A7"));
                taskItem->setForeground(QBrush(Qt::gray));
                QFont font = taskItem->font();
                font.setStrikeOut(true);
                taskItem->setFont(font);
            } else {
                statusItem->setBackground(QColor("#E0E0E0"));
            }

            model->appendRow({idItem, taskItem, statusItem, dateItem});
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(2, new StatusDelegate(this));

    connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem *item) {
        int row = item->row();
        int id = model->item(row, 0)->text().toInt();

        if (item->column() == 1) {
            QString newTask = item->text();
            editTask(id, newTask);
        }

        if (item->column() == 2) {
            QString newStatus = item->text();
            QSqlQuery query(db);
            query.prepare("UPDATE tasks SET status = ? WHERE id = ? AND username = ?");
            query.addBindValue(newStatus);
            query.addBindValue(id);
            query.addBindValue(currentUsername);
            if (!query.exec()) {
                qDebug() << "Failed to update status:" << query.lastError().text();
            }

            QString status = newStatus.toLower();
            if (status == "ready") {
                item->setBackground(QColor("#A5D6A7"));
                model->item(row, 1)->setForeground(QBrush(Qt::gray));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(true);
                model->item(row, 1)->setFont(font);
            } else {
                item->setBackground(QColor("#E0E0E0"));
                model->item(row, 1)->setForeground(QBrush(Qt::black));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(false);
                model->item(row, 1)->setFont(font);
            }
        }
    });

    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->setAlternatingRowColors(true);
}

int MainWindow::findNextAvailableId() {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM tasks WHERE username = ? ORDER BY id ASC");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to fetch IDs:" << query.lastError().text();
        return 1;
    }

    int expectedId = 1;
    while (query.next()) {
        int currentId = query.value(0).toInt();
        if (currentId != expectedId) {
            break;
        }
        expectedId++;
    }
    return expectedId;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit_todo->text().trimmed();
    if (taskText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Task cannot be empty.");
        return;
    }

    QSqlQuery query(db);
    QString status = "To Do";
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int newId = findNextAvailableId();

    query.prepare("INSERT INTO tasks (id, username, task, status, date) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(newId);
    query.addBindValue(currentUsername);
    query.addBindValue(taskText);
    query.addBindValue(status);
    query.addBindValue(dateStr);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Could not insert task.");
        return;
    }

    QStandardItem* idItem = new QStandardItem(QString::number(newId));
    idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QStandardItem* taskItem = new QStandardItem(taskText);
    taskItem->setEditable(true);
    QStandardItem* statusItem = new QStandardItem("To Do");
    statusItem->setEditable(true);
    statusItem->setBackground(QColor("#E0E0E0"));
    QStandardItem* dateItem = new QStandardItem(dateStr);
    dateItem->setEditable(false);

    model->appendRow({idItem, taskItem, statusItem, dateItem});
    ui->lineEdit_todo->clear();
}

void MainWindow::editTask(int id, const QString& newTask) {
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET task = ? WHERE id = ? AND username = ?");
    query.addBindValue(newTask);
    query.addBindValue(id);
    query.addBindValue(currentUsername);
    if (!query.exec()) {#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QDateTime>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), model(nullptr), currentUsername("") {
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("app.db");
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to connect to database: " + db.lastError().text());
        return;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password_ TEXT NOT NULL)")) {
        QMessageBox::critical(this, "Error", "Failed to create users table: " + query.lastError().text());
        return;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS tasks (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT NOT NULL, task TEXT NOT NULL, status TEXT NOT NULL, date TEXT NOT NULL, FOREIGN KEY(username) REFERENCES users(username))")) {
        QMessageBox::critical(this, "Error", "Failed to create tasks table: " + query.lastError().text());
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit->setPlaceholderText("Enter your username");
    ui->lineEdit_2->setPlaceholderText("Enter your password");
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    this->setStyleSheet("background-color: #2C3E50;");
    ui->lineEdit->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    ui->lineEdit_2->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    QString btnStyle = "QPushButton { border-radius: 20px; background-color: #2ECC71; color: white; font-weight: bold; } QPushButton:hover { background-color: #27AE60; }";
    ui->pushButton->setStyleSheet(btnStyle);
    ui->pushButton_2->setStyleSheet(btnStyle);
    ui->pushButton_3->setStyleSheet(
        "QPushButton { border: none; color: #95A5A6; text-decoration: underline; } QPushButton:hover { color: #ECF0F1; }"
    );

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::clickLoginButton);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::clickRegisterButton);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::clickForgotPasswordButton);

    setupTodoListUI();
}

void MainWindow::setupTodoListUI() {
    ui->tableView->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #eeeeee;
            padding: 4px;
            border: none;
            font-weight: bold;
        }
        QTableView {
            border: 1px solid #ccc;
            background-color: #FAFAFA;
            alternate-background-color: #F0F0F0;
            gridline-color: transparent;
            selection-background-color: #B3E5FC;
            selection-color: black;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
        }
        QTableView::item {
            padding: 6px;
        }
    )");

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::exitApp);
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=]() { filterTasks("To Do"); });
    connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() { filterTasks("In Progress"); });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() { filterTasks("Ready"); });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [=]() { filterTasks(); });
    connect(ui->pushButton_2_todo, &QPushButton::clicked, this, &MainWindow::removeSelectedTasks);
    connect(ui->pushButton_3_todo, &QPushButton::clicked, this, &MainWindow::clearTasks);
    connect(ui->pushButton_todo, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->pushButton_logout, &QPushButton::clicked, this, &MainWindow::logout);

    applyButtonStyle();
}

void MainWindow::applyButtonStyle() {
    QStringList styles = {
        "QPushButton { background-color: #81D4FA; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #4FC3F7; }",
        "QPushButton { background-color: #FFD54F; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #FFCA28; }",
        "QPushButton { background-color: #81C784; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #66BB6A; }",
        "QPushButton { background-color: #E0E0E0; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #BDBDBD; }",
        "QPushButton { background-color: #E57373; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #EF5350; }",
        "QPushButton { background-color: #BDBDBD; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #9E9E9E; }",
        "QPushButton { background-color: #EF5350; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #E53935; }",
        "QPushButton { background-color: #4A90E2; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #357ABD; }",
        "QPushButton { background-color: #FF7043; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #F4511E; }"
    };

    QList<QPushButton*> buttons = {
        ui->pushButton_5,
        ui->pushButton_6,
        ui->pushButton_7,
        ui->pushButton_9,
        ui->pushButton_2_todo,
        ui->pushButton_3_todo,
        ui->pushButton_4,
        ui->pushButton_todo,
        ui->pushButton_logout
    };

    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setStyleSheet(styles[i]);
    }
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
    if (model) {
        delete model;
    }
}

QString MainWindow::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool MainWindow::registerUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9]{4,}$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username must be at least 4 characters and contain only letters and numbers.");
        return false;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Error",
                             "Password must be at least 6 characters long and include:\n"
                             "- One uppercase letter\n"
                             "- One lowercase letter\n"
                             "- One number");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + query.lastError().text());
        return false;
    }
    if (query.next()) {
        QMessageBox::warning(this, "Error", "Username already exists");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password_) VALUES (:username, :password_)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password_", hashPassword(password));
    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Error", "Registration failed: " + insertQuery.lastError().text());
        return false;
    }

    QMessageBox::information(this, "Success", "Registration successful");
    return true;
}

bool MainWindow::loginUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QSqlQuery loginQuery(db);
    loginQuery.prepare("SELECT password_ FROM users WHERE username = :username");
    loginQuery.bindValue(":username", username);
    if (!loginQuery.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + loginQuery.lastError().text());
        return false;
    }

    if (loginQuery.next()) {
        QString storedPassword = loginQuery.value(0).toString();
        if (storedPassword == hashPassword(password)) {
            currentUsername = username;
            setupTaskModel();
            ui->stackedWidget->setCurrentIndex(1);
            QMessageBox::information(this, "Success", "Login successful");
            return true;
        }
    }

    QMessageBox::warning(this, "Error", "Invalid username or password");
    return false;
}

void MainWindow::clickForgotPasswordButton() {
    QString username = ui->lineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your username.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_ FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Error checking username: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        bool ok;
        QString newPassword = QInputDialog::getText(this, "Reset Password",
                                                   "Enter new password (at least 6 characters, 1 uppercase, 1 number):",
                                                   QLineEdit::Password, "", &ok);
        if (ok && !newPassword.isEmpty()) {
            QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
            if (!passwordRegex.match(newPassword).hasMatch()) {
                QMessageBox::warning(this, "Error",
                                     "Password must be at least 6 characters long and include:\n"
                                     "- One uppercase letter\n"
                                     "- One lowercase letter\n"
                                     "- One number");
                return;
            }

            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET password_ = :password WHERE username = :username");
            updateQuery.bindValue(":password", hashPassword(newPassword));
            updateQuery.bindValue(":username", username);
            if (!updateQuery.exec()) {
                QMessageBox::critical(this, "Error", "Failed to update password: " + updateQuery.lastError().text());
                return;
            }

            QMessageBox::information(this, "Success", "Your password has been reset successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Invalid password entered.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Username not found.");
    }
}

void MainWindow::clickRegisterButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    registerUser(username, password);
}

void MainWindow::clickLoginButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    loginUser(username, password);
}

void MainWindow::logout() {
    currentUsername = "";
    if (model) {
        delete model;
        model = nullptr;
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupTaskModel() {
    if (model) {
        delete model;
    }
    model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Task");
    model->setHeaderData(2, Qt::Horizontal, "Status");
    model->setHeaderData(3, Qt::Horizontal, "Date");

    QSqlQuery query(db);
    query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString task = query.value(1).toString();
            QString status = query.value(2).toString();
            QString date = query.value(3).toString();

            QStandardItem* idItem = new QStandardItem(QString::number(id));
            idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QStandardItem* taskItem = new QStandardItem(task);
            QStandardItem* statusItem = new QStandardItem(status);
            QStandardItem* dateItem = new QStandardItem(date);
            dateItem->setEditable(false);

            if (status.toLower() == "ready") {
                statusItem->setBackground(QColor("#A5D6A7"));
                taskItem->setForeground(QBrush(Qt::gray));
                QFont font = taskItem->font();
                font.setStrikeOut(true);
                taskItem->setFont(font);
            } else {
                statusItem->setBackground(QColor("#E0E0E0"));
            }

            model->appendRow({idItem, taskItem, statusItem, dateItem});
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(2, new StatusDelegate(this));

    connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem *item) {
        int row = item->row();
        int id = model->item(row, 0)->text().toInt();

        if (item->column() == 1) {
            QString newTask = item->text();
            editTask(id, newTask);
        }

        if (item->column() == 2) {
            QString newStatus = item->text();
            QSqlQuery query(db);
            query.prepare("UPDATE tasks SET status = ? WHERE id = ? AND username = ?");
            query.addBindValue(newStatus);
            query.addBindValue(id);
            query.addBindValue(currentUsername);
            if (!query.exec()) {
                qDebug() << "Failed to update status:" << query.lastError().text();
            }

            QString status = newStatus.toLower();
            if (status == "ready") {
                item->setBackground(QColor("#A5D6A7"));
                model->item(row, 1)->setForeground(QBrush(Qt::gray));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(true);
                model->item(row, 1)->setFont(font);
            } else {
                item->setBackground(QColor("#E0E0E0"));
                model->item(row, 1)->setForeground(QBrush(Qt::black));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(false);
                model->item(row, 1)->setFont(font);
            }
        }
    });

    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->setAlternatingRowColors(true);
}

int MainWindow::findNextAvailableId() {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM tasks WHERE username = ? ORDER BY id ASC");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to fetch IDs:" << query.lastError().text();
        return 1;
    }

    int expectedId = 1;
    while (query.next()) {
        int currentId = query.value(0).toInt();
        if (currentId != expectedId) {
            break;
        }
        expectedId++;
    }
    return expectedId;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit_todo->text().trimmed();
    if (taskText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Task cannot be empty.");
        return;
    }

    QSqlQuery query(db);
    QString status = "To Do";
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int newId = findNextAvailableId();

    query.prepare("INSERT INTO tasks (id, username, task, status, date) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(newId);
    query.addBindValue(currentUsername);
    query.addBindValue(taskText);
    query.addBindValue(status);
    query.addBindValue(dateStr);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Could not insert task.");
        return;
    }

    QStandardItem* idItem = new QStandardItem(QString::number(newId));
    idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QStandardItem* taskItem = new QStandardItem(taskText);
    taskItem->setEditable(true);
    QStandardItem* statusItem = new QStandardItem("To Do");
    statusItem->setEditable(true);
    statusItem->setBackground(QColor("#E0E0E0"));
    QStandardItem* dateItem = new QStandardItem(dateStr);
    dateItem->setEditable(false);

    model->appendRow({idItem, taskItem, statusItem, dateItem});
    ui->lineEdit_todo->clear();
}

void MainWindow::editTask(int id, const QString& newTask) {
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET task = ? WHERE id = ? AND username = ?");
    query.addBindValue(newTask);
    query.addBindValue(id);
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to update task name:" << query.lastError().text();
        QMessageBox::warning(this, "Update Error", "Failed to update the task name.");
    }
}

void MainWindow::removeSelectedTasks() {
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select a task to remove.");
        return;
    }

    std::sort(selected.begin(), selected.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : selected) {
        int id = model->item(index.row(), 0)->text().toInt();
        QSqlQuery query(db);
        query.prepare("DELETE FROM tasks WHERE id = ? AND username = ?");
        query.addBindValue(id);
        query.addBindValue(currentUsername);
        if (!query.exec()) {
            qDebug() << "Delete error:" << query.lastError().text();
        }
        model->removeRow(index.row());
    }
}

void MainWindow::filterTasks(const QString& status) {
    model->removeRows(0, model->rowCount());

    QSqlQuery query(db);
    if (status.isEmpty()) {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
        query.addBindValue(currentUsername);
    } else {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE status = ? AND username = ?");
        query.addBindValue(status);
        query.addBindValue(currentUsername);
    }

    if (!query.exec()) {
        qDebug() << "Failed to fetch tasks:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString task = query.value(1).toString();
        QString stat = query.value(2).toString();
        QString date = query.value(3).toString();

        QStandardItem* idItem = new QStandardItem(QString::number(id));
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QStandardItem* taskItem = new QStandardItem(task);
        QStandardItem* statusItem = new QStandardItem(stat);
        QStandardItem* dateItem = new QStandardItem(date);
        dateItem->setEditable(false);

        if (stat.toLower() == "ready") {
            statusItem->setBackground(QColor("#A5D6A7"));
            taskItem->setForeground(QBrush(Qt::gray));
            QFont font = taskItem->font();
            font.setStrikeOut(true);
            taskItem->setFont(font);
        } else {
            statusItem->setBackground(QColor("#E0E0E0"));
        }

        model->appendRow({idItem, taskItem, statusItem, dateItem});
    }
}

void MainWindow::clearTasks() {
    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Clear tasks error:" << query.lastError().text();
    }
    model->removeRows(0, model->rowCount());
}

void MainWindow::exitApp() {
    QApplication::quit();
}

        qDebug() << "Failed to update task name:" << query.lastError().text();
        QMessageBox::warning(this, "Update Error", "Failed to update the task name.");
    }
}

void MainWindow::removeSelectedTasks() {
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select a task to remove.");
        return;
    }

    std::sort(selected.begin(), selected.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : selected) {
        int id = model->item(index.row(), 0)->text().toInt();
        QSqlQuery query(db);
        query.prepare("DELETE FROM tasks WHERE id = ? AND username = ?");
        query.addBindValue(id);
        query.addBindValue(currentUsername);
        if (!query.exec()) {
            qDebug() << "Delete error:" << query.lastError().text();
        }
        model->removeRow(index.row());
    }
}

void MainWindow::filterTasks(const QString& status) {
    model->removeRows(0, model->rowCount());

    QSqlQuery query(db);
    if (status.isEmpty()) {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
        query.addBindValue(currentUsername);
    } else {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE status = ? AND username = ?");
        query.addBindValue(status);
        query.addBindValue(currentUsername);
    }

    if (!query.exec()) {
        qDebug() << "Failed to fetch tasks:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString task = query.value(1).toString();
        QString stat = query.value(2).toString();
        QString date = query.value(3).toString();

        QStandardItem* idItem = new QStandardItem(QString::number(id));
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QStandardItem* taskItem = new QStandardItem(task);
        QStandardItem* statusItem = new QStandardItem(stat);
        QStandardItem* dateItem = new QStandardItem(date);
        dateItem->setEditable(false);

        if (stat.toLower() == "ready") {
            statusItem->setBackground(QColor("#A5D6A7"));
            taskItem->setForeground(QBrush(Qt::gray));
            QFont font = taskItem->font();
            font.setStrikeOut(true);
            taskItem->setFont(font);
        } else {
            statusItem->setBackground(QColor("#E0E0E0"));
        }

        model->appendRow({idItem, taskItem, statusItem, dateItem});
    }
}

void MainWindow::clearTasks() {
    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Clear tasks error:" << query.lastError().text();
    }
    model->removeRows(0, model->rowCount());
}

void MainWindow::exitApp() {
    QApplication::quit();
}

    }

    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit->setPlaceholderText("Enter your username");
    ui->lineEdit_2->setPlaceholderText("Enter your password");
    ui->lineEdit_2->setEchoMode(QLineEdit::Password);

    this->setStyleSheet("background-color: #2C3E50;");
    ui->lineEdit->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    ui->lineEdit_2->setStyleSheet(
        "QLineEdit { background-color: #2C3E50; border: none; border-bottom: 2px solid #2ECC71; color: white; }"
    );

    QString btnStyle = "QPushButton { border-radius: 20px; background-color: #2ECC71; color: white; font-weight: bold; } QPushButton:hover { background-color: #27AE60; }";
    ui->pushButton->setStyleSheet(btnStyle);
    ui->pushButton_2->setStyleSheet(btnStyle);
    ui->pushButton_3->setStyleSheet(
        "QPushButton { border: none; color: #95A5A6; text-decoration: underline; } QPushButton:hover { color: #ECF0F1; }"
    );

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::clickLoginButton);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::clickRegisterButton);
    connect(ui->pushButton_3, &QPushButton::clicked, this, &MainWindow::clickForgotPasswordButton);

    setupTodoListUI();
}

void MainWindow::setupTodoListUI() {
    ui->tableView->setStyleSheet(R"(
        QHeaderView::section {
            background-color: #eeeeee;
            padding: 4px;
            border: none;
            font-weight: bold;
        }
        QTableView {
            border: 1px solid #ccc;
            background-color: #FAFAFA;
            alternate-background-color: #F0F0F0;
            gridline-color: transparent;
            selection-background-color: #B3E5FC;
            selection-color: black;
            font-family: 'Segoe UI', sans-serif;
            font-size: 12px;
        }
        QTableView::item {
            padding: 6px;
        }
    )");

    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::exitApp);
    connect(ui->pushButton_5, &QPushButton::clicked, this, [=]() { filterTasks("To Do"); });
    connect(ui->pushButton_6, &QPushButton::clicked, this, [=]() { filterTasks("In Progress"); });
    connect(ui->pushButton_7, &QPushButton::clicked, this, [=]() { filterTasks("Ready"); });
    connect(ui->pushButton_9, &QPushButton::clicked, this, [=]() { filterTasks(); });
    connect(ui->pushButton_2_todo, &QPushButton::clicked, this, &MainWindow::removeSelectedTasks);
    connect(ui->pushButton_3_todo, &QPushButton::clicked, this, &MainWindow::clearTasks);
    connect(ui->pushButton_todo, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(ui->pushButton_logout, &QPushButton::clicked, this, &MainWindow::logout);

    applyButtonStyle();
}

void MainWindow::applyButtonStyle() {
    QStringList styles = {
        "QPushButton { background-color: #81D4FA; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #4FC3F7; }",
        "QPushButton { background-color: #FFD54F; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #FFCA28; }",
        "QPushButton { background-color: #81C784; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #66BB6A; }",
        "QPushButton { background-color: #E0E0E0; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #BDBDBD; }",
        "QPushButton { background-color: #E57373; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #EF5350; }",
        "QPushButton { background-color: #BDBDBD; color: black; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #9E9E9E; }",
        "QPushButton { background-color: #EF5350; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #E53935; }",
        "QPushButton { background-color: #4A90E2; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #357ABD; }",
        "QPushButton { background-color: #FF7043; color: white; border: none; border-radius: 10px; padding: 6px 12px; } QPushButton:hover { background-color: #F4511E; }"
    };

    QList<QPushButton*> buttons = {
        ui->pushButton_5,
        ui->pushButton_6,
        ui->pushButton_7,
        ui->pushButton_9,
        ui->pushButton_2_todo,
        ui->pushButton_3_todo,
        ui->pushButton_4,
        ui->pushButton_todo,
        ui->pushButton_logout
    };

    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setStyleSheet(styles[i]);
    }
}

MainWindow::~MainWindow() {
    db.close();
    delete ui;
    if (model) {
        delete model;
    }
}

QString MainWindow::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool MainWindow::registerUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QRegularExpression usernameRegex("^[a-zA-Z0-9]{4,}$");
    if (!usernameRegex.match(username).hasMatch()) {
        QMessageBox::warning(this, "Error", "Username must be at least 4 characters and contain only letters and numbers.");
        return false;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
    if (!passwordRegex.match(password).hasMatch()) {
        QMessageBox::warning(this, "Error",
                             "Password must be at least 6 characters long and include:\n"
                             "- One uppercase letter\n"
                             "- One lowercase letter\n"
                             "- One number");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT username FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + query.lastError().text());
        return false;
    }
    if (query.next()) {
        QMessageBox::warning(this, "Error", "Username already exists");
        return false;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO users (username, password_) VALUES (:username, :password_)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password_", hashPassword(password));
    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Error", "Registration failed: " + insertQuery.lastError().text());
        return false;
    }

    QMessageBox::information(this, "Success", "Registration successful");
    return true;
}

bool MainWindow::loginUser(const QString& username, const QString& password) {
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Username and password cannot be empty");
        return false;
    }

    QSqlQuery loginQuery(db);
    loginQuery.prepare("SELECT password_ FROM users WHERE username = :username");
    loginQuery.bindValue(":username", username);
    if (!loginQuery.exec()) {
        QMessageBox::warning(this, "Error", "Query failed: " + loginQuery.lastError().text());
        return false;
    }

    if (loginQuery.next()) {
        QString storedPassword = loginQuery.value(0).toString();
        if (storedPassword == hashPassword(password)) {
            currentUsername = username;
            setupTaskModel();
            ui->stackedWidget->setCurrentIndex(1);
            QMessageBox::information(this, "Success", "Login successful");
            return true;
        }
    }

    QMessageBox::warning(this, "Error", "Invalid username or password");
    return false;
}

void MainWindow::clickForgotPasswordButton() {
    QString username = ui->lineEdit->text();
    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter your username.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT password_ FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "Error checking username: " + query.lastError().text());
        return;
    }

    if (query.next()) {
        bool ok;
        QString newPassword = QInputDialog::getText(this, "Reset Password",
                                                   "Enter new password (at least 6 characters, 1 uppercase, 1 number):",
                                                   QLineEdit::Password, "", &ok);
        if (ok && !newPassword.isEmpty()) {
            QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
            if (!passwordRegex.match(newPassword).hasMatch()) {
                QMessageBox::warning(this, "Error",
                                     "Password must be at least 6 characters long and include:\n"
                                     "- One uppercase letter\n"
                                     "- One lowercase letter\n"
                                     "- One number");
                return;
            }

            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE users SET password_ = :password WHERE username = :username");
            updateQuery.bindValue(":password", hashPassword(newPassword));
            updateQuery.bindValue(":username", username);
            if (!updateQuery.exec()) {
                QMessageBox::critical(this, "Error", "Failed to update password: " + updateQuery.lastError().text());
                return;
            }

            QMessageBox::information(this, "Success", "Your password has been reset successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Invalid password entered.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Username not found.");
    }
}

void MainWindow::clickRegisterButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    registerUser(username, password);
}

void MainWindow::clickLoginButton() {
    QString username = ui->lineEdit->text();
    QString password = ui->lineEdit_2->text();
    loginUser(username, password);
}

void MainWindow::logout() {
    currentUsername = "";
    if (model) {
        delete model;
        model = nullptr;
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::setupTaskModel() {
    if (model) {
        delete model;
    }
    model = new QStandardItemModel(this);
    model->setColumnCount(4);
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Task");
    model->setHeaderData(2, Qt::Horizontal, "Status");
    model->setHeaderData(3, Qt::Horizontal, "Date");

    QSqlQuery query(db);
    query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString task = query.value(1).toString();
            QString status = query.value(2).toString();
            QString date = query.value(3).toString();

            QStandardItem* idItem = new QStandardItem(QString::number(id));
            idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            QStandardItem* taskItem = new QStandardItem(task);
            QStandardItem* statusItem = new QStandardItem(status);
            QStandardItem* dateItem = new QStandardItem(date);
            dateItem->setEditable(false);

            if (status.toLower() == "ready") {
                statusItem->setBackground(QColor("#A5D6A7"));
                taskItem->setForeground(QBrush(Qt::gray));
                QFont font = taskItem->font();
                font.setStrikeOut(true);
                taskItem->setFont(font);
            } else {
                statusItem->setBackground(QColor("#E0E0E0"));
            }

            model->appendRow({idItem, taskItem, statusItem, dateItem});
        }
    }

    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(2, new StatusDelegate(this));

    connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem *item) {
        int row = item->row();
        int id = model->item(row, 0)->text().toInt();

        if (item->column() == 1) {
            QString newTask = item->text();
            editTask(id, newTask);
        }

        if (item->column() == 2) {
            QString newStatus = item->text();
            QSqlQuery query(db);
            query.prepare("UPDATE tasks SET status = ? WHERE id = ? AND username = ?");
            query.addBindValue(newStatus);
            query.addBindValue(id);
            query.addBindValue(currentUsername);
            if (!query.exec()) {
                qDebug() << "Failed to update status:" << query.lastError().text();
            }

            QString status = newStatus.toLower();
            if (status == "ready") {
                item->setBackground(QColor("#A5D6A7"));
                model->item(row, 1)->setForeground(QBrush(Qt::gray));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(true);
                model->item(row, 1)->setFont(font);
            } else {
                item->setBackground(QColor("#E0E0E0"));
                model->item(row, 1)->setForeground(QBrush(Qt::black));
                QFont font = model->item(row, 1)->font();
                font.setStrikeOut(false);
                model->item(row, 1)->setFont(font);
            }
        }
    });

    ui->tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    ui->tableView->setAlternatingRowColors(true);
}

int MainWindow::findNextAvailableId() {
    QSqlQuery query(db);
    query.prepare("SELECT id FROM tasks WHERE username = ? ORDER BY id ASC");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to fetch IDs:" << query.lastError().text();
        return 1;
    }

    int expectedId = 1;
    while (query.next()) {
        int currentId = query.value(0).toInt();
        if (currentId != expectedId) {
            break;
        }
        expectedId++;
    }
    return expectedId;
}

void MainWindow::addTask() {
    QString taskText = ui->lineEdit_todo->text().trimmed();
    if (taskText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Task cannot be empty.");
        return;
    }

    QSqlQuery query(db);
    QString status = "To Do";
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    int newId = findNextAvailableId();

    query.prepare("INSERT INTO tasks (id, username, task, status, date) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(newId);
    query.addBindValue(currentUsername);
    query.addBindValue(taskText);
    query.addBindValue(status);
    query.addBindValue(dateStr);

    if (!query.exec()) {
        qDebug() << "Insert error:" << query.lastError().text();
        QMessageBox::critical(this, "Database Error", "Could not insert task.");
        return;
    }

    QStandardItem* idItem = new QStandardItem(QString::number(newId));
    idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QStandardItem* taskItem = new QStandardItem(taskText);
    taskItem->setEditable(true);
    QStandardItem* statusItem = new QStandardItem("To Do");
    statusItem->setEditable(true);
    statusItem->setBackground(QColor("#E0E0E0"));
    QStandardItem* dateItem = new QStandardItem(dateStr);
    dateItem->setEditable(false);

    model->appendRow({idItem, taskItem, statusItem, dateItem});
    ui->lineEdit_todo->clear();
}

void MainWindow::editTask(int id, const QString& newTask) {
    QSqlQuery query(db);
    query.prepare("UPDATE tasks SET task = ? WHERE id = ? AND username = ?");
    query.addBindValue(newTask);
    query.addBindValue(id);
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Failed to update task name:" << query.lastError().text();
        QMessageBox::warning(this, "Update Error", "Failed to update the task name.");
    }
}

void MainWindow::removeSelectedTasks() {
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No selection", "Please select a task to remove.");
        return;
    }

    std::sort(selected.begin(), selected.end(), [](const QModelIndex& a, const QModelIndex& b) {
        return a.row() > b.row();
    });

    for (const QModelIndex &index : selected) {
        int id = model->item(index.row(), 0)->text().toInt();
        QSqlQuery query(db);
        query.prepare("DELETE FROM tasks WHERE id = ? AND username = ?");
        query.addBindValue(id);
        query.addBindValue(currentUsername);
        if (!query.exec()) {
            qDebug() << "Delete error:" << query.lastError().text();
        }
        model->removeRow(index.row());
    }
}

void MainWindow::filterTasks(const QString& status) {
    model->removeRows(0, model->rowCount());

    QSqlQuery query(db);
    if (status.isEmpty()) {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE username = ?");
        query.addBindValue(currentUsername);
    } else {
        query.prepare("SELECT id, task, status, date FROM tasks WHERE status = ? AND username = ?");
        query.addBindValue(status);
        query.addBindValue(currentUsername);
    }

    if (!query.exec()) {
        qDebug() << "Failed to fetch tasks:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int id = query.value(0).toInt();
        QString task = query.value(1).toString();
        QString stat = query.value(2).toString();
        QString date = query.value(3).toString();

        QStandardItem* idItem = new QStandardItem(QString::number(id));
        idItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        QStandardItem* taskItem = new QStandardItem(task);
        QStandardItem* statusItem = new QStandardItem(stat);
        QStandardItem* dateItem = new QStandardItem(date);
        dateItem->setEditable(false);

        if (stat.toLower() == "ready") {
            statusItem->setBackground(QColor("#A5D6A7"));
            taskItem->setForeground(QBrush(Qt::gray));
            QFont font = taskItem->font();
            font.setStrikeOut(true);
            taskItem->setFont(font);
        } else {
            statusItem->setBackground(QColor("#E0E0E0"));
        }

        model->appendRow({idItem, taskItem, statusItem, dateItem});
    }
}

void MainWindow::clearTasks() {
    QSqlQuery query(db);
    query.prepare("DELETE FROM tasks WHERE username = ?");
    query.addBindValue(currentUsername);
    if (!query.exec()) {
        qDebug() << "Clear tasks error:" << query.lastError().text();
    }
    model->removeRows(0, model->rowCount());
}

void MainWindow::exitApp() {
    QApplication::quit();
}
